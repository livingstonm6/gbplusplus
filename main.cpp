//#include "SDL.h"

//int main(int argc, char* argv[]) {
//	SDL_Init(SDL_INIT_VIDEO);
//	SDL_Window* window = SDL_CreateWindow("title", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 600, 400, SDL_WINDOW_SHOWN);
//	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 0);
//
//	SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
//
//	SDL_RenderClear(renderer);
//
//	SDL_RenderPresent(renderer);
//
//	SDL_Delay(5000);
//
//	return 0;
//}

#include "Emulator.h"
int main(int arc, char* argv[]) {

	Emulator emulator;

	emulator.run();

	return 0;
}


