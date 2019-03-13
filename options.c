#include "setup.h"
#include "graphics.h"

int createOptions(Information *info){
    int row = 1;
    int i=0; //für for-Schleifen
    while(row == 1){
        if(SDL_PollEvent(&info->event)){
            switch(info->event.type){
                case SDL_QUIT:
                    return 0;
                    break;
                case SDL_KEYDOWN:
                    switch(info->event.key.keysym.sym){
                        case SDLK_DOWN:
                        for(i=1;i<OSIZE;i++){
                            if(info->options[i].isPressed){
                                info->options[i].isPressed=false;
                                if(i==(OSIZE-1)){
                                    info->options[1].isPressed=true;
                                    info->options[(OSIZE-1)].tts.color = DEFCOL;
                                    info->options[1].tts.color = PRECOL;
                                }
                                else{
                                    info->options[i+1].isPressed=true;
                                    info->options[i].tts.color = DEFCOL;
                                    info->options[i+1].tts.color = PRECOL;
                                }
                                break;
                            }
                        }
                        for(i=1;i<OSIZE;i++)
                            info->options[i].tts = drawText(info,info->options[i].tts.text,info->options[i].tts.rect.x,info->options[i].tts.rect.y,info->options[i].tts.color);
                        break;

                        case SDLK_UP:
                            for(i=1;i<OSIZE;i++){
                                if(info->options[i].isPressed){
                                    info->options[i].isPressed=false;
                                if(i==1){
                                    info->options[(OSIZE-1)].isPressed=true;
                                    info->options[1].tts.color = DEFCOL;
                                    info->options[(OSIZE-1)].tts.color = PRECOL;
                                }
                                else{
                                    info->options[i-1].isPressed=true;
                                    info->options[i].tts.color = DEFCOL;
                                    info->options[i-1].tts.color = PRECOL;
                                }
                                break;
                                }
                            }
                            for(i=1;i<OSIZE;i++)
                                info->options[i].tts = drawText(info,info->options[i].tts.text,info->options[i].tts.rect.x,info->options[i].tts.rect.y,info->options[i].tts.color);
                            break;

                        case SDLK_SPACE:
                            for(i=1;i<OSIZE;i++){
                                if(info->options[i].isPressed){
                                    switch (i){
                                        case 1: //Play H2O Drops
                                            Mix_FreeChunk(info->music);
                                            info->music = Mix_LoadWAV("files/H2O.wav");
                                            if (Mix_PlayChannel( -1,info->music, 1) == -1)
                                                SDL_Log(SDL_GetError());
                                            break;
                                        case 2: //Play MissLady
                                            Mix_FreeChunk(info->music);
                                            info->music = Mix_LoadWAV("files/MissLady.wav");
                                            if (Mix_PlayChannel( -1,info->music, 1) == -1)
                                                SDL_Log(SDL_GetError());
                                            break;
                                        case 3: //Play Dancing Leo
                                            Mix_FreeChunk(info->music);
                                            info->music = Mix_LoadWAV("files/Leopard.wav");
                                            if (Mix_PlayChannel( -1,info->music, 1) == -1)
                                                SDL_Log(SDL_GetError());
                                            break;
                                        case 4: //Play Standart Music
                                            Mix_FreeChunk(info->music);
                                            info->music = Mix_LoadWAV("files/game1.wav");
                                            if (Mix_PlayChannel( -1,info->music, 1) == -1)
                                                SDL_Log(SDL_GetError());
                                            break;
                                        case 5: //Music Off (Channel 1 für Music, andere für Sounds)
                                            if(Mix_Paused(-1) == 0){
                                                Mix_Pause(-1);
                                                info->options[5].tts.text = "Music On";
                                                info->options[5].tts.word_length = strlen(info->options[5].tts.text) * (info->fontSize - 10);
                                                info->options[5].tts.rect.x = (info->current.w - info->options[5].tts.word_length)/2;
                                                info->options[5].tts.rect.y = (info->current.h/(2*OSIZE))*10.6;
                                                info->options[5].tts = drawText(info,info->options[5].tts.text,info->options[5].tts.rect.x,info->options[5].tts.rect.y,PRECOL);
                                            }
                                            else{
                                                Mix_Resume(-1);
                                                info->options[5].tts.text = "Music Off";
                                                info->options[5].tts.word_length = strlen(info->options[5].tts.text) * (info->fontSize - 10);
                                                info->options[5].tts.rect.x = (info->current.w - info->options[5].tts.word_length)/2;
                                                info->options[5].tts.rect.y = (info->current.h/(2*OSIZE))*10.6;
                                                info->options[5].tts = drawText(info,info->options[5].tts.text,info->options[5].tts.rect.x,info->options[5].tts.rect.y,PRECOL);
                                            }

                                            break;
                                        case 6: // Back to Main Menu
                                            info->state = MAIN_MENU;
                                            return 1;
                                            break;
                                    }
                                }
                            }
                            break;

                            case SDLK_ESCAPE:
                                info->state = MAIN_MENU;
                                return 1;
                                break;

                    }
            }
        }
        // Bild immer aktualisieren (bei jeder while Schleife) für das ausgewählte, rote Element
        SDL_RenderClear(info->renderer);
        SDL_RenderCopy(info->renderer, info->bitmapTex, NULL, NULL);
        for(int i=0;i<OSIZE;i++) SDL_RenderCopy(info->renderer,info->options[i].tts.texture,NULL,&info->options[i].tts.rect);
        SDL_RenderPresent(info->renderer);

    }
    return 0;
}
