#pragma once
#include "Registers.h"
#include "instructions.h"
#include "utility/Common.h"
#include <vector>

class CPUContext
{
public:
	Registers reg;
	bool halted = false;
	Instruction current_instruction;
	bool writing_to_memory = false;
	u16 fetched_data{};
	u16 memory_destination{};
	u8 current_opcode{};
	bool interrupt_master_enable = false;
	bool enabling_ime = false;

	std::vector<InterruptType> ppu_interrupts;
	u8 cycles_to_increment{};

};

