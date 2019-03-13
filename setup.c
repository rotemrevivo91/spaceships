#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_image.h>

#include "constructors.h"
#include "graphics.h"
#include "mainmenu.h"
#include "newgame.h"
#include "highscore.h"

Information info;

void createWindow(){

    if(SDL_Init(SDL_INIT_EVERYTHING) == -1)
        SDL_Log(SDL_GetError());
    if(TTF_Init() == -1)
        SDL_Log(TTF_GetError());
    if(IMG_Init(IMG_INIT_PNG))
        SDL_Log(IMG_GetError());
    if(Mix_Init(MIX_INIT_MP3))
        SDL_Log(Mix_GetError());
    if( Mix_OpenAudio( 22050, MIX_DEFAULT_FORMAT, 2, 4096 ) == -1 )
        SDL_Log(Mix_GetError());

    info.startTime = SDL_GetTicks()/1000;

    // Get screen's resolution
    for(int i = 0; i < SDL_GetNumVideoDisplays(); ++i){
        int should_be_zero = SDL_GetCurrentDisplayMode(i, &info.current);

        if(should_be_zero != 0)
            SDL_Log("Could not get display mode for video display #%d: %s", i, SDL_GetError());
        else
            SDL_Log("Display #%d: current display mode is %dx%dpx @ %dhz.", i, info.current.w, info.current.h, info.current.refresh_rate);
  }

    // Create an application window with the following settings:
    info.window = SDL_CreateWindow(
        "Spaceships wars - demo",                  // window title
        SDL_WINDOWPOS_UNDEFINED,           // initial x position
        SDL_WINDOWPOS_UNDEFINED,           // initial y position
        info.current.w,                               // width, in pixels
        info.current.h,                               // height, in pixels
        SDL_WINDOW_FULLSCREEN_DESKTOP//SDL_WINDOW_FULLSCREEN                  // flags - see below
    );

    // Check that the window was successfully created
    if (info.window == NULL) {
        // In the case that the window could not be made...
        printf("Could not create window: %s\n", SDL_GetError());
    }

    //setup info
    setupInfo(&info);
    //Start the main menu
    gameOptions();
    // Close and destroy the window
    SDL_DestroyWindow(info.window);
    // Clean up
    clearAll(info);
    Mix_CloseAudio();
    IMG_Quit();
    TTF_CloseFont(info.font);
    //TTF_Quit();
    SDL_QuitSubSystem(SDL_INIT_EVERYTHING);
    SDL_Quit();
}

void setupInfo(Information *info){
    info->renderer = SDL_CreateRenderer(info->window , -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    info->bitmapSurface = SDL_LoadBMP("files/background.bmp");
    if(info->bitmapSurface == NULL)
        SDL_Log("Could not load background image");
    info->bitmapTex = SDL_CreateTextureFromSurface(info->renderer, info->bitmapSurface);
    SDL_FreeSurface(info->bitmapSurface);
    info->state = MAIN_MENU;
    //default music
    info->music = Mix_LoadWAV("files/game1.wav");
    if (Mix_PlayChannel( -1,info->music, 1) == -1)
        SDL_GetError();
}

//main loop of the program.
//The game has 4 different states: MAIN_MENU, START_GAME, OPTIONS, HIGHSCORES
//each case initiate a new screen. When the new screen terminates, if it terminated with returning 1
//it means to stay in the game, and hence stay in the loop. If 0 was returned finish the loop, hence quit the game.
void gameOptions(){
    setupMainMenu(&info);
    int result = createMainMenu(&info);
    clearMainMenu(&info);
    while(result == 1){
        switch(info.state){
            case MAIN_MENU:
                setupMainMenu(&info);
                result = createMainMenu(&info);
                clearMainMenu(&info);
                break;
            case START_GAME:
                setupLevel1(&info);
                result = createNewGame(&info);
                clearNewGame(&info);
                break;
            case OPTIONS:
                setupOptions(&info);
                result = createOptions(&info);
                clearOptions(&info);
                break;
            case HIGHSCORES:
                setupHighscores(&info);
                result = showhs(&info);
                clearHighscores(&info);
                break;
        }
    }
}

void setupLevel1(Information *info){
    SDL_FlushEvents(SDLK_SPACE, SDL_LASTEVENT);
    //New Game
    info->newGame = malloc(sizeof(NewGame));
    info->newGame->isPaused = false;
    info->newGame->isDeathscreen = false;
    info->newGame->gameStartTime = 0;
    info->newGame->mins = 0;
    info->newGame->tts = malloc(5 * sizeof(TextToScreen));
    info->newGame->tts[0].text = malloc(7 * sizeof(char));
    info->newGame->tts[1].text = malloc(5 * sizeof(char));
    info->newGame->tts[2].text = malloc(10 * sizeof(char));
    info->newGame->tts[3].text = malloc(40 * sizeof(char));
    info->newGame->tts[4].text = malloc(40 * sizeof(char));
    //Player
    info->player = malloc(sizeof(Player));
    if(info->player == NULL)
        SDL_Log("Failed allocating player.");
    info->player->character = malloc(sizeof(Character));
    if(info->player->character == NULL)
        SDL_Log("Failed allocating player's character");
    if(info->player->character == NULL)
        SDL_Log("Failed allocating player's character.");
    info->player->character->rect.w = info->current.w/10;
    info->player->character->rect.h = info->current.h/10;
    info->player->character->rect.x = info->current.w/2 - info->player->character->rect.w/2;
    info->player->character->rect.y = info->current.h - info->current.h/64 - info->player->character->rect.h;
    info->player->lives = 3;
    info->player->points = 0;
    info->player->character->speed = 15;
    info->player->character->isFiring = false;
    info->player->character->isRespawned = false;
    info->player->character->texture = IMG_LoadTexture(info->renderer,"files/spaceship3.png");
    if(info->player->character->texture == NULL)
        SDL_Log("Failed loading player's image");
    //weapons
    info->player->character->l = malloc(3 * sizeof(Laser));
    if(info->player->character->l == NULL)
        SDL_Log("Failed allocating laser");
    info->player->character->l->speed = 15;
    info->player->character->l->isActive = false;
    info->player->character->l->sound = Mix_LoadWAV("files/laser1.wav");
    if(info->player->character->l->sound == NULL)
        SDL_Log("Failed loading player's laser sound");
    info->player->character->l->texture = IMG_LoadTexture(info->renderer,"files/beams.png");
    if(info->player->character->l->texture == NULL)
        SDL_Log("Failed loading player's laser image");
    info->player->character->l->rect.w = info->player->character->rect.w/5;
    info->player->character->l->rect.h = info->player->character->rect.h/5;
    info->player->character->l->rect.x = info->player->character->rect.x + info->player->character->rect.w/2;
    info->player->character->l->rect.y = info->player->character->rect.y;
    info->player->character->l->crop.w = info->player->character->rect.w/5;
    info->player->character->l->crop.h = info->player->character->rect.h/2;
    info->player->character->l->crop.x = 166;
    info->player->character->l->crop.y = 217;
    //enemie
    info->enemies = malloc(ESIZE * sizeof(Enemy));
    if(info->enemies == NULL)
        SDL_Log("Failed allocating enemies");
    for(int i=0;i<ESIZE;i++){
        info->enemies[i].isPoint = false;
        info->enemies[i].deathCounter = 0;
        info->enemies[i].character = malloc(sizeof(Character));
        info->enemies[i].character->rect.x = 10 + i*(info->current.w/8);
        info->enemies[i].character->rect.y = info->current.h/100 + info->player->character->rect.h;
        info->enemies[i].character->rect.w = info->player->character->rect.w;
        info->enemies[i].character->rect.h = info->player->character->rect.h;
        info->enemies[i].character->speed = 0;
        info->enemies[i].currentSpeed = 0;
        info->enemies[i].character->isHit = false;
        info->enemies[i].character->isFiring = false;
        info->enemies[i].character->isRespawned = false;
        info->enemies[i].character->texture = IMG_LoadTexture(info->renderer,"files/spaceship1.png");
        if(info->enemies[i].character->texture == NULL)
            SDL_Log("Failed loading enemy %d image",i);
        info->enemies[i].character->l = malloc(sizeof(Laser));
        info->enemies[i].character->l->speed = 1;
        info->enemies[i].character->l->isActive = false;
        info->enemies[i].character->l->sound = Mix_LoadWAV("files/laser1.wav");
        if(info->enemies[i].character->l->sound == NULL)
            SDL_Log("Failed loading enemies's laser sound");
        info->enemies[i].character->l->texture = IMG_LoadTexture(info->renderer,"files/beams.png");
        if(info->enemies[i].character->l->texture == NULL)
            SDL_Log("Failed loading enemy's %d laser image",i);
        info->enemies[i].character->l->rect.w = info->enemies[i].character->rect.w/5;
        info->enemies[i].character->l->rect.h = info->enemies[i].character->rect.h/5;
        info->enemies[i].character->l->rect.x = info->enemies[i].character->rect.x + info->enemies[i].character->rect.w/2;
        info->enemies[i].character->l->rect.y = info->enemies[i].character->rect.y;
        info->enemies[i].character->l->crop.w = 30;
        info->enemies[i].character->l->crop.h = 25;
        info->enemies[i].character->l->crop.x = 37;
        info->enemies[i].character->l->crop.y = 110;
    }
    //explosion
    info->explosion = malloc(sizeof(Explosion));
    if(info->explosion == NULL)
        SDL_Log("Failed allocating explosion");
    info->explosion->texture = IMG_LoadTexture(info->renderer,"files/explosion.png");
    if(info->explosion->texture == NULL)
        SDL_Log("Failed loading explosion's image");
    info->explosion->ImageRect.w = 64;
    info->explosion->ImageRect.h = 64;
    info->explosion->ScreenRect.w = info->player->character->rect.w;
    info->explosion->ScreenRect.h = info->player->character->rect.h;
    info->explosion->Frames = 16;
    info->explosion->isActive = false;
}

void setupMainMenu(Information *info){
    //clear even queue
    SDL_FlushEvent(SDL_LASTEVENT);
    //allocating the menu
    info->mmenu = malloc(MSIZE * sizeof (MenuOptions));
    //Menu's headline
    info->fontSize = info->current.h / 10;
    info->font = TTF_OpenFont("files/font.ttf",info->fontSize);
    if(info->font == NULL)
        SDL_Log("Could not load the font");
    info->mmenu[0].tts.text = "SPACESHIPS";
    info->mmenu[0].tts.word_length = strlen(info->mmenu[0].tts.text) * (info->fontSize-20);
    info->mmenu[0].tts.rect.x = (info->current.w - info->mmenu[0].tts.word_length)/2;
    info->mmenu[0].tts.rect.y = (info->current.h/10);
    info->mmenu[0].tts = drawText(info,info->mmenu[0].tts.text,info->mmenu[0].tts.rect.x,info->mmenu[0].tts.rect.y,DEFCOL);
    //Menu's options
    info->mmenu[1].isPressed = true;
    for(int i=2;i<MSIZE;i++)
        info->mmenu[i].isPressed=false;

    //Setup the font
    TTF_CloseFont(info->font);
    info->fontSize = info->current.h / 20;
    info->font = TTF_OpenFont("files/font.ttf",info->fontSize);
    if(info->font == NULL)
        SDL_Log("Could not load the font");

    info->mmenu[1].tts.text = "Start Game";
    info->mmenu[1].tts.word_length = strlen(info->mmenu[1].tts.text) * (info->fontSize - 10);
    info->mmenu[1].tts.rect.x = (info->current.w - info->mmenu[1].tts.word_length)/2;
    info->mmenu[1].tts.rect.y = (info->current.h/10)*3;
    info->mmenu[1].tts = drawText(info,info->mmenu[1].tts.text,info->mmenu[1].tts.rect.x,info->mmenu[1].tts.rect.y,PRECOL);
    info->mmenu[2].tts.text = "Music options";
    info->mmenu[2].tts.word_length = strlen(info->mmenu[2].tts.text) * (info->fontSize - 10);
    info->mmenu[2].tts.rect.x = (info->current.w - info->mmenu[2].tts.word_length)/2;
    info->mmenu[2].tts.rect.y = (info->current.h/10)*5;
    info->mmenu[2].tts = drawText(info,info->mmenu[2].tts.text,info->mmenu[2].tts.rect.x,info->mmenu[2].tts.rect.y,DEFCOL);
    info->mmenu[3].tts.text = "Top scores";
    info->mmenu[3].tts.word_length = strlen(info->mmenu[3].tts.text) * (info->fontSize - 10);
    info->mmenu[3].tts.rect.x = (info->current.w - info->mmenu[3].tts.word_length)/2;
    info->mmenu[3].tts.rect.y = (info->current.h/10)*7;
    info->mmenu[3].tts = drawText(info,info->mmenu[3].tts.text,info->mmenu[3].tts.rect.x,info->mmenu[3].tts.rect.y,DEFCOL);
    info->mmenu[4].tts.text = "Exit";
    info->mmenu[4].tts.word_length = strlen(info->mmenu[4].tts.text) * (info->fontSize - 10);
    info->mmenu[4].tts.rect.x = (info->current.w - info->mmenu[4].tts.word_length)/2;
    info->mmenu[4].tts.rect.y = (info->current.h/10)*9;
    info->mmenu[4].tts = drawText(info,info->mmenu[4].tts.text,info->mmenu[4].tts.rect.x,info->mmenu[4].tts.rect.y,DEFCOL);
}

void setupHighscores(Information *info){
    info->highscores = malloc(sizeof(Highscores));
    info->highscores->position=0;
    info->highscores->newHSFound = false;
    info->highscores->tts[13].text = malloc(4 * sizeof(char));
    for(int i =0 ;i<strlen(info->highscores->names);i++)
        info->highscores->names[i] = 0;

    readFile(info);
}

void setupOptions(Information *info){

    //Event clearen
    SDL_FlushEvent(SDL_LASTEVENT);
    //Speicherplatzreservierung
    info->options = malloc(OSIZE * sizeof (MenuOptions));
    //Erstes Element angewählt (isPressed)
    info->options[1].isPressed = true;
    for(int i=2;i<OSIZE;i++) info->options[i].isPressed=false;

    //Schrift
    info->fontSize = info->current.h / 10;
    info->font = TTF_OpenFont("files/font.ttf",info->fontSize);
    if(info->font == NULL)
        SDL_Log("Could not load the font");
    info->options[0].tts.text = "MUSIC OPTIONS";
    info->options[0].tts.word_length = strlen(info->options[0].tts.text) * (info->fontSize -20);
    info->options[0].tts.rect.x = (info->current.w - info->options[0].tts.word_length)/2;
    info->options[0].tts.rect.y = (info->current.h/(2*OSIZE));
    info->options[0].tts = drawText(info,info->options[0].tts.text,info->options[0].tts.rect.x,info->options[0].tts.rect.y,DEFCOL);

    TTF_CloseFont(info->font);                      //Schriftgröße 30(normale Schrift)
    info->fontSize = info->current.h / 20;
    info->font = TTF_OpenFont("files/font.ttf",info->fontSize);
    if(info->font == NULL)
        SDL_Log("Could not load the font");
    info->options[1].tts.text = "H2O Drops (Chill/Electro)";
    info->options[1].tts.word_length = strlen(info->options[1].tts.text) * (info->fontSize - 10);
    info->options[1].tts.rect.x = (info->current.w - info->options[1].tts.word_length)/2;
    info->options[1].tts.rect.y = (info->current.h/(2*OSIZE))*4;
    info->options[1].tts = drawText(info,info->options[1].tts.text,info->options[1].tts.rect.x,info->options[1].tts.rect.y,PRECOL);
    info->options[2].tts.text = "Miss Lady (Reggae/Hip Hop)";
    info->options[2].tts.word_length = strlen(info->options[2].tts.text) * (info->fontSize - 10);
    info->options[2].tts.rect.x = (info->current.w - info->options[2].tts.word_length)/2;
    info->options[2].tts.rect.y = (info->current.h/(2*OSIZE))*5.4;
    info->options[2].tts = drawText(info,info->options[2].tts.text,info->options[2].tts.rect.x,info->options[2].tts.rect.y,DEFCOL);
    info->options[3].tts.text = "Dancing Leopard (Jazz/Blues)";
    info->options[3].tts.word_length = strlen(info->options[3].tts.text) * (info->fontSize - 10);
    info->options[3].tts.rect.x = (info->current.w - info->options[3].tts.word_length)/2;
    info->options[3].tts.rect.y = (info->current.h/(2*OSIZE))*6.8;
    info->options[3].tts = drawText(info,info->options[3].tts.text,info->options[3].tts.rect.x,info->options[3].tts.rect.y,DEFCOL);
    info->options[4].tts.text = "Game Music (Standart)";
    info->options[4].tts.word_length = strlen(info->options[4].tts.text) * (info->fontSize - 10);
    info->options[4].tts.rect.x = (info->current.w - info->options[4].tts.word_length)/2;
    info->options[4].tts.rect.y = (info->current.h/(2*OSIZE))*8.2;
    info->options[4].tts = drawText(info,info->options[4].tts.text,info->options[4].tts.rect.x,info->options[4].tts.rect.y,DEFCOL);
    if(Mix_Paused(-1) == 0)
        info->options[5].tts.text = "Music Off";
    else
        info->options[5].tts.text = "Music On";
    info->options[5].tts.word_length = strlen(info->options[5].tts.text) * (info->fontSize - 10);
    info->options[5].tts.rect.x = (info->current.w - info->options[5].tts.word_length)/2;
    info->options[5].tts.rect.y = (info->current.h/(2*OSIZE))*10.6;
    info->options[5].tts = drawText(info,info->options[5].tts.text,info->options[5].tts.rect.x,info->options[5].tts.rect.y,DEFCOL);
    info->options[6].tts.text = "Back to Menu";
    info->options[6].tts.word_length = strlen(info->options[6].tts.text) * (info->fontSize - 10);
    info->options[6].tts.rect.x = (info->current.w - info->options[6].tts.word_length)/2;
    info->options[6].tts.rect.y = (info->current.h/(2*OSIZE))*12;
    info->options[6].tts = drawText(info,info->options[6].tts.text,info->options[6].tts.rect.x,info->options[6].tts.rect.y,DEFCOL);
}


void clearNewGame(Information *info){
    SDL_DestroyTexture(info->newGame->tts->texture);
    info->newGame = NULL;
    free(info->newGame);
    //Free Explosion
    SDL_DestroyTexture(info->explosion->texture);
    info->explosion = NULL;
    free(info->explosion);
    //Free player
    SDL_DestroyTexture(info->player->character->texture);
    SDL_DestroyTexture(info->player->character->l->texture);
    Mix_FreeChunk(info->player->character->l->sound);
    info->player->character->l = NULL;
    free(info->player->character->l);
    info->player->character = NULL;
    free(info->player->character);
    info->player = NULL;
    free(info->player);
    //Free Enemies
    for(int i=0;i<ESIZE;i++){
        SDL_DestroyTexture(info->enemies[i].character->texture);
        SDL_DestroyTexture(info->enemies[i].character->l->texture);
        Mix_FreeChunk(info->enemies[i].character->l->sound);
        info->enemies[i].character->l = NULL;
        free(info->enemies[i].character->l);
        info->enemies[i].character = NULL;
        free(info->enemies[i].character);
    }
    info->enemies = NULL;
    free(info->enemies);
}

void clearMainMenu(Information *info){
    for(int i=0;i<MSIZE;i++)
        SDL_DestroyTexture(info->mmenu[i].tts.texture);
    info->mmenu = NULL;
    free(info->mmenu);
}

void clearHighscores(Information *info){
    for(int i=0;i<40;i++){
        info->highscores->names[i] = NULL;
        if(i<10)
            info->highscores->hscores[i] = NULL;
    }
}

void clearOptions(Information *info){
    for(int i=0;i<OSIZE;i++)
        SDL_DestroyTexture(info->options[i].tts.texture);
    info->options = NULL;
    free(info->options);
}

void clearAll(Information *info){
    //Destory Renderer + Background
    SDL_DestroyRenderer(info->renderer);
    SDL_DestroyTexture(info->bitmapTex);
    //Free music
    Mix_FreeChunk(info->music);
    //Destroy renderer
    SDL_DestroyRenderer(info->renderer);
}


