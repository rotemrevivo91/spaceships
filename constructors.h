#include <stdbool.h>
#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>
#include <stdio.h>

#pragma once

#define DEFCOL (SDL_Colour) {255,255,255}
#define PRECOL (SDL_Colour) {255,0,0}

#define MAIN_MENU 1
#define START_GAME 2
#define OPTIONS 3
#define HIGHSCORES 4

#define MSIZE 5
#define ESIZE 8
#define OSIZE 7

// Game + Animation

struct _TextToScreen{
    char *text;
    SDL_Surface *headline;
    SDL_Texture *texture;
    SDL_Color color;
    SDL_Rect rect;
    int word_length;
};typedef struct _TextToScreen TextToScreen;

//Menu
struct _MenuOptions{
    TextToScreen tts;
    bool isPressed;
};typedef struct _MenuOptions MenuOptions;

//Weapons

struct LaserStruct{
	SDL_Rect rect;
	SDL_Rect crop;
	SDL_Texture *texture;
	Mix_Chunk *sound;
	SDL_TimerID timer;
	int speed;
	bool isActive;
};typedef struct LaserStruct Laser;

struct _Explosion{
    SDL_Rect ImageRect;
    SDL_Rect ScreenRect;
    SDL_Texture *texture;
    uint32_t ticks;
    int Frames;
    bool isActive;
};typedef struct _Explosion Explosion;

//Character
struct CharacterStruct{
	SDL_Rect rect;
	SDL_Texture *texture;
	SDL_Surface *surface;
	Laser *l;
	bool isFiring;
	bool isHit;
	bool isRespawned;
	double speed;
};typedef struct CharacterStruct Character;

struct PlayerStruct{
    Character *character;
    int lives;
    int points;
    bool isLeft;
    bool isRight;
};typedef struct PlayerStruct Player;

struct EnemyStruct{
    Character *character;
    double currentSpeed;
    int deathCounter;
    bool isPoint;
};typedef struct EnemyStruct Enemy;


//new game
struct NewGameStruct{
    SDL_Texture *bitmapTex; //loading the background
    SDL_Surface *bitmapSurface; //image surface -> texture
    SDL_Event event;  //events on the screen (key pressed..)
    bool isPaused;
    bool isDeathscreen;
    TextToScreen *tts;
    uint32_t gameStartTime;
    char timeAsString[20];
    int mins;
}; typedef struct NewGameStruct NewGame;

struct HighscoresStruct{
    TextToScreen tts[14]; //for reading the highscores
     //for setting a new highscore
    FILE* data;
    char names[40];    //all the names of the file
    int hscores[10];   //all the scores of the file
    int position;      //position of the score
    bool newHSFound;
};typedef struct HighscoresStruct Highscores;

struct InformationStruct{
    //window
    SDL_Window *window;
    SDL_DisplayMode current;
    //graphics
    SDL_Renderer *renderer;
    SDL_Texture *bitmapTex;
    SDL_Surface *bitmapSurface;
    TTF_Font *font;
    int fontSize;
    //music
    Mix_Chunk *music;
    //events
    SDL_Event event;
    //main menu
    MenuOptions *mmenu;
    //options menu
    MenuOptions *options;
    //game
    NewGame *newGame;
    Player *player;
    Enemy *enemies; //array of enemies
    Explosion *explosion;
    //highscores
    Highscores *highscores;
    //game state
    int state;
    //time
    uint32_t startTime;
}; typedef struct InformationStruct Information;

