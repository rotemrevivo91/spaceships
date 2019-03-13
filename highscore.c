#include <unistd.h>
#include "constructors.h"
#include "graphics.h"


/*
	void generatehs(void)
	generates a highscore file with name "highscore.hs"
	fills file with AAAA0AAAA0AAAA0...
	fills it 10 times with sequence AAAA0
	does not return anything
	ends with closing the file
*/
void generatehs(Information *info) {
	info->highscores->data = fopen("files/highscore.txt", "w+");

	for (int i = 0; i < 10; i++) {
		for (int j = 0; j < 4; j++)
			fputc(65,info->highscores->data);

		fputc(0,info->highscores->data);
	}
	fclose(info->highscores->data);
}

//reads the entire highscores file and transfer the data to two arrays:
//hscores[10], names[40]
void readFile(Information *info){
    //if file doesn't exist, call generatehs() function
    if (access("files/highscore.txt", F_OK) == -1)
		generatehs(info);

    //opening file for reading and writing without overwritting the file
    info->highscores->data = fopen("files/highscore.txt", "r+");
    //set file's cursor to the begining of the file
    fseek(info->highscores->data,0,SEEK_SET);

    if(info->highscores->data != NULL){
        int i = 0;
        int j = 0;
        //reads 10 scores
        while(j<10){
            //each name consists of 4 letters over 10 scores
            while(i<j*4+4){
                info->highscores->names[i] = fgetc(info->highscores->data);
                i++;
            }
            info->highscores->hscores[j] = fgetc(info->highscores->data);
            j++;
        }
    }
    fclose(info->highscores->data);
}

//updating the highscores file after a game is over
void newhs(Information *info, int score, int pos) {
    //temporary background texture
    SDL_Texture *tmp = info->bitmapTex;
    //if file doesn't exist, call generatehs() function
	if (access("files/highscore.txt", F_OK) == -1)
		generatehs(info);

    //opening file for reading and writing without overwritting the file
	info->highscores->data = fopen("files/highscore.txt", "r+");
	//position of the score
	info->highscores->position = -1;
	//font and fontsize
	info->fontSize = info->current.h / 20;
	info->font = TTF_OpenFont("files/font.ttf", info->fontSize);

	//setting the location of the highscore title
	info->highscores->tts[12].text = "NEW HIGHSCORE";		//headline with positioning
    info->highscores->tts[12].word_length = strlen(info->highscores->tts[12].text) * (info->fontSize-10);
    info->highscores->tts[12].rect.x = (info->current.w - info->highscores->tts[12].word_length)/2;
    info->highscores->tts[12].rect.y = info->current.h/5;
    info->highscores->tts[12] = drawText(info,info->highscores->tts[12].text,info->highscores->tts[12].rect.x,info->highscores->tts[13].rect.y,PRECOL);
    info->highscores->tts[13].rect.x = (info->current.w - info->highscores->tts[12].word_length)/2;
    info->highscores->tts[13].rect.y = info->current.h/2;

    //set the location to put the new highscore
    info->highscores->position = pos;


	//array has to be adjusted and overwrite last element.
	for (int i = 8; i >= info->highscores->position; i--) {
        //moving all elements one place down
		info->highscores->hscores[i+1] = info->highscores->hscores[i];
		for (int j = 0; j < 4; j++){
		    //names gets 4 char string of last 4 char string in names[], puts each element 4 elements lower
		    //names[36 to 39] get overwritten by names[32 to 35]
			info->highscores->names[i*4+4+j] = info->highscores->names[i*4+j];
        }
	}

	//number of letters being types counter
	int x = -1;
	//while loop breaker
	int c = 1;
    //start catching text inputs events
    SDL_StartTextInput();

	while (c == 1) {
        if(SDL_PollEvent(&info->event)){
            switch (info->event.type) {
                case SDL_TEXTINPUT:
                    if(x < 3){
                        x++;
                        //moving the text from the event to the texture's text
                        strcpy(info->highscores->tts[13].text + x , &info->event.text.text);
                    }
                    break;
                case SDL_KEYDOWN:
                    switch (info->event.key.keysym.sym){
                        case SDLK_ESCAPE:
                            info->state = MAIN_MENU;
                            return 1;
                            break;
                        //if name is 4 characters longs and space being entered finish the loop
                        case SDLK_SPACE:
                            if (x == 3)
                                c = 0;
                    }
                    break;
            }

        }
        SDL_RenderClear(info->renderer);

        if(SDL_RenderCopy(info->renderer,tmp,NULL,NULL) == -1)
            SDL_Log(SDL_GetError());

        info->highscores->tts[13] = drawText(info,info->highscores->tts[13].text, info->highscores->tts[13].rect.x, info->highscores->tts[13].rect.y,DEFCOL);
        if(SDL_RenderCopy(info->renderer,info->highscores->tts[12].texture,NULL,&info->highscores->tts[12].rect) == -1)
            SDL_Log(SDL_GetError());
        //rendering the text only if text is not null
        if (x>-1)
            if(SDL_RenderCopy(info->renderer,info->highscores->tts[13].texture,NULL,&info->highscores->tts[13].rect) == -1)
                SDL_Log(SDL_GetError());
        //render all to screen
        SDL_RenderPresent(info->renderer);
	}
    /////////////////////////////////////////////////////////////////////////////////////////////
	//saving all new data to the file


	//copy new highscore's name to new array location
    for(int i=0;i<4;i++){
        info->highscores->names[info->highscores->position*4 +i] = info->highscores->tts[13].text[i];
    }
    //copy new highscore to new array location
    info->highscores->hscores[info->highscores->position] = score;

	for (int i = 0; i < 10; i++) {						//writing a number into the file, 10 numbers in total
		for (int j = i*4; j < i*4+4; j++){				//writing the 4 letter string input name into the file, 10 names in total
			fputc(info->highscores->names[j], info->highscores->data);
		}
		fputc(info->highscores->hscores[i], info->highscores->data);
	}

	fclose(info->highscores->data);
}


int showhs(Information *info) {
    int r = 1;
	if (access("files/highscore.txt", F_OK) == -1){							//if highscorefile does not exist; access(): <unistd.h> Linux, <io.h> MS
		generatehs(info);											//generate highscorefile
	}

	SDL_Texture *tmp = info->bitmapTex;

    char* names;												//string for the names in the highscorelists
    names = malloc(4*sizeof(char));
    int scores;												//int for the scores in the highscorelists

    info->fontSize = info->current.h / 10;
    info->font = TTF_OpenFont("files/font.ttf", info->fontSize);
    if(info->font == NULL)
        SDL_Log("Could not load the font");

    info->highscores->tts[0].text = "TOP SCORES";
    info->highscores->tts[0].word_length = strlen(info->highscores->tts[0].text) * (info->fontSize - 20);
    info->highscores->tts[0].rect.x = (info->current.w - info->highscores->tts[0].word_length)/2;
    info->highscores->tts[0].rect.y = info->current.h/100;
    info->highscores->tts[0] = drawText(info,info->highscores->tts[0].text,info->highscores->tts[0].rect.x,info->highscores->tts[0].rect.y,PRECOL);

    info->fontSize = info->current.h / 20;
    info->font = TTF_OpenFont("files/font.ttf", info->fontSize);

    info->highscores->tts[1].text = " POS | NAME | SCORES ";
    info->highscores->tts[1].word_length = strlen(info->highscores->tts[1].text) * (info->fontSize - 10);
    info->highscores->tts[1].rect.x = (info->current.w - info->highscores->tts[1].word_length)/2;
    info->highscores->tts[1].rect.y = (info->current.h/100)*20;
    info->highscores->tts[1] = drawText(info, info->highscores->tts[1].text, info->highscores->tts[1].rect.x, info->highscores->tts[1].rect.y, DEFCOL);

    char* t;
    t = malloc(16*sizeof(char));

    readFile(info);

    for (int i = 0; i < 10; i++) {                              //reading 10 int numbers
        for (int j = 0; j < 4; j++)
            names[j] = info->highscores->names[i*4+j];

        info->highscores->tts[i+2].text = malloc(18*sizeof(char));
        scores = info->highscores->hscores[i];
        sprintf(info->highscores->tts[i+2].text,"%d     ",i+1);
        sprintf(t,"      %d",scores);
        strcat(info->highscores->tts[i+2].text,names);
        strcat(info->highscores->tts[i+2].text,t);


        info->highscores->tts[i+2].word_length = strlen(info->highscores->tts[i+2].text) * 10;
        if(i == 9)
            info->highscores->tts[i+2].rect.x = (info->current.w - info->highscores->tts[i+2].word_length)/3 - 9;
        else
            info->highscores->tts[i+2].rect.x = (info->current.w - info->highscores->tts[i+2].word_length)/3;	//x = display width - word length
        info->highscores->tts[i+2].rect.y = (info->current.h/15)*(i+4);		//positioning every text below the previous

        info->highscores->tts[i+2] = drawText(info, info->highscores->tts[i+2].text, info->highscores->tts[i+2].rect.x, info->highscores->tts[i+2].rect.y, DEFCOL);
    }

    SDL_RenderClear(info->renderer);
    SDL_RenderCopy(info->renderer,tmp,NULL,NULL);
    for(int i = 0;i<12;i++)
        SDL_RenderCopy(info->renderer,info->highscores->tts[i].texture ,NULL,&info->highscores->tts[i].rect);
    SDL_RenderPresent(info->renderer);

    while(r == 1){
        if(SDL_PollEvent(&info->event)){
            switch(info->event.type){
                case SDL_KEYDOWN:
                    switch(info->event.key.keysym.sym){
                        case SDLK_ESCAPE:
                            r = 0;
                            break;
                    }
                break;
            }
        }

    }

	info->state = MAIN_MENU;
	fclose(info->highscores->data);
	free(names);
    return 1;
}
