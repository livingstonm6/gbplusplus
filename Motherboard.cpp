#include "Motherboard.h"
#include <fstream>
#include <iostream>
#include <thread>

void Motherboard::display_tile(SDL_Surface* screen, u16 address, int tile_num, int x, int y)
{
	for (int tile_y = 0; tile_y < 16; tile_y += 2) {
		u8 b1 = bus.read(address + (tile_num * 16) + tile_y);
		u8 b2 = bus.read(address + (tile_num * 16) + tile_y + 1);
		for (int bit = 7; bit > 0; bit--) {
			int high = !!(b1 & (1 << bit)) << 1;
			int low = !!(b2 & (1 << bit));
			int colour = high | low;
			SDL_Rect r = { 0, 0, 0, 0 };
			r.x = x + ((7 - bit) * scale);
			r.y = y + (tile_y / 2 * scale);
			r.w = scale;
			r.h = scale;
			SDL_FillRect(screen, &r, tile_colours[colour]);
		}

	}
}

void Motherboard::update_debug_window(SDL_Surface* screen, SDL_Texture* texture, SDL_Renderer* renderer)
{
	int x_draw = 0;
	int y_draw = 0;
	int tile_num = 0;
	u16 address = 0x8000;

	SDL_Rect r = { 0, 0, debug_width, debug_height };
	SDL_FillRect(screen, &r, 0xFF111111);

	for (int y = 0; y < 24; y++) {
		for (int x = 0; x < 16; x++) {
			display_tile(screen, address, tile_num,
				x_draw + (x * scale),
				y_draw + (y * scale));
			x_draw += (8 * scale);
			tile_num++;
		}
		y_draw += (8 * scale);
		x_draw = 0;
	}

	SDL_UpdateTexture(texture, nullptr, screen->pixels, screen->pitch);
	SDL_RenderClear(renderer);
	SDL_RenderCopy(renderer, texture, nullptr, nullptr);
	SDL_RenderPresent(renderer);

}

void Motherboard::update_window(SDL_Surface* screen, SDL_Texture* texture, SDL_Renderer* renderer)
{

	SDL_Rect r = { 0, 0, 2048, 2048 };
	for (int line_num = 0; line_num < ppu.Y_RES; line_num++) {
		for (int x = 0; x < ppu.X_RES; x++) {
			r.x = x * scale;
			r.y = line_num * scale;
			r.w = scale;
			r.h = scale;
			u16 address = x + (line_num * ppu.X_RES);
			u32 buf = ppu.read_video_buffer(address);
			SDL_FillRect(screen, &r, buf);
		}
	}

	SDL_UpdateTexture(texture, nullptr, screen->pixels, screen->pitch);
	SDL_RenderClear(renderer);
	SDL_RenderCopy(renderer, texture, nullptr, nullptr);
	SDL_RenderPresent(renderer);

}

void Motherboard::init()
{
	cartridge.load_rom(filename);
	bus.connect(&ppu_memory, &lcd, &cartridge, &timer, &apu);
	cpu.connect(&bus, &timer, &ppu, &lcd);
	ppu.connect(&lcd, &ppu_memory, &bus);
}

void Motherboard::handle_input(bool down, SDL_Keycode key)
{
	//TODO handle up/down
	//TODO controller support
	//TODO rebind keys
	thread_lock.lock();
	switch (key) {
	case SDLK_z:
		bus.update_joypad(JIT_B, down);
		break;
	case SDLK_x:
		bus.update_joypad(JIT_A, down);
		break;
	case SDLK_UP:
		bus.update_joypad(JIT_UP, down);
		break;
	case SDLK_LEFT:
		bus.update_joypad(JIT_LEFT, down);
		break;
	case SDLK_DOWN:
		bus.update_joypad(JIT_DOWN, down);
		break;
	case SDLK_RIGHT:
		bus.update_joypad(JIT_RIGHT, down);
		break;
	case SDLK_c:
		bus.update_joypad(JIT_START, down);
		break;
	case SDLK_v:
		bus.update_joypad(JIT_SELECT, down);
		break;

	}
	thread_lock.unlock();

}



void Motherboard::run()
{
	std::cout << "MOBO RUNNING" << "\n";

	

	//for (const auto& byte : cartridge.data) {
	//	std::cout << byte << "\n";
	//}


	//for (unsigned char byte : cartridge.data) {
	//	if (byte != 0) {
	//		std::cout << static_cast<int>(byte) << "\n";
	//	}
	//}

	//std::cout << "Finished!" << "\n";
	//std::cout << "Size: "  << cartridge.data.size() << "\n";
	//std::cout << "Byte 0x100:" << cartridge.data[0x100] << "\n";
	//std::cout << (0 == cartridge.data[0x100]) << "\n";

	SDL_Init(SDL_INIT_VIDEO);

	// Main Window

	window = SDL_CreateWindow("gb++", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

	SDL_SetRenderDrawColor(renderer, 0, 255, 255, 255);

	SDL_RenderClear(renderer);

	screen = SDL_CreateRGBSurface(0, SCREEN_WIDTH, SCREEN_HEIGHT, 32, 0x00FF0000,
		0x0000FF00, 0x000000FF, 0xFF000000);

	texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888,
		SDL_TEXTUREACCESS_STREAMING, SCREEN_WIDTH, SCREEN_HEIGHT);

	SDL_RenderPresent(renderer);


	// Debug Window

	/*SDL_Window* debug_window = SDL_CreateWindow("gb++ Tile Viewer",
		SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, debug_width,
		debug_height, SDL_WINDOW_SHOWN);
	SDL_Surface* debug_screen = SDL_CreateRGBSurface(0,
		(16 * 8 * scale) + (16 * scale),
		(32 * 8 * scale) + (64 * scale),
		32, 0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);

	SDL_Renderer* debug_renderer = SDL_CreateRenderer(debug_window, -1, SDL_RENDERER_ACCELERATED);

	SDL_Texture* debug_texture = SDL_CreateTexture(debug_renderer,
		SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING,
		(16 * 8 * scale) + (16 * scale),
		(32 * 8 * scale) + (64 * scale));*/


	running = true;

	if (cpu.debug_logging) {
		cpu.update_logfile();
	}

	std::thread cpu_thread(&Motherboard::run_cpu, this);

	SDL_Event event;

	while (running) {

		while (SDL_PollEvent(&event)) {

			if (event.type == SDL_KEYDOWN) {
				handle_input(true, event.key.keysym.sym);
			}

			if (event.type == SDL_KEYUP) {
				handle_input(false, event.key.keysym.sym);
			}

			if (event.type == SDL_QUIT) {
				thread_lock.lock();
				running = false;
				thread_lock.unlock();
				break;
			}
		}

	}

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();

}

void Motherboard::run_cpu()
{
	int current_frame = 0;
	int step_count = 0;

	while (running) {

		cpu.step();

		if (current_frame != ppu.current_frame) {
			current_frame++;
			update_window(screen, texture, renderer);
			//update_debug_window(debug_screen, debug_texture, debug_renderer);
			step_count = 0;
			cartridge.save_battery();
		}

		step_count++;

	}

}
