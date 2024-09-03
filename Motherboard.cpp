#include "Motherboard.h"
#include <fstream>
#include <iostream>
#include "core/utility/FileOpenDialog.h"
#include <chrono>

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
			r.y = (line_num * scale) + menu_bar_height;
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

	render_gui();

	SDL_RenderPresent(renderer);

}



void Motherboard::init()
{

	if (!emulator_running) {
		init_rendering();
		init_audio();
	}
	else {
		cartridge = Cartridge();
		bus = Bus();
		cpu = CPU();
		ppu = PPU();
		lcd = LCD();
		apu = APU(device);
		timer = Timer();
		ppu_memory = PPUMemory();
	}

	bus.connect(&ppu_memory, &lcd, &cartridge, &timer, &apu);
	cpu.connect(&bus, &timer, &ppu, &apu);
	ppu.connect(&lcd, &ppu_memory, &bus);

	emulator_running = true;
}

void Motherboard::init_rendering()
{
	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);

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

	// Main window GUI

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
	ImGui::StyleColorsDark();
	ImGui_ImplSDL2_InitForSDLRenderer(window, renderer);
	ImGui_ImplSDLRenderer2_Init(renderer);


	// Debug Window
	if (show_debug_window) {
		debug_window = SDL_CreateWindow("gb++ Tile Viewer",
			SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, debug_width,
			debug_height, SDL_WINDOW_SHOWN);
		debug_screen = SDL_CreateRGBSurface(0,
			(16 * 8 * scale) + (16 * scale),
			(32 * 8 * scale) + (64 * scale),
			32, 0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);

		debug_renderer = SDL_CreateRenderer(debug_window, -1, SDL_RENDERER_ACCELERATED);

		debug_texture = SDL_CreateTexture(debug_renderer,
			SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING,
			(16 * 8 * scale) + (16 * scale),
			(32 * 8 * scale) + (64 * scale));
	}

}

void Motherboard::init_audio()
{
	SDL_zero(spec);

	spec.format = AUDIO_F32;
	spec.channels = 1;
	spec.freq = SAMPLE_RATE;
	spec.samples = BUFFER_SIZE;
	spec.callback = NULL;

	device = SDL_OpenAudioDevice(NULL, 0, &spec, NULL, 0);

	if (device == 0) {
		std::cerr << "Error opening audio device." << std::endl;
		exit(-2);
	}
	std::cout << "device: " << device << "\n";

	SDL_PauseAudioDevice(device, 0);

	SDL_AudioStatus status = SDL_GetAudioDeviceStatus(device);
	switch (status) {
	case SDL_AUDIO_STOPPED:
		std::cout << "Audio device stopped\n";
		break;
	case SDL_AUDIO_PLAYING:
		std::cout << "Audio device playing\n";
		break;
	case SDL_AUDIO_PAUSED:
		std::cout << "Audio device paused\n";
		break;
	}

	std::cout << "Audio initialized\n";
}

void Motherboard::handle_input(bool down, SDL_Keycode key)
{
	//TODO handle up/down
	//TODO controller support
	//TODO rebind keys
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

}



void Motherboard::run()
{
	SDL_Event event;

	while (emulator_running) {

		while (SDL_PollEvent(&event)) {

			ImGui_ImplSDL2_ProcessEvent(&event);

			if (event.type == SDL_KEYDOWN) {
				handle_input(true, event.key.keysym.sym);
			}

			if (event.type == SDL_KEYUP) {
				handle_input(false, event.key.keysym.sym);
			}

			if (event.type == SDL_QUIT) {
				emulator_running = false;
				stop_cpu = true;
				start_cpu = false;
				break;
			}
		}

		if (stop_cpu) {
			cpu_running = false;
			if (cpu_thread != nullptr && cpu_thread->joinable()) {
				cpu_thread->join();
			}
			stop_cpu = false;
			cpu_thread = nullptr;
			if (emulator_running) {
				init();
			}
		}

		if (start_cpu) {
			cartridge.load_rom(filename);
			cpu_thread = std::make_unique<std::thread>(&Motherboard::run_cpu, this);
			start_cpu = false;
		}

		if (!cpu_running && emulator_running) {
			// Render GUI if emulation hasn't started or was closed
			SDL_RenderClear(renderer);
			render_gui();
			SDL_RenderPresent(renderer);
		}
	}

	ImGui_ImplSDLRenderer2_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();

}

void Motherboard::run_cpu()
{
	int current_frame = 0;
	int step_count = 0;
	int start_time = SDL_GetTicks();
	//double frame_time_target = (double)1000 / 60;
	double frame_time_target = 0; // remove frame rate cap


	cpu_running = true;
	while (cpu_running) {

		if (!paused) {
			cpu.step();
			step_count++;
		}

		int current_time = SDL_GetTicks();
		if (current_frame != ppu.current_frame ||
			(paused && (current_time - start_time > frame_time_target))) {
			
			if (!paused) {
				current_frame++;
			}

			update_window(screen, texture, renderer);
			if (show_debug_window) {
				update_debug_window(debug_screen, debug_texture, debug_renderer);
			}
			start_time = SDL_GetTicks();
			step_count = 0;
			cartridge.save_battery();
		}
	}
}

void Motherboard::render_gui()
{
	ImGui_ImplSDLRenderer2_NewFrame();
	ImGui_ImplSDL2_NewFrame();
	ImGui::NewFrame();

	if (ImGui::BeginMainMenuBar()) {
		if (!window_y_adjusted) {
			menu_bar_height = (int)ImGui::GetWindowSize().y;
			//SDL_SetWindowSize(window, SCREEN_WIDTH, SCREEN_WIDTH + menu_bar_height);

			window_y_adjusted = true;
		}
		if (ImGui::BeginMenu("File")) {
			if (ImGui::MenuItem("Open...")) {
				filename = FileOpenDialog::open();
				if (filename != "") {
					stop_cpu = true;
					start_cpu = true;
				}
			}
			if (ImGui::MenuItem("Exit")) {
				stop_cpu = true;
				emulator_running = false;
			}

			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Emulation")) {
			if (paused) {
				if (ImGui::MenuItem("Unpause")) {
					paused = false;
				}
			}
			else {
				if (ImGui::MenuItem("Pause")) {
					paused = true;
				}
			}
			if (cpu_running) {
				if (ImGui::MenuItem("Stop")) {
					stop_cpu = true;
				}
				if (ImGui::MenuItem("Reset")) {
					stop_cpu = true;
					start_cpu = true;
				}
			}

			if (ImGui::BeginMenu("Frame Rate Cap")) {
				if (ImGui::MenuItem("60 (1x)")) {
					ppu.set_framerate_cap(60);
				}
				if (ImGui::MenuItem("120 (2x)")) {
					ppu.set_framerate_cap(120);
				}
				if (ImGui::MenuItem("Unlocked")) {
					ppu.set_framerate_cap(0);
				}
			}

			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
	}

	ImGui::Render();
	SDL_RenderSetScale(renderer, io.DisplayFramebufferScale.x, io.DisplayFramebufferScale.y);
	SDL_SetRenderDrawColor(renderer, (Uint8)(clear_color.x * 255), (Uint8)(clear_color.y * 255), (Uint8)(clear_color.z * 255), (Uint8)(clear_color.w * 255));
	ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData());
}
