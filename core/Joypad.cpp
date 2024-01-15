#include "Joypad.h"

void Joypad::write(u8 value)
{
	select_buttons = (value >> 5) & 1;
	select_dpad = (value >> 4) & 1;
}

u8 Joypad::read()
{
	if (!select_buttons) {
		return (select_dpad << 4) | (start << 3) | (select << 2) | (b << 1) | a;
	}
	else if (!select_dpad) {
		return (1 << 5) | (down << 3) | (up << 2) | (left << 1) | right;
	}
	else {
		return 0b111111;
	}
}


