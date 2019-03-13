#include "graphics.h"
#include "setup.h"
#include "highscore.h"

int createNewGame(Information *info){
    //getting the keys' state
    const Uint8 *keystates = SDL_GetKeyboardState( NULL );

    int r=1;
    int RowEnemies = -1;
    int RowPlayer = -1;

    info->font = TTF_OpenFont("files/font.ttf",30);
    //game loop
    SDL_FlushEvents(SDL_SCANCODE_SPACE, SDL_LASTEVENT);
    while(r==1){
        if(keystates[SDL_SCANCODE_SPACE] &&!info->newGame->isPaused && !info->newGame->isDeathscreen)
            setShooting(info);
        if(keystates[SDL_SCANCODE_RIGHT] &&!info->newGame->isPaused && !info->newGame->isDeathscreen)
            info->player->isRight = true;
        if(keystates[SDL_SCANCODE_LEFT] &&!info->newGame->isPaused && !info->newGame->isDeathscreen)
            info->player->isLeft = true;
        if(keystates[SDL_SCANCODE_ESCAPE] && !info->newGame->isDeathscreen)
            info->newGame->isPaused = true;
        //stay or leave the game
        if(info->newGame->isPaused){
            drawNewScreen(info, 4,"QUIT TO MAIN MENU? Y/N");
            if(keystates[SDL_SCANCODE_Y]){
                SDL_FlushEvents(SDL_SCANCODE_SPACE, SDL_LASTEVENT);
                info->state = MAIN_MENU;
                info->newGame->isPaused = false;
                return 1;
            }
            else if(keystates[SDL_SCANCODE_N]){
                info->newGame->isPaused = false;
                info->startTime = info->newGame->gameStartTime;
            }
        }
        //deathscreen
        if(info->newGame->isDeathscreen){
            drawNewScreen(info, 3,"YOU LOST");
            if(keystates[SDL_SCANCODE_ESCAPE]){
                SDL_FlushEvents(SDL_SCANCODE_ESCAPE, SDL_LASTEVENT);
                setupHighscores(info);
                for(int i=0;i<10;i++){
                    if (info->player->points > info->highscores->hscores[i]) {
                        newhs(info,info->player->points,i);
                        break;
                    }
                }
                info->state = MAIN_MENU;
                return 1;
            }
        }
        SDL_PumpEvents();

        //render all if game is not paused
        if(!info->newGame->isPaused){
            //}
            //clear the screen before setting it up
            SDL_RenderClear(info->renderer);
            //////////////////////////////////////////////////////////////////////////////////////////////////////
            // Setting initial game screen
            //
            //Background
            if(SDL_RenderCopy(info->renderer, info->bitmapTex, NULL, NULL) == -1)
                SDL_Log(SDL_GetError());
            //render all if not deathscreen
            if(!info->newGame->isDeathscreen){
                //Status bar
                setupStatusBar(info);
                for(int i=0;i<3;i++)
                    if(SDL_RenderCopy(info->renderer, info->newGame->tts[i].texture, NULL, &info->newGame->tts[i].rect) == -1)
                        SDL_Log("HEREX: %d %s",i,SDL_GetError());
                //player
                if(SDL_RenderCopy (info->renderer, info->player->character->texture, NULL, &info->player->character->rect) == -1)
                    SDL_Log(SDL_GetError());
                //enemies
                for(int i=0;i<8;i++){
                    if(SDL_RenderCopy (info->renderer, info->enemies[i].character->texture, NULL, &info->enemies[i].character->rect) == -1)
                        SDL_Log(SDL_GetError());
                }
                //////////////////////////////////////////////////////////////////////////////////////////////////////
                // player and enemies properties:
                //
                //player movement
                if(info->player->isRight){
                    movePlayer(info,2);
                    info->player->isRight = false;
                }

                if(info->player->isLeft){
                    movePlayer(info,1);
                    info->player->isLeft = false;
                }
                //enemy movement
                enemyMovement(info);
                //player shooting
                shooting(info);
                //enemy shooting
                enemyShooting(info);
                //////////////////////////////////////////////////////////////////////////////////////////////////////
                //game properties:
                //
                //collision + explosion of enemies
                for(int i=0;i<8;i++){
                    //if the laser that killed the enemy is no longer active, the enemy will no longer considered as "just spawned"
                    if(!info->player->character->l->isActive)
                        info->enemies[i].character->isRespawned = false;
                    //if there was a collision between the player's laser and an enemy, as long as this enemy was not just hit and respawned:
                    if(detectCollision(info->player->character->l->rect, info->enemies[i].character->rect) && !info->enemies[i].character->isHit
                       && !info->enemies[i].character->isRespawned){
                        //set enemy as hit
                        info->enemies[i].character->isHit = true;
                        info->explosion->isActive = true;
                        info->explosion->ScreenRect = info->enemies[i].character->rect;
                        //respawn enemy
                        updateEnemy(info,i);
                        //update player's points
                        info->player->points++;
                        //player's speed
                        switch (info->player->points){
                            case 10:
                                info->player->character->speed++;
                            break;
                            case 50:
                                info->player->character->speed++;
                            break;
                            case 100:
                                info->player->character->speed++;
                            break;
                        }
                        break;
                    }
                }
                //collision and explosion of player
                for(int i=0;i<ESIZE;i++){
                    if(detectCollision(info->enemies[i].character->l->rect,info->player->character->rect) && !info->player->character->isRespawned){
                        info->player->character->isHit = true;
                        info->player->character->isRespawned = true;
                        info->player->lives--;
                        info->enemies[i].isPoint = true;
                        //explosion
                        info->player->character->isHit = true;
                        info->explosion->isActive = true;
                        info->explosion->ScreenRect = info->player->character->rect;
                    }
                    //player to bo hitted only once while the projectile pass through it
                    if(info->enemies[i].character->l->rect.y <= info->enemies[i].character->rect.y && info->enemies[i].isPoint){
                        info->player->character->isRespawned = false;
                        info->enemies[i].isPoint = false;
                        info->player->points--;
                    }

                }
                //death
                if(info->player->lives == 0)
                    info->newGame->isDeathscreen = true;
                //explosion
                RowEnemies = updateExplosion(RowEnemies,info);
                RowPlayer = updateExplosion(RowPlayer,info);
                //////////////////////////////////////////////////////////////////////////////////////////////////////
                //show all to screen
                SDL_RenderPresent(info->renderer);
            }
        }
    }
    return 1;
}

void setupStatusBar(Information *info){
    info->fontSize = info->current.h / 30;
    info->font = TTF_OpenFont("files/font.ttf",info->fontSize);
    if(info->font == NULL)
        SDL_Log("Could not load the font");
    //Lives bar
    sprintf(info->newGame->tts[0].text, "Lives: %d", info->player->lives);
    info->newGame->tts[0].word_length = strlen(info->newGame->tts[0].text) * (info->fontSize - 20);
    info->newGame->tts[0].rect.x = 10;
    info->newGame->tts[0].rect.y = info->current.h/100;
    info->newGame->tts[0] = drawText(info,info->newGame->tts[0].text,info->newGame->tts[0].rect.x,info->newGame->tts[0].rect.y,DEFCOL);
    //Time bar
    uint32_t t=0;
    if(SDL_GetTicks() != 0)
        t = SDL_GetTicks()/1000;
    info->newGame->gameStartTime =  t - info->newGame->mins*60;

    if(info->newGame->gameStartTime % 60 == 0 && info->newGame->gameStartTime > 0)
        info->newGame->mins++;

    if(info->newGame->gameStartTime < 10 && info->newGame->mins < 10)
        sprintf(info->newGame->tts[1].text, "0%d:0%lu" , info->newGame->mins,info->newGame->gameStartTime);
    else if(info->newGame->gameStartTime < 10)
        sprintf(info->newGame->tts[1].text, "%d:%0lu" , info->newGame->mins,info->newGame->gameStartTime);
    else if(info->newGame->mins < 10)
        sprintf(info->newGame->tts[1].text, "0%d:%lu" , info->newGame->mins,info->newGame->gameStartTime);
    else
        sprintf(info->newGame->tts[1].text, "%d:%lu" , info->newGame->mins,info->newGame->gameStartTime);

    info->newGame->tts[1].word_length = strlen(info->newGame->tts[1].text) * (info->fontSize - 20);
    info->newGame->tts[1].rect.x = (info->current.w - info->newGame->tts[1].word_length)/2;
    info->newGame->tts[1].rect.y = info->current.h/100;
    info->newGame->tts[1] = drawText(info,info->newGame->tts[1].text,info->newGame->tts[1].rect.x,info->newGame->tts[1].rect.y,DEFCOL);
    //Points bar
    sprintf(info->newGame->tts[2].text, "Points: %d" , info->player->points);
    info->newGame->tts[2].word_length = strlen(info->newGame->tts[2].text) * (info->fontSize - 20);
    info->newGame->tts[2].rect.x = info->newGame->tts[1].rect.x + info->newGame->tts[1].word_length + info->current.w/3;
    info->newGame->tts[2].rect.y = info->current.h/100;
    info->newGame->tts[2] = drawText(info,info->newGame->tts[2].text,info->newGame->tts[2].rect.x,info->newGame->tts[2].rect.y,DEFCOL);
    //White line
    SDL_SetRenderDrawColor(info->renderer, 255, 255, 255, 255 );
    SDL_RenderDrawLine(info->renderer,0,info->current.h/10,info->current.w,info->current.h/10);
}

//set properties for shooting mode
void setShooting(Information *info){
    if(!info->player->character->isFiring){
        info->player->character->isFiring = true;
        info->player->character->l->isActive = true;
        if (Mix_PlayChannel( 1, info->player->character->l->sound, 0 ) == -1)
            SDL_Log(Mix_GetError());
    }
}

void shooting(Information *info){
    //if not already shooting, start shooting
    if(!info->player->character->isFiring){
        info->player->character->l->rect.x = info->player->character->rect.x + info->player->character->rect.w/2;
        info->player->character->l->rect.y = info->player->character->rect.y;
    }
    //if laser is over the boundy of the game
    if (info->player->character->l->rect.y <= info->current.h/10){
        info->player->character->isFiring = false;
        info->player->character->l->isActive = false;
    }
    //render laser
    if(info->player->character->isFiring){
        info->player->character->l->rect.y -= info->player->character->l->speed;
        if(SDL_RenderCopy (info->renderer, info->player->character->l->texture, &info->player->character->l->crop, &info->player->character->l->rect) == -1)
             SDL_Log("HERE3: %s",SDL_GetError());
    }
}

//draw new text on the screen creating a new screen
//pause screen, death screen
void drawNewScreen(Information *info, int p,char* text){
    info->fontSize = info->current.h / 20;
    info->font = TTF_OpenFont("files/font.ttf",info->fontSize);
    info->newGame->tts[p].text = text;
    info->newGame->tts[p].word_length = strlen(info->newGame->tts[p].text) * (info->fontSize - 10);
    info->newGame->tts[p].rect.x = (info->current.w - info->newGame->tts[p].word_length)/2 ;
    info->newGame->tts[p].rect.y = info->current.h/2;
    info->newGame->tts[p] = drawText(info,info->newGame->tts[p].text,info->newGame->tts[p].rect.x,info->newGame->tts[p].rect.y,PRECOL);
    if(SDL_RenderCopy (info->renderer, info->newGame->tts[p].texture, NULL, &info->newGame->tts[p].rect) == -1)
        SDL_Log(SDL_GetError());
    SDL_RenderPresent(info->renderer);

}




