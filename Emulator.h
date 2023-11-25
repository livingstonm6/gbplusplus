#pragma once
//#include "SDL.h"
#include <string>
#include "Motherboard.h"
class Emulator
{
public:
	Motherboard motherboard;
	Emulator() {}
	void run();
};

