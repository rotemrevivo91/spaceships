CC=gcc
# unpacked SDL2 into a folder named SDL2 and not to C:/SDL2
# SDL2.dll moved from SDL2\x86_64-w64-mingw32\bin to project root the same for SDL2_ttf.dll and libfreetype-6.dll from the SDL2_ttf/lib/x86 folder
# also the SDL2_ttf files from the include folder are copied to SDL2\x86_64-w64-mingw32\include\SDL2
# and the files from SDL2_ttf\lib\x86 to SDL2\x86_64-w64-mingw32\lib
CFLAGS=-I".\SDL2\x86_64-w64-mingw32\include" -std=c99
LDFLAGS=-L".\SDL2\x86_64-w64-mingw32\lib" -lmingw32 -lSDL2main -lSDL2_ttf -lSDL2 -lopengl32

all: run

PacSnake.exe:
	$(CC) $(CFLAGS) -o PacSnake.exe PacSnake.c Ghosts.c Player.c PowerUps.c Map.c Position.c Highscore.c Rendering.c $(LDFLAGS)

build:
	$(CC) $(CFLAGS) -o PacSnake.exe PacSnake.c Ghosts.c Player.c PowerUps.c Map.c Position.c Highscore.c Rendering.c $(LDFLAGS)

run: PacSnake.exe
	PacSnake.exe
