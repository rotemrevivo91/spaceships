#include <SDL.h>
#include <SDL_ttf.h>
#include <stdio.h>
#include <string.h>

#include "setup.h"
#include "graphics.h"


int createMainMenu(Information *info){
    int r = 1;
    while(r == 1){
        if(SDL_PollEvent(&info->event)){
            switch(info->event.type){
                case SDL_QUIT:
                    return 0;
                    break;
                case SDL_KEYDOWN:
                    switch(info->event.key.keysym.sym){
                        case SDLK_DOWN:
                        for(int i=1;i<MSIZE;i++){
                            if(info->mmenu[i].isPressed){
                                info->mmenu[i].isPressed=false;
                                if(i==4){
                                    info->mmenu[1].isPressed=true;
                                    info->mmenu[4].tts.color = DEFCOL;
                                    info->mmenu[1].tts.color = PRECOL;
                                }
                                else{
                                    info->mmenu[i+1].isPressed=true;
                                    info->mmenu[i].tts.color = DEFCOL;
                                    info->mmenu[i+1].tts.color = PRECOL;
                                }
                                break;
                            }
                        }
                        for(int i=1;i<MSIZE;i++)
                            info->mmenu[i].tts = drawText(info,info->mmenu[i].tts.text,info->mmenu[i].tts.rect.x,info->mmenu[i].tts.rect.y,info->mmenu[i].tts.color);
                        break;

                        case SDLK_UP:
                            for(int i=1;i<MSIZE;i++){
                                if(info->mmenu[i].isPressed){
                                    info->mmenu[i].isPressed=false;
                                if(i==1){
                                    info->mmenu[4].isPressed=true;
                                    info->mmenu[1].tts.color = DEFCOL;
                                    info->mmenu[4].tts.color = PRECOL;
                                }
                                else{
                                    info->mmenu[i-1].isPressed=true;
                                    info->mmenu[i].tts.color = DEFCOL;
                                    info->mmenu[i-1].tts.color = PRECOL;
                                }
                                break;
                                }
                            }
                            for(int i=1;i<MSIZE;i++)
                                info->mmenu[i].tts = drawText(info,info->mmenu[i].tts.text,info->mmenu[i].tts.rect.x,info->mmenu[i].tts.rect.y,info->mmenu[i].tts.color);
                            break;

                        case SDLK_SPACE:
                            for(int i=1;i<MSIZE;i++){
                                if(info->mmenu[i].isPressed){
                                    switch (i){
                                        case 1:
                                            info->state = START_GAME;
                                            return 1;
                                            break;
                                        case 2:
                                            info->state = OPTIONS;
                                            return 1;
                                            break;
                                        case 3:
                                            info->state = HIGHSCORES;
                                            return 1;
                                            break;
                                        case 4:
                                            return 0;
                                            break;
                                    }
                                }
                            }
                            break;
                    }
            }
        }
        SDL_RenderClear(info->renderer);
        SDL_RenderCopy(info->renderer, info->bitmapTex, NULL, NULL);
        for(int i=0;i<MSIZE;i++)
            SDL_RenderCopy(info->renderer,info->mmenu[i].tts.texture,NULL,&info->mmenu[i].tts.rect);
        SDL_RenderPresent(info->renderer);

    }
    return 0;
}

