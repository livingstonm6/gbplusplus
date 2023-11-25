#pragma once
#include "Registers.h"
#include "instructions.h"
#include "common.h"

struct InterruptRequests {
	bool vblank = false;
	bool lcd_stat = false;
	bool timer = false;
	bool serial = false;
	bool joypad = false;
};


class CPUContext
{
public:
	Registers reg;
	bool halted = false;
	Instruction current_instruction;
	bool destination_is_memory = false;
	u16 fetched_data{};
	u16 memory_destination{};
	u8 current_opcode{};
	u8 cycles_to_increment{};
	bool ime = false;
	bool enabling_ime = false;

	struct InterruptRequests interrupt_requests;

};

