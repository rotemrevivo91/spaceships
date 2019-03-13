#include <math.h>
#include "constructors.h"

#define LEFT 1
#define RIGHT 2
#define UP 3
#define DOWN 4

void movePlayer(Information *info, int direction);
TextToScreen drawText(Information *info, char* text, int x, int y, SDL_Color color);
int updateExplosion(int Row, Information *info);
bool detectCollision(SDL_Rect rect1, SDL_Rect rect2);
void updateEnemy(Information *info, int i);
void enemyMovement(Information *info);
