#include "Registers.h"
#include <iostream>

u16 Registers::read(RegisterType rt)
{
	switch (rt) {
		case RT_A:
			return A;
			break;
		case RT_F:
			return F;
			break;
		case RT_B:
			return B;
			break;
		case RT_C:
			return C;
			break;
		case RT_D:
			return D;
			break;
		case RT_E:
			return E;
			break;
		case RT_H:
			return H;
			break;
		case RT_L:
			return L;
			break;
		case RT_AF:
			return (A << 8) | F;
			break;
		case RT_BC:
			return (B << 8) | C;
			break;
		case RT_DE:
			return (D << 8) | E;
			break;
		case RT_HL:
			return (H << 8) | L;
			break;
		case RT_SP:
			return SP;
			break;
		case RT_PC:
			return PC;
			break;
		default:
			std::cerr << "Error reading register " << "\n";
			exit(-1);
	}
}

void Registers::write(RegisterType rt, u16 value)
{
	switch (rt) {
		case RT_A:
			A = value & 0xFF;
			break;
		case RT_F:
			F = value & 0xFF;
			break;
		case RT_B:
			B = value & 0xFF;
			break;
		case RT_C:
			C = value & 0xFF;
			break;
		case RT_D:
			D = value & 0xFF;
			break;
		case RT_E:
			E = value & 0xFF;
			break;
		case RT_H:
			H = value & 0xFF;
			break;
		case RT_L:
			L = value & 0xFF;
			break;
		case RT_AF:
			A = (value >> 8);
			F = (value & 0xFF);
			break;
		case RT_BC:
			B = (value >> 8);
			C = (value & 0xFF);
			break;
		case RT_DE:
			D = (value >> 8);
			E = (value & 0xFF);
			break;
		case RT_HL:
			H = (value >> 8);
			L = (value & 0xFF);
			break;
		case RT_SP:
			SP = value;
			break;
		case RT_PC:
			PC = value;
			break;
	}
}

void Registers::add(RegisterType rt, u16 value)
{
	u16 val = read(rt);
	val += value;
	write(rt, val);
}

bool Registers::get_flag(CPUFlagType ft)
{
	return (F >> ft) & 1;
}

void Registers::set_flag(CPUFlagType ft, bool value)
{
	if (value == 0) {
		u8 mask = ~(1 << ft);
		F &= mask;
	}
	else {
		u8 mask = 1 << ft;
		F |= mask;
	}
}

void Registers::set_flags(int z, int n, int h, int c)
{
	if (z != -1) {
		set_flag(CFT_Z, z);
	}
	if (n != -1) {
		set_flag(CFT_N, n);
	}
	if (h != -1) {
		set_flag(CFT_H, h);
	}
	if (c != -1) {
		set_flag(CFT_C, c);
	}
}




