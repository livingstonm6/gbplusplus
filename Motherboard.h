#pragma once
#include "Cartridge.h"
#include "Bus.h"
#include "CPU.h"
#include "PPU.h"
#include "LCD.h"
#include <string>
#include "Timer.h"
#include "SDL.h"
#include "APU.h"
#include <mutex>
#include "imgui/imgui.h"
#include "imgui/imgui_impl_sdl2.h"
#include "imgui/imgui_impl_sdlrenderer2.h"
#include <Windows.h>


class Motherboard
{
	const int SCREEN_WIDTH = 640;
	const int SCREEN_HEIGHT = 576;
	const int scale = 4;
	const int debug_width = 16 * 8 * scale;
	const int debug_height = 32 * 8 * scale;
	u32 tile_colours[4] = { 0xFFFFFFFF, 0xFFAAAAAA, 0xFF555555, 0xFF000000 };
	void handle_input(bool, SDL_Keycode);
	std::mutex thread_lock;
	void run_cpu(ImGuiIO&);

	SDL_Window* window = nullptr;
	SDL_Renderer* renderer = nullptr;
	SDL_Surface* screen = nullptr;
	SDL_Texture* texture = nullptr;

	SDL_Window* debug_window = nullptr;
	SDL_Renderer* debug_renderer = nullptr;
	SDL_Surface* debug_screen = nullptr;
	SDL_Texture* debug_texture = nullptr;

	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

	HRESULT BasicFileOpen();

public:
	Cartridge cartridge;
	Bus bus;
	CPU cpu;
	PPU ppu;
	LCD lcd;
	APU apu;
	Timer timer;
	PPUMemory ppu_memory;
	std::string filename = "pokemon.gb";

	bool show_debug_window = false;

	bool running = true;
	bool paused = false;

	Motherboard() { init(); }

	void display_tile(SDL_Surface*, u16, int, int, int);
	void update_debug_window(SDL_Surface*, SDL_Texture*, SDL_Renderer*);
	void update_window(SDL_Surface*, SDL_Texture*, SDL_Renderer*, ImGuiIO&);
	void init();
	void run();
};

