#include "CPU.h"
#include <iostream>
void CPU::step() {
	step_count++;
	if (!ctx.halted) {
		fetch_instruction();
		fetch_data();
		execute();
		bus->debug_update();
		//bus->debug_print();
		if (ctx.cycles_to_increment > 0) {
			increment_cycles(ctx.cycles_to_increment);
			ctx.cycles_to_increment = 0;
		}
	}
	else {
		increment_cycles(1);
		if (bus->read(0xFF0F) && bus->read(0xFFFF)) {
			ctx.halted = false;
		}
	}
	if (ctx.ime) {
		handle_interrupts();
		ctx.enabling_ime = false;
	}

	if (ctx.enabling_ime) {
		ctx.ime = true;
	}
	//if (ctx.reg.read(RT_PC) == 0xc068) {
	//	stepping = true;
	//}
	//print_context();
	//print_pc();
	if (stepping) {
		std::string user_input;
		std::cin >> user_input;
	}
}

void CPU::print_pc() {
	std::cout << "PC: 0x" << std::hex << ctx.reg.read(RT_PC) << "\n";
}

void CPU::print_context() {
	std::cout << "Opcode: 0x" << std::hex << (int)ctx.current_opcode << "\n";
	std::cout << "A: 0x" << std::hex << ctx.reg.read(RT_A) << " ";
	std::cout << "F: 0x" << std::hex << ctx.reg.read(RT_F) << "\n";
	std::cout << "B: 0x" << std::hex << ctx.reg.read(RT_B) << " ";
	std::cout << "C: 0x" << std::hex << ctx.reg.read(RT_C) << "\n";
	std::cout << "D: 0x" << std::hex << ctx.reg.read(RT_D) << " ";
	std::cout << "E: 0x" << std::hex << ctx.reg.read(RT_E) << "\n";
	std::cout << "H: 0x" << std::hex << ctx.reg.read(RT_H) << " ";
	std::cout << "L: 0x" << std::hex << ctx.reg.read(RT_L) << "\n";
	std::cout << "SP: 0x" << std::hex << ctx.reg.read(RT_SP) << "\n";
	std::cout << "PC: 0x" << std::hex << ctx.reg.read(RT_PC) << "\n";
	std::cout << "[";
	if (ctx.reg.get_flag(CFT_Z)) {
		std::cout << "Z";
	}
	else {
		std::cout << "-";
	}
	if (ctx.reg.get_flag(CFT_N)) {
		std::cout << "N";
	}
	else {
		std::cout << "-";
	}
	if (ctx.reg.get_flag(CFT_C)) {
		std::cout << "C";
	}
	else {
		std::cout << "-";
	}
	if (ctx.reg.get_flag(CFT_H)) {
		std::cout << "H";
	}
	else {
		std::cout << "-";
	}
	std::cout << "]\n\n";




}

void CPU::fetch_instruction() {
	u16 address = ctx.reg.read(RT_PC);
	ctx.current_opcode = bus->read(address);
	ctx.reg.write(RT_PC, address + 1);
	ctx.current_instruction = INSTRUCTIONS[ctx.current_opcode];

}

void CPU::fetch_data()
{
	ctx.memory_destination = 0;
	ctx.destination_is_memory = false;

	switch (ctx.current_instruction.mode) {
	case AM_IMP:
	{
		break;
	}
	case AM_R:
	{
		ctx.fetched_data = ctx.reg.read(ctx.current_instruction.reg1);
		break;
	}
	case AM_R_R:
	{
		ctx.fetched_data = ctx.reg.read(ctx.current_instruction.reg2);
		break;
	}
	case AM_R_D8:
	{
		u16 pc = ctx.reg.read(RT_PC);
		ctx.fetched_data = bus->read(pc);
		increment_cycles(1);
		ctx.reg.write(RT_PC, pc + 1);
		break;
	}
	case AM_D16:
		
	case AM_R_D16:
	{
		u16 pc = ctx.reg.read(RT_PC);
		u8 low = bus->read(pc);
		increment_cycles(1);
		u8 high = bus->read(pc + 1);
		increment_cycles(1);
		ctx.fetched_data = (high << 8) | low;
		ctx.reg.write(RT_PC, pc + 2);
		break;
	}

	case AM_MR_R:
	{
		ctx.fetched_data = ctx.reg.read(ctx.current_instruction.reg2);
		ctx.memory_destination = ctx.reg.read(ctx.current_instruction.reg1);
		ctx.destination_is_memory = true;

		if (ctx.current_instruction.reg1 == RT_C) {
			ctx.memory_destination |= 0xFF00;
		}
		break;
	}

	case AM_R_MR:
	{
		u16 address = ctx.reg.read(ctx.current_instruction.reg2);
		if (ctx.current_instruction.reg2 == RT_C) {
			address |= 0xFF00;
		}
		ctx.fetched_data = bus->read(address);
		increment_cycles(1);
		break;
	}

	case AM_R_HLI:
	{
		ctx.fetched_data = bus->read(ctx.reg.read(ctx.current_instruction.reg2));
		increment_cycles(1);
		ctx.reg.add(RT_HL, 1);
		break;
	}

	case AM_R_HLD:
	{
		ctx.fetched_data = bus->read(ctx.reg.read(ctx.current_instruction.reg2));
		increment_cycles(1);
		ctx.reg.add(RT_HL, -1);
		break;
	}

	case AM_HLI_R:
	{
		ctx.fetched_data = ctx.reg.read(ctx.current_instruction.reg2);
		ctx.memory_destination = ctx.reg.read(ctx.current_instruction.reg1);
		ctx.destination_is_memory = true;
		ctx.reg.add(RT_HL, 1);
		break;
	}

	case AM_HLD_R:
	{
		ctx.fetched_data = ctx.reg.read(ctx.current_instruction.reg2);
		ctx.memory_destination = ctx.reg.read(ctx.current_instruction.reg1);
		ctx.destination_is_memory = true;
		ctx.reg.add(RT_HL, -1);
		break;
	}

	case AM_R_A8:
	{
		ctx.fetched_data = bus->read(ctx.reg.read(RT_PC));
		increment_cycles(1);
		ctx.reg.add(RT_PC, 1);
		break;
	}

	case AM_A8_R:
	{
		ctx.memory_destination = bus->read(ctx.reg.read(RT_PC));
		ctx.destination_is_memory = true;
		ctx.fetched_data = ctx.reg.read(ctx.current_instruction.reg2);
		increment_cycles(1);
		ctx.reg.add(RT_PC, 1);
		break;
	}

	case AM_HL_SPR:

	case AM_D8:
	{
		ctx.fetched_data = bus->read(ctx.reg.read(RT_PC));
		increment_cycles(1);
		ctx.reg.add(RT_PC, 1);
		break;
	}

	case AM_MR_D8:
	{
		ctx.fetched_data = bus->read(ctx.reg.read(RT_PC));
		increment_cycles(1);
		ctx.reg.add(RT_PC, 1);
		ctx.memory_destination = ctx.reg.read(ctx.current_instruction.reg1);
		ctx.destination_is_memory = true;
		break;
	}

	case AM_MR:
	{
		ctx.memory_destination = ctx.reg.read(ctx.current_instruction.reg1);
		ctx.destination_is_memory = true;
		ctx.fetched_data = bus->read(ctx.reg.read(ctx.current_instruction.reg1));
		increment_cycles(1);
		break;
	}

	case AM_D16_R:

	case AM_A16_R:
	{
		u8 low = bus->read(ctx.reg.read(RT_PC));
		increment_cycles(1);
		u8 high = bus->read(ctx.reg.read(RT_PC) + 1);
		increment_cycles(1);
		ctx.memory_destination = (high << 8) | low;
		ctx.destination_is_memory = true;
		ctx.reg.add(RT_PC, 2);
		ctx.fetched_data = ctx.reg.read(ctx.current_instruction.reg2);
		break;
	}

	case AM_R_A16:
	{
		u8 low = bus->read(ctx.reg.read(RT_PC));
		increment_cycles(1);
		u8 high = bus->read(ctx.reg.read(RT_PC) + 1);
		increment_cycles(1);
		u16 address = (high << 8) | low;
		ctx.reg.add(RT_PC, 2);
		ctx.fetched_data = bus->read(address);
		increment_cycles(1);
		break;
	}

	}

}

void CPU::init()
{
}

void CPU::execute()
{
	executor.execute(&ctx, bus);
}


void CPU::increment_cycles(int cycles)
{
	for (int i = 0; i < cycles; i++) {
		for (int j = 0; j < 4; j++) {
			ticks++;
			if (timer->tick()) {
				request_interrupt(IT_TIMER);
			}
			
		}
	}
}

void CPU::int_handle(u16 address)
{
	u16 pc = ctx.reg.read(RT_PC);
	executor.stack_push16(&ctx, bus, pc);
	ctx.reg.write(RT_PC, address);
}

void CPU::request_interrupt(InterruptType type)
{
	u16 interrupt_flag = bus->read(0xFF0F);
	u8 new_flag = interrupt_flag | type;
	bus->write(0xFF0F, new_flag);
}


bool CPU::interrupt_check(u16 address, InterruptType type)
{
	u16 interrupt_flag = bus->read(0xFF0F);
	u16 interrupt_enable = bus->read(0xFFFF);

	if ((interrupt_flag & type) && (interrupt_enable & type)) {
		int_handle(address);
		bus->write(0xFF0F, interrupt_flag & ~type);
		ctx.halted = false;
		ctx.ime = false;
		return true;
	}
	return false;

}

void CPU::handle_interrupts()
{
	if (interrupt_check(0x40, IT_VBLANK)) {
	}
	else if (interrupt_check(0x48, IT_LCD_STAT)) {
	}
	else if (interrupt_check(0x50, IT_TIMER)) {
	}
	else if (interrupt_check(0x58, IT_SERIAL)) {
	}
	else if (interrupt_check(0x60, IT_JOYPAD)){
	}
}

void CPU::connect(Bus* b, Timer* t, PPU* p)
{
	bus = b;
	timer = t;
	ppu = p;
}
