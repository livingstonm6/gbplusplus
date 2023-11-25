#pragma once
#include "Cartridge.h"
#include "Bus.h"
#include "CPU.h"
#include "PPU.h"
#include "LCD.h"
#include <string>
#include "Timer.h"

class Motherboard
{
public:
	Cartridge cartridge;
	Bus bus;
	CPU cpu;
	PPU ppu;
	LCD lcd;
	Timer timer;
	std::string filename = "01-special.gb";

	bool running = true;

	Motherboard() { init(); }


	void init();
	void run();
};

