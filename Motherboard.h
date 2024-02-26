#pragma once
#include "core/Cartridge.h"
#include "core/Bus.h"
#include "core/CPU.h"
#include "core/PPU.h"
#include "core/LCD.h"
#include "core/Timer.h"
#include "core/APU.h"
#include "SDL.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_sdl2.h"
#include "imgui/imgui_impl_sdlrenderer2.h"
#include <string>
#include <thread>
#include <memory>


class Motherboard
{
	int menu_bar_height = 19;
	const int SCREEN_WIDTH = 640;
	const int SCREEN_HEIGHT = 576 + menu_bar_height;
	const int scale = 4;
	const int debug_width = 16 * 8 * scale;
	const int debug_height = 32 * 8 * scale;
	bool window_y_adjusted = false;

	u32 tile_colours[4] = { 0xFFFFFFFF, 0xFFAAAAAA, 0xFF555555, 0xFF000000 };
	void handle_input(bool, SDL_Keycode);
	void run_cpu();

	bool stop_cpu = false;
	bool start_cpu = false;

	SDL_Window* window = nullptr;
	SDL_Renderer* renderer = nullptr;
	SDL_Surface* screen = nullptr;
	SDL_Texture* texture = nullptr;

	SDL_Window* debug_window = nullptr;
	SDL_Renderer* debug_renderer = nullptr;
	SDL_Surface* debug_screen = nullptr;
	SDL_Texture* debug_texture = nullptr;

	ImGuiIO io;
	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

	void init_rendering();
	void render_gui();
	
	std::unique_ptr<std::thread> cpu_thread = nullptr;

public:
	Cartridge cartridge{};
	Bus bus{};
	CPU cpu{};
	PPU ppu{};
	LCD lcd{};
	APU apu{};
	Timer timer{};
	PPUMemory ppu_memory{};
	std::string filename{};

	bool show_debug_window = false;

	bool emulator_running = false;
	bool cpu_running = false;
	bool paused = false;

	Motherboard() { init(); }

	void display_tile(SDL_Surface*, u16, int, int, int);
	void update_debug_window(SDL_Surface*, SDL_Texture*, SDL_Renderer*);
	void update_window(SDL_Surface*, SDL_Texture*, SDL_Renderer*);
	void init();
	void run();
};

