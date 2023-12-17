#pragma once
#include "Common.h"
class Joypad
{
public:
	// 0 = pressed, 1 = not pressed
	bool start = 1;
	bool select = 1;
	bool b = 1;
	bool a = 1;
	bool down = 1;
	bool up = 1;
	bool left = 1;
	bool right = 1;
	
	// 0 = selected, 1 = not selected, both 1 = return all not pressed
	bool select_buttons = 1;
	bool select_dpad = 1;

	void write(u8);
	u8 read();
};

