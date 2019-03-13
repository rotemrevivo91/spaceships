#include <math.h>
#include <time.h>
#include <stdlib.h>

#include "constructors.h"

#define LEFT 1
#define RIGHT 2

//move the player according to the given direction and as long as the player is in screen bounds.
void movePlayer(Information *info, int direction){
    if(direction == LEFT && info->player->character->rect.x > 0)
        info->player->character->rect.x -= info->player->character->speed;
    else if(direction == RIGHT && info->player->character->rect.x < info->current.w - info->player->character->rect.w)
        info->player->character->rect.x += info->player->character->speed;
    if(SDL_RenderCopy(info->renderer,info->player->character->texture,NULL,&info->player->character->rect) == -1)
        SDL_Log("graphics:movePlayer:info->player->character->texture : %s",SDL_GetError());
}

//move enemies according to their speed
void enemyMovement(Information *info){
    for(int i=0;i<ESIZE;i++){
        //increase enemies speed by 0.25
        //every 4 deaths of an enemy, he gets a speed boost
        info->enemies[i].character->speed += 0.25;
        if(info->enemies[i].character->speed == 1){
            info->enemies[i].character->rect.y += 1;
            //set laser's coordinates back to enemy
            if(!info->enemies[i].character->l->isActive)
                info->enemies[i].character->l->rect.y = info->enemies[i].character->rect.y;
            //set speed back to an int value in order to render it on the screen
            info->enemies[i].character->speed = info->enemies[i].currentSpeed;
            if(SDL_RenderCopy(info->renderer,info->enemies[i].character->texture,NULL,&info->enemies[i].character->rect) == -1)
               SDL_Log("graphics:enemyMovement:info->enemies[%d]->character->texture : %s",i,SDL_GetError());
        }
        //respawn enemies when they reach the other end of the screen
        if(info->enemies[i].character->rect.y == info->current.h){
            info->player->points--;
            updateEnemy(info,i);
        }
    }
}

void enemyShooting (Information *info){
    //pick 2 random enemies to shoot at a time
    srand(info->startTime - time(NULL));
    int rand1 = rand()%8;
    int rand2 = rand()%8;

    //setting all other enemies to set not shooting
    for(int i=0;i<ESIZE;i++){
        if(i!=rand1 && !info->enemies[i].character->l->isActive)
            info->enemies[i].character->isFiring = false;
    }

    //setting the 2 picked enemies for shooting
    info->enemies[rand1].character->isFiring = true;
    if(info->player->points > 50){
        info->enemies[rand2].character->isFiring = true;
        info->enemies[rand2].character->l->isActive = true;
    }
    //setting the laser of the 2 picked enemies to active
    info->enemies[rand1].character->l->isActive = true;

    for(int i=0;i<ESIZE; i++){
        if(info->enemies[i].character->isFiring && info->enemies[rand1].character->l->isActive && info->enemies[rand1].character->l->isActive){
            //laser over screen's boundry
            if(info->enemies[i].character->l->rect.y >= info->current.h){
                //set laser back to enemy location
                info->enemies[i].character->l->rect.x = info->enemies[i].character->rect.x + info->enemies[i].character->rect.w/2;
                info->enemies[i].character->l->rect.y = info->enemies[i].character->rect.y + info->enemies[i].character->rect.h/2;
                info->enemies[i].character->l->isActive = false;
            }
            //render the laser's movement
            info->enemies[i].character->l->rect.y += info->enemies[i].character->l->speed * 3;
            if(SDL_RenderCopy (info->renderer, info->enemies[i].character->l->texture, &info->enemies[i].character->l->crop, &info->enemies[i].character->l->rect) == -1)
                SDL_Log(SDL_GetError());
        }
    }
}

int updateExplosion(int Row, Information *info){
    if(info->explosion->isActive && info->explosion->Frames > 0){
        //the explosion's png is consisted of 24 separate images over 3 rows (8 in a row)
        //when the 8th picture is in frame move to the next row
        if(info->explosion->Frames%8==0)
            Row++;

        //render the explosion over a span of 3 seconds
        info->explosion->ticks = (SDL_GetTicks()/3000) % 8;
        info->explosion->ImageRect.x = info->explosion->ticks * 64;
        //each image is 64pixel width
        info->explosion->ImageRect.y = Row * 64;
        info->explosion->isActive = true;

        if(SDL_RenderCopy (info->renderer, info->explosion->texture, &info->explosion->ImageRect, &info->explosion->ScreenRect) == -1)
            SDL_Log(SDL_GetError());
        info->explosion->Frames--;
    }
    //reset explosion's properties
    else{
        info->explosion->isActive = false;
        info->explosion->Frames = 16;
        Row = -1;
    }
    return Row;
}

//The function is called when an enemy dies. All enemy properties are being reseted
void updateEnemy(Information *info, int i){
    info->enemies[i].deathCounter++;
    info->enemies[i].character->rect.x = 10 + i*(info->current.w/8);
    info->enemies[i].character->rect.y = info->current.h/100 + info->player->character->rect.h;
    info->enemies[i].character->isHit = false;
    info->enemies[i].character->isRespawned = true;
    //increase enemy speed
    if(info->enemies[i].character->speed < 0.75)
        info->enemies[i].currentSpeed=info->enemies[i].character->speed+=0.25;
    //laser's speed
    switch (info->enemies[i].deathCounter){
        case 5:
            info->enemies[i].character->l->speed++;
            info->enemies[i].deathCounter++;
        break;
        case 11:
            info->enemies[i].character->l->speed++;
            info->enemies[i].deathCounter++;
        break;
        case 17:
            info->enemies[i].character->l->speed++;
            info->enemies[i].deathCounter++;
        break;
        case 53:
            info->enemies[i].character->l->speed++;
            info->enemies[i].deathCounter++;
        break;
        case 104:
            info->enemies[i].character->l->speed++;
            info->enemies[i].deathCounter++;
        break;
    }
}

bool detectCollision(SDL_Rect rect1, SDL_Rect rect2){

	// Find edges of rect1
	int left1 = rect1.x;
	int right1 = rect1.x + rect1.w;
	int top1 = rect1.y;
	int bottom1 = rect1.y + rect1.h;

	// Find edges of rect2
	int left2 = rect2.x;
	int right2 = rect2.x + rect2.w;
	int top2 = rect2.y;
	int bottom2 = rect2.y + rect2.h;

	// Check edges
	if ( left1 > right2 )// Left 1 is right of right 2
		return false; // No collision

	if ( right1 < left2 ) // Right 1 is left of left 2
		return false; // No collision

	if ( top1 > bottom2 ) // Top 1 is below bottom 2
		return false; // No collision

	if ( bottom1 < top2 ) // Bottom 1 is above top 2
		return false; // No collision

	return true;
}

//Transforms a string into an texture object to draw on the screen
TextToScreen drawText(Information *info, char* text, int x, int y, SDL_Color color){
    TextToScreen tts;
    tts.text = text;
    tts.color = color;
    tts.rect.x = x;
    tts.rect.y = y;
    tts.headline = TTF_RenderText_Solid(info->font,text,color);
    tts.texture = SDL_CreateTextureFromSurface(info->renderer,tts.headline);
    SDL_FreeSurface(tts.headline);
    SDL_QueryTexture(tts.texture,NULL,NULL,&tts.rect.w,&tts.rect.h);
    return tts;
}
