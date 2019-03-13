#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_image.h>

#include "graphics.h"
#include "constructors.h"
#include "mainmenu.h"
#include "newgame.h"

void createWindow();
void setupInfo(Information *info);
void gameOptions();
void setupMainMenu(Information *info);
void setupLevel1(Information *info);
void setupHighscores(Information *info);
void setupOptions(Information *info);
void clearAll(Information *info);
void clearNewGame(Information *info);
void clearMainMenu(Information *info);
void clearOptions(Information *info);

