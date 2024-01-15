#pragma once
#include "instructions.h"
#include "utility/Common.h"

class Registers
{
	u8 A = 0x01;
	u8 F = 0xB0;
	u8 B = 0x00;
	u8 C = 0x13;
	u8 D = 0x00;
	u8 E = 0xD8;
	u8 H = 0x01;
	u8 L = 0x4D;
	u16 SP = 0xFFFE;
	u16 PC = 0x100;

public:
	u16 read(RegisterType rt);

	void write(RegisterType rt, u16 value);
	void add(RegisterType rt, u16 value);
	
	bool get_flag(CPUFlagType ft);
	void set_flag(CPUFlagType ft, bool value);
	void set_flags(int z, int n, int h, int c);


};

