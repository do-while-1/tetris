#include "/usr/include/SDL2/SDL.h"
#include "shapes.h"
#include "stdlib.h"

#define screensizex 1600
#define screensizey 900

enum direction {UP, LEFT, DOWN, RIGHT};
enum blockType {I, S, Z, T, J, L, O};

struct shape{
	int x, y;
	enum direction dir;
	enum blockType type;
	int s[7][4][4][4];
};

struct game{
	struct shape block;
	//struct shape next;
	int field[10][22];
	unsigned int currentTime;
}tetris = {{0, 0, UP, I, {{{{0}}}}}, {{0}, {0}}};

struct graphics{
	SDL_Window* window;
	SDL_Renderer* renderer;
}gfx = {NULL, NULL};

void graphicsInit();
void graphicsClose();
void graphicsDraw();		
int gameEvents();
void gameRun();
int checkColl();
void addBlock();
void moveDown();
void dropBlock();
void checkLimit();
void resetBlock();
void scanLines();
void moveBlocksDown(int line);

int main(){
	graphicsInit();
	tetris.currentTime = SDL_GetTicks();
	int s, r, x, y;
	for(s = 0; s < 7; s++){
		for(r = 0; r < 4; r++){
			for(x = 0; x < 4; x++){
				for(y = 0; y < 4; y++){
					tetris.block.s[s][r][x][y] = shapetypes[s][r][x][y];
				}
			}
		}
	}

	do{
		graphicsDraw();
		gameRun();
	}while(gameEvents());

	return 0;
}

void gameRun(){
	if(tetris.currentTime + 500 < SDL_GetTicks()){
		moveDown(&tetris.block);
		tetris.currentTime = SDL_GetTicks();
	}
	checkColl();
}

void resetBlock(){
	tetris.block.type = rand() % 7;
	tetris.block.dir = UP;
	tetris.block.x = 4;
	tetris.block.y = 0;
}

void checkLimit(){
	int x, y;
	int right = 0;
	int left = 4;
	for(x = 0; x < 4; x++){
		for(y = 0; y < 4; y++){
			if(tetris.block.s[tetris.block.type][tetris.block.dir][x][y] == 1 && x < left) left = x;
			if(tetris.block.s[tetris.block.type][tetris.block.dir][x][y] == 1 && x > right) right = x;
		}
	}
	if(tetris.block.x < 0 - left) tetris.block.x = 0 - left;
	if(tetris.block.x > 9 - right) tetris.block.x = 9 - right;
}

void moveDown(struct shape * s){
	s->y++;
}

void dropBlock(){
	do{
		tetris.block.y++;
	}while(checkColl() == 0);
}

int checkColl(){
	int x, y;
	int limit = 0;
	for(x = 0; x < 4; x++){
		for(y = 0; y < 4; y++){
			if(tetris.block.s[tetris.block.type][tetris.block.dir][x][y] == 1){
				limit = tetris.block.y + y;
				if(limit > 20){
					addBlock();
					return 1;
				}
				if(tetris.field[tetris.block.x + x][limit+1] == 1){
					addBlock();
					return 1;
				}
			}
	
		}
	}
	return 0;
}

void addBlock(){
	int x, y;
	for(x = 0; x < 4; x++){
		for(y = 0; y < 4; y++){
			if(tetris.block.s[tetris.block.type][tetris.block.dir][x][y] == 1){
				tetris.field[tetris.block.x + x][tetris.block.y + y] = 1;
			}
		
		}
	}
	resetBlock();
	scanLines();
}

void moveBlocksDown(int line){
	int i, x, y;
	for(i = 0; i < 10; i++){
		tetris.field[i][line] = 0;
	}
	for(y = 21; y >= 0; y--){
		for(x = 0; x < 10; x++){
			if(y<line) tetris.field[x][y+1] = tetris.field[x][y];
		}
	}

}

void scanLines(){
	int x, y;
	int count = 0;
	for(y = 0; y < 22; y++){
		for(x = 0; x < 10; x++){
			if(tetris.field[x][y] == 1) count++;			
		}
		if(count == 10) moveBlocksDown(y);
		count = 0;
	}
}

int gameEvents(){
	SDL_Event event;
	while(SDL_PollEvent (&event)){
		switch(event.type){
			case SDL_QUIT:
				return 0;
			case SDL_KEYDOWN:
				if(event.key.keysym.sym == SDLK_ESCAPE)
					return 0;
				if(event.key.keysym.sym == SDLK_UP){
					tetris.block.dir++;
					if(tetris.block.dir > 3) tetris.block.dir = 0;
					checkLimit();
					break;}
				if(event.key.keysym.sym == SDLK_DOWN){
					moveDown(&tetris.block);
					checkColl();
					break;}
				if(event.key.keysym.sym == SDLK_LEFT){
					tetris.block.x--;
					checkLimit();
					break;}
				if(event.key.keysym.sym == SDLK_RIGHT){
					tetris.block.x++;
					checkLimit();
					break;}
				if(event.key.keysym.sym == SDLK_SPACE){
					dropBlock();
					break;}

				default:
					break;
		}
	}
	return 1;
}

void graphicsInit(){
	if(SDL_Init(SDL_INIT_VIDEO) != 0){
		printf("sdl init failed\n");
		graphicsClose();
	}
	gfx.window = SDL_CreateWindow("arctic tetris", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,           
        screensizex, screensizey, SDL_WINDOW_RESIZABLE);
	if(gfx.window == NULL){
		printf("window creation failed\n");
		graphicsClose();
	}
	gfx.renderer = SDL_CreateRenderer(gfx.window, -1, SDL_RENDERER_ACCELERATED);
}

void graphicsDraw(){
	SDL_RenderClear(gfx.renderer);
	SDL_Rect box = {625,100,350,770};
	SDL_RenderDrawRect(gfx.renderer, &box);
	int x, y;
	for(x = 0; x < 4; x++){
		for(y = 0; y < 4; y++){
			if(tetris.block.s[tetris.block.type][tetris.block.dir][x][y] == 1){
				SDL_Rect rect = {0,0,0,0};
				rect.x = 625 + x * 35 + tetris.block.x * 35;
				rect.y = 100 + y * 35 + tetris.block.y * 35;
				rect.w = 35;
				rect.h = 35;
				SDL_RenderFillRect(gfx.renderer, &rect);
			}
		}
	}
	for(x = 0; x < 10; x++){
		for(y = 0; y < 22; y++){
			if(tetris.field[x][y] == 1){
				SDL_Rect rect = {0,0,0,0};
				rect.x = 625 + x * 35;
				rect.y = 100 + y * 35;
				rect.w = 35;
				rect.h = 35;
				SDL_RenderFillRect(gfx.renderer, &rect);
			}
	}
}
	SDL_RenderPresent(gfx.renderer);
	
}
void graphicsClose(){
	SDL_DestroyWindow(gfx.window);
	SDL_DestroyRenderer(gfx.renderer);
	SDL_Quit();
	exit(1);
}
