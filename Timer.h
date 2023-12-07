#pragma once
#include "common.h"
class Timer
{
	int ticks{};
	u16 div = 0xABCC;
	u8 tima{};
	u8 tma{};
	u8 tac{};
	u8 tima_overflow_count{};
public:
	u8 read(u16 address);
	void write(u16 address, u8 value);

	bool tick();

};

