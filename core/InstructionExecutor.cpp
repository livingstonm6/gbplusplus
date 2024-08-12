#include "InstructionExecutor.h"
#include <iostream>

void InstructionExecutor::execute_instruction(CPUContext* cpu, Bus* bus)
{
	Instruction inst = cpu->current_instruction;
	(this->*processor_functions[inst.type])(cpu, bus);

}

void InstructionExecutor::stack_push(CPUContext* cpu, Bus* bus, u8 data)
{
	cpu->reg.add(RT_SP, -1);
	bus->write(cpu->reg.read(RT_SP), data);
}

void InstructionExecutor::stack_push16(CPUContext* cpu, Bus* bus, u16 data)
{
	stack_push(cpu, bus, data >> 8);
	stack_push(cpu, bus, data & 0xFF);
}

u8 InstructionExecutor::stack_pop(CPUContext* cpu, Bus* bus)
{
	u8 data = bus->read(cpu->reg.read(RT_SP));
	cpu->reg.add(RT_SP, 1);
	return data;
}

u16 InstructionExecutor::stack_pop16(CPUContext* cpu, Bus* bus)
{
	u8 low = stack_pop(cpu, bus);
	u8 high = stack_pop(cpu, bus);
	return (high << 8) | low;
}

bool InstructionExecutor::check_condition(CPUContext* cpu)
{
	bool z = cpu->reg.get_flag(CFT_Z);
	bool c = cpu->reg.get_flag(CFT_C);
	switch (cpu->current_instruction.cond) {
	case CT_NONE:
		return true;
	case CT_C:
		return c;
	case CT_NC:
		return c == 0;
	case CT_Z:
		return z;
	case CT_NZ:
		return z == 0;
	}
}

void InstructionExecutor::go_to(CPUContext* cpu, Bus* bus, u16 address, bool push_pc)
{
	if (check_condition(cpu)) {
		if (push_pc) {
			stack_push16(cpu, bus, cpu->reg.read(RT_PC));
			cpu->cycles_to_increment += 2;
		}
		cpu->reg.write(RT_PC, address);
		cpu->cycles_to_increment++;
	}
}

void InstructionExecutor::proc_ld(CPUContext* cpu, Bus* bus)
{	
	if (cpu->writing_to_memory) {
		cpu->cycles_to_increment++;
		if (cpu->current_instruction.reg1 == RT_HL) {
			bus->write(cpu->memory_destination, (u8)cpu->fetched_data);
		}
		else if (cpu->current_instruction.reg2 >= 10) { // 16 bit
			bus->write16(cpu->memory_destination, cpu->fetched_data);
		}
		else {
			bus->write(cpu->memory_destination, (u8)cpu->fetched_data);
		}
	}
	else {
		cpu->reg.write(cpu->current_instruction.reg1, cpu->fetched_data);
	}

	if (cpu->current_instruction.mode == AM_HL_SPR) {
		bool h_flag = (cpu->reg.read(cpu->current_instruction.reg2) & 0xF) + (cpu->fetched_data & 0xF) >= 0x10;
		bool c_flag = (cpu->reg.read(cpu->current_instruction.reg2) & 0xFF) + (cpu->fetched_data & 0xFF) >= 0x100;
		cpu->reg.set_flags(0, 0, h_flag, c_flag);
		s8 val = (signed char)cpu->fetched_data;
		u16 result = cpu->reg.read(cpu->current_instruction.reg2) + val;
		cpu->reg.write(cpu->current_instruction.reg1, result);

	}

}

void InstructionExecutor::proc_jp(CPUContext* cpu, Bus* bus)
{
	go_to(cpu, bus, cpu->fetched_data, false);
}

void InstructionExecutor::proc_jr(CPUContext* cpu, Bus* bus)
{
	s8 rel = (signed char)(cpu->fetched_data & 0xFF);
	u16 address = cpu->reg.read(RT_PC) + rel;
	go_to(cpu, bus, address, false);
}

void InstructionExecutor::proc_call(CPUContext* cpu, Bus* bus)
{
	go_to(cpu, bus, cpu->fetched_data, true);
}

void InstructionExecutor::proc_nop(CPUContext* cpu, Bus* bus)
{
	cpu->cycles_to_increment += 4;
}

void InstructionExecutor::proc_adc(CPUContext* cpu, Bus* bus)
{
	u16 data = cpu->fetched_data;
	u8 a = (u8)cpu->reg.read(RT_A);
	bool cf = cpu->reg.get_flag(CFT_C);
	u16 result = data + a + cf;
	cpu->reg.write(RT_A, result);
	cpu->reg.set_flags((result & 0xFF) == 0, 0,
		(data & 0xF) + (a & 0xF) + cf > 0xF,
		data + a + cf > 0xFF);
}

void InstructionExecutor::proc_di(CPUContext* cpu, Bus* bus)
{
	cpu->interrupt_master_enable = false;
}

void InstructionExecutor::proc_xor(CPUContext* cpu, Bus* bus)
{
	u16 result = cpu->reg.read(RT_A) ^ cpu->fetched_data;
	cpu->reg.write(RT_A, result);
	cpu->reg.set_flags((result & 0xFF) == 0, 0, 0, 0);
}

void InstructionExecutor::proc_ldh(CPUContext* cpu, Bus* bus)
{
	
	if (cpu->current_instruction.reg1 == RT_A) {
		u16 data = bus->read(cpu->fetched_data);
		cpu->reg.write(RT_A, data);
	}
	else {
		bus->write(0xFF00 | cpu->memory_destination, (u8)cpu->fetched_data);
	}
	cpu->cycles_to_increment++;
}

void InstructionExecutor::proc_pop(CPUContext* cpu, Bus* bus)
{
	u8 low = stack_pop(cpu, bus);
	cpu->cycles_to_increment++;
	u8 high = stack_pop(cpu, bus);
	cpu->cycles_to_increment++;

	u16 value = (high << 8) | low;
	if (cpu->current_instruction.reg1 == RT_AF) {
		cpu->reg.write(RT_AF, value & 0xFFF0);
	}
	else {
		cpu->reg.write(cpu->current_instruction.reg1, value);
	}

}

void InstructionExecutor::proc_push(CPUContext* cpu, Bus* bus)
{
	u8 high = cpu->reg.read(cpu->current_instruction.reg1) >> 8;
	stack_push(cpu, bus, high);
	u8 low = cpu->reg.read(cpu->current_instruction.reg1) & 0xFF;
	stack_push(cpu, bus, low);

	cpu->cycles_to_increment += 3;

}

void InstructionExecutor::proc_ret(CPUContext* cpu, Bus* bus)
{
	if (cpu->current_instruction.cond != CT_NONE) {
		cpu->cycles_to_increment++;
	}

	if (check_condition(cpu)) {
		u8 low = stack_pop(cpu, bus);
		u8 high = stack_pop(cpu, bus);
		u16 result = (high << 8) | low;

		cpu->reg.write(RT_PC, result);
		cpu->cycles_to_increment += 3;

	}
}

void InstructionExecutor::proc_reti(CPUContext* cpu, Bus* bus)
{
	cpu->interrupt_master_enable = true;
	proc_ret(cpu, bus);
}

void InstructionExecutor::proc_rst(CPUContext* cpu, Bus* bus)
{
	go_to(cpu, bus, cpu->current_instruction.param, true);
}

void InstructionExecutor::proc_inc(CPUContext* cpu, Bus* bus)
{	
	bool z;
	bool h;
	u16 val = cpu->reg.read(cpu->current_instruction.reg1) + 1;
	if (cpu->current_instruction.reg1 >= 10) { // 16 bit
		cpu->cycles_to_increment++;
	}
	else {
		h = (val & 0xF) == 0;
		z = (val & 0xFF) == 0;
	}
	if (cpu->current_instruction.reg1 == RT_HL && cpu->current_instruction.mode == AM_MR) {
		u16 address = cpu->reg.read(RT_HL);
		u16 val = bus->read(address) + 1;
		bus->write(address, (u8)val);
		z = (val & 0xFF) == 0;
		h = (val & 0xF) == 0;
	}
	else {
		cpu->reg.write(cpu->current_instruction.reg1, val);
	}
	if ((cpu->current_opcode & 0x03) == 0x03) {
		return;
	}


	cpu->reg.set_flags(z, 0, h, -1);

}

void InstructionExecutor::proc_dec(CPUContext* cpu, Bus* bus)
{
	u16 val = cpu->reg.read(cpu->current_instruction.reg1) - 1;
	int z{};
	bool h;
	if (cpu->current_instruction.reg1 >= 10) { // 16 bit
		cpu->cycles_to_increment++;
	}
	else {
		z = (val & 0xFF) == 0;
		h = (val & 0xF) == 0xF;
	}
	if (cpu->current_instruction.reg1 == RT_HL && cpu->current_instruction.mode == AM_MR) {
		u16 address = cpu->reg.read(RT_HL);
		u16 val = bus->read(address) - 1;
		bus->write(address, (u8)val);
		z = (val & 0xFF) == 0;
		h = (val & 0xF) == 0xF;
	}
	else {
		cpu->reg.write(cpu->current_instruction.reg1, val);
	}
	if ((cpu->current_opcode & 0x0B) == 0x0B) {
		return;
	}

	cpu->reg.set_flags(z, 1, h, -1);

}

void InstructionExecutor::proc_add(CPUContext* cpu, Bus* bus)
{
	u16 val;
	int z;
	int h;
	int c;
	if (cpu->current_instruction.reg1 >= 10) { // 16 bit
		cpu->cycles_to_increment++;
		if (cpu->current_instruction.reg1 == RT_SP) {
			val = cpu->reg.read(RT_SP) + (signed char)cpu->fetched_data;
			z = 0;
			h = ((cpu->reg.read(RT_SP) & 0xF) + (cpu->fetched_data & 0xF)) >= 0x10;
			c = ((cpu->reg.read(RT_SP) & 0xFF) + (cpu->fetched_data & 0xFF)) >= 0x100;
		}
		else {
			val = cpu->reg.read(cpu->current_instruction.reg1) + cpu->fetched_data;
			z = -1;
			h = ((cpu->reg.read(cpu->current_instruction.reg1) & 0xFFF) + (cpu->fetched_data & 0xFFF)) >= 0x1000;
			c = (cpu->reg.read(cpu->current_instruction.reg1) + cpu->fetched_data) >= 0x10000;
		}
	}
	else {
		val = cpu->reg.read(cpu->current_instruction.reg1) + (signed char)cpu->fetched_data;
		z = (val & 0xFF) == 0;
		h = ((cpu->reg.read(cpu->current_instruction.reg1) & 0xF) + (cpu->fetched_data & 0xF)) >= 0x10;
		c = ((cpu->reg.read(cpu->current_instruction.reg1) & 0xFF) + (cpu->fetched_data & 0xFF)) >= 0x100;
	}

	cpu->reg.write(cpu->current_instruction.reg1, val);
	cpu->reg.set_flags(z, 0, h, c);

}

void InstructionExecutor::proc_sub(CPUContext* cpu, Bus* bus)
{
	u16 val = cpu->reg.read(cpu->current_instruction.reg1);
	bool cf = val - cpu->fetched_data < 0;
	bool h = (val & 0xF) - (cpu->fetched_data & 0xF) < 0;
	val -= cpu->fetched_data;
	bool z;
	if (cpu->current_instruction.reg1 >= 10) {
		z = val == 0;
	}
	else {
		z = (val & 0xFF) == 0;
	}
	cpu->reg.write(cpu->current_instruction.reg1, val);
	cpu->reg.set_flags(z, 1, h, cf);

}

void InstructionExecutor::proc_sbc(CPUContext* cpu, Bus* bus)
{
	bool cf = cpu->reg.get_flag(CFT_C);
	u16 val = cpu->fetched_data + cf;
	u16 reg1_val = cpu->reg.read(cpu->current_instruction.reg1);
	u16 result = reg1_val - val;

	bool z = (result & 0xFF) == 0;
	bool h = (reg1_val & 0xF) - (cpu->fetched_data & 0xF) - cf < 0;
	bool cf_new = reg1_val - cpu->fetched_data - cf < 0;

	cpu->reg.write(cpu->current_instruction.reg1, result);
	cpu->reg.set_flags(z, 1, h, cf_new);

}

void InstructionExecutor::proc_and(CPUContext* cpu, Bus* bus)
{
	u16 result = cpu->reg.read(RT_A) & cpu->fetched_data;
	cpu->reg.write(RT_A, result);
	cpu->reg.set_flags(result == 0, 0, 1, 0);
}

void InstructionExecutor::proc_or(CPUContext* cpu, Bus* bus)
{
	u16 result = cpu->reg.read(RT_A) | cpu->fetched_data;
	cpu->reg.write(RT_A, result);
	cpu->reg.set_flags(result == 0, 0, 0, 0);
}

void InstructionExecutor::proc_cp(CPUContext* cpu, Bus* bus)
{
	int result = cpu->reg.read(RT_A) - cpu->fetched_data;
	cpu->reg.set_flags(result == 0, 1, (cpu->reg.read(RT_A) & 0x0F) - (cpu->fetched_data & 0x0F) < 0, result < 0);
}

void InstructionExecutor::proc_cb(CPUContext* cpu, Bus* bus)
{
	u8 op = (u8)cpu->fetched_data;
	RegisterType reg_type = u8_to_rt(op & 0b111);
	u8 bit = (op >> 3) & 0b111;
	u8 bit_op = (op >> 6) & 0b11;
	u16 reg_val = cpu->reg.read(reg_type);
	int z;
	int c;
	int new_c;

	cpu->cycles_to_increment++;

	if (reg_type == RT_HL) {
		cpu->cycles_to_increment += 2;
		reg_val = bus->read(reg_val);
	}

	switch (bit_op) {
	case 1:
	{
		// BIT
		if (reg_val & (1 << bit)) {
			z = 0;
		}
		else {
			z = 1;
		}
		cpu->reg.set_flags(z, 0, 1, -1);
		return;
	}

	case 2:
	{
		// RST
		reg_val &= ~(1 << bit);
		if (reg_type == RT_HL) {
			bus->write(cpu->reg.read(reg_type), (u8)reg_val);
		}
		else {
			cpu->reg.write(reg_type, reg_val);
		}
		return;
	}
	case 3:
	{
		// SET
		reg_val |= (1 << bit);
		if (reg_type == RT_HL) {
			bus->write(cpu->reg.read(reg_type), (u8)reg_val);
		}
		else {
			cpu->reg.write(reg_type, reg_val);
		}
		return;
	}

	}

	c = cpu->reg.get_flag(CFT_C);

	switch (bit) {
	case 0:
	{
		// RLC
		new_c = false;
		u8 result = reg_val << 1;
		if ((reg_val & (1 << 7)) != 0) {
			result |= 1;
			new_c = true;
		}
		if (reg_type == RT_HL) {
			bus->write(cpu->reg.read(reg_type), result);
		}
		else {
			cpu->reg.write(reg_type, result);
		}
		cpu->reg.set_flags(result == 0, 0, 0, new_c);
		return;
	}

	case 1:
	{
		// RRC
		c = reg_val & 1;
		reg_val = (reg_val >> 1) | (c << 7);

		if (reg_type == RT_HL) {
			bus->write(cpu->reg.read(reg_type), (u8)reg_val);
		}
		else {
			cpu->reg.write(reg_type, reg_val);
		}
		cpu->reg.set_flags((reg_val & 0xFF) == 0, 0, 0, c);
		return;
	}
	
	case 2:
		// RL
	{
		bool new_c = reg_val >> 7;
		reg_val = (reg_val << 1) & 0xFF;
		reg_val |= c;

		if (reg_type == RT_HL) {
			bus->write(cpu->reg.read(reg_type), (u8)reg_val);
		}
		else {
			cpu->reg.write(reg_type, reg_val);
		}
		cpu->reg.set_flags((reg_val & 0xFF) == 0, 0, 0, new_c);
		return;
	}

	case 3:
	{
		// RR
		u16 old = reg_val;
		reg_val >>= 1;
		reg_val |= (c << 7);

		if (reg_type == RT_HL) {
			bus->write(cpu->reg.read(reg_type), (u8)reg_val);
		}
		else {
			cpu->reg.write(reg_type, reg_val);
		}

		if (reg_val) {
			z = 0;
		}
		else {
			z = 1;
		}

		if (old & 1) {
			c = 1;
		}
		else {
			c = 0;
		}
		cpu->reg.set_flags(z, false, false, c);
		return;
	}

	case 4:
	{
		// SLA
		c = (reg_val >> 7);
		reg_val = (reg_val << 1) & 0xFF;
		if (reg_type == RT_HL) {
			bus->write(cpu->reg.read(reg_type), (u8)reg_val);
		}
		else {
			cpu->reg.write(reg_type, reg_val);
		}

		cpu->reg.set_flags((reg_val & 0xFF) == 0, 0, 0, c);
		return;
	}

	case 5:
	{
		// SRA
		c = reg_val & 1;
		bool b7 = reg_val >> 7;
		reg_val = (reg_val >> 1) | (b7 << 7);

		if (reg_type == RT_HL) {
			bus->write(cpu->reg.read(reg_type), (u8)reg_val);
		}
		else {
			cpu->reg.write(reg_type, reg_val);
		}

		cpu->reg.set_flags((reg_val & 0xFF) == 0, 0, 0, c);
		return;
	}

	case 6:
	{
		// SWAP
		reg_val = (((reg_val & 0xF0) >> 4) | ((reg_val & 0xF) << 4)) & 0xFF;

		if (reg_type == RT_HL) {
			bus->write(cpu->reg.read(reg_type), (u8)reg_val);
		}
		else {
			cpu->reg.write(reg_type, reg_val);
		}

		cpu->reg.set_flags((reg_val & 0xFF) == 0, 0, 0, 0);

		return;
	}

	case 7:
		// SRL
	{
		c = reg_val & 1;
		reg_val >>= 1;

		if (reg_type == RT_HL) {
			bus->write(cpu->reg.read(reg_type), (u8)reg_val);
		}
		else {
			cpu->reg.write(reg_type, reg_val);
		}

		cpu->reg.set_flags((reg_val & 0xFF) == 0, 0, 0, c);

		return;
	}
	}
}

void InstructionExecutor::proc_rrca(CPUContext* cpu, Bus* bus)
{
	u8 a = (u8)cpu->reg.read(RT_A);
	bool b = a & 1;
	a >>= 1;
	a |= (b << 7);
	cpu->reg.write(RT_A, a);
	cpu->reg.set_flags(0, 0, 0, b);
}

void InstructionExecutor::proc_rlca(CPUContext* cpu, Bus* bus)
{
	u8 a = (u8)cpu->reg.read(RT_A);
	int cf = a >> 7;
	a = (a << 1) | cf;
	cpu->reg.write(RT_A, a);
	cpu->reg.set_flags(0, 0, 0, cf);
}

void InstructionExecutor::proc_rla(CPUContext* cpu, Bus* bus)
{
	u8 a = (u8)cpu->reg.read(RT_A);
	int cf = cpu->reg.get_flag(CFT_C);
	bool new_cf = (a >> 7) & 1;
	a = (a << 1) | cf;
	cpu->reg.write(RT_A, a);
	cpu->reg.set_flags(0, 0, 0, new_cf);
}

void InstructionExecutor::proc_rra(CPUContext* cpu, Bus* bus)
{
	u8 a = (u8)cpu->reg.read(RT_A);
	bool cf = cpu->reg.get_flag(CFT_C);
	bool new_c = a & 1;
	a >>= 1;
	a |= (cf << 7);
	cpu->reg.write(RT_A, a);
	cpu->reg.set_flags(0, 0, 0, new_c);
}

void InstructionExecutor::proc_stop(CPUContext* cpu, Bus* bus)
{
	cpu->halted = true;
}

void InstructionExecutor::proc_halt(CPUContext* cpu, Bus* bus)
{
	cpu->halted = true;
}

void InstructionExecutor::proc_daa(CPUContext* cpu, Bus* bus)
{
	u8 a = (u8)cpu->reg.read(RT_A);
	bool cf = cpu->reg.get_flag(CFT_C);
	bool n = cpu->reg.get_flag(CFT_N);
	bool h = cpu->reg.get_flag(CFT_H);

	bool new_cf = 0;

	if (!n) {
		if (cf || a > 0x99) {
			a += 0x60;
			new_cf = 1;
		}
		if (h || (a & 0xF) > 0x09) {
			a += 0x6;
		}
	}
	else {
		if (cf) {
			a -= 0x60;
			new_cf = 1;
		}
		if (h) {
			a -= 0x6;
		}
	}


	cpu->reg.write(RT_A, a);
	cpu->reg.set_flags(a == 0, -1, 0, new_cf);
}

void InstructionExecutor::proc_scf(CPUContext* cpu, Bus* bus)
{
	cpu->reg.set_flags(-1, 0, 0, 1);
}

void InstructionExecutor::proc_ccf(CPUContext* cpu, Bus* bus)
{
	bool cf = cpu->reg.get_flag(CFT_C);
	cf ^= 1;
	cpu->reg.set_flags(-1, 0, 0, cf);
}

void InstructionExecutor::proc_ei(CPUContext* cpu, Bus* bus)
{
	cpu->enabling_ime = true;
}

void InstructionExecutor::proc_cpl(CPUContext* cpu, Bus* bus)
{
	u8 a = (u8)cpu->reg.read(RT_A);
	u8 complement = ~a;
	cpu->reg.write(RT_A, complement);
	cpu->reg.set_flags(-1, 1, 1, -1);
	
}

RegisterType u8_to_rt(u8 num) {
	switch (num) {
	case 0:
		return RT_B;
	case 1:
		return RT_C;
	case 2:
		return RT_D;
	case 3:
		return RT_E;
	case 4:
		return RT_H;
	case 5:
		return RT_L;
	case 6:
		return RT_HL;
	case 7:
		return RT_A;
	default:
		std::cout << "Invalid register type value in InstructionExecutor.cpp" << "\n";
		std::cout << num << "\n";
		exit(-1);
	}
	
}