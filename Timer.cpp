#include "Timer.h"
#include <iostream>

u8 Timer::read(u16 address)
{
	switch (address) {
	case 0xFF04:
		return (div >> 8);
	case 0xFF05:
		return tima;
	case 0xFF06:
		return tma;
	case 0xFF07:
		return tac;
	default:
		std::cout << "Error timer read" << "\n";
		exit(-1);
	}
}

void Timer::write(u16 address, u8 value)
{
	switch (address) {
	case 0xFF04:
		div = 0;
		break;
	case 0xFF05:
		tima = value;
		break;
	case 0xFF06:
		tma = value;
		break;
	case 0xFF07:
		tac = value;
		break;
	default:
		std::cout << "Error timer write" << "\n";
		exit(-1);
	}
}

bool Timer::tick()
{
	u16 previous_div = div;
	div++;
	bool timer_update = false;
	bool request_interrupt = false;

	switch (tac & 0b11) {
	case 0b00:
		timer_update = (previous_div & (1 << 9) && ((div & 1 << 9) == 0));
		break;
	case 0b01:
		timer_update = (previous_div & (1 << 3) && ((div & 1 << 3) == 0));
		break;
	case 0b10:
		timer_update = (previous_div & (1 << 5) && ((div & 1 << 5) == 0));
		break;
	case 0b11:
		timer_update = (previous_div & (1 << 7) && ((div & 1 << 7) == 0));
		break;
	}

	if (timer_update && (tac & (1 << 2))) {
		tima++;
		request_interrupt = true;

	}

	return request_interrupt;

}
