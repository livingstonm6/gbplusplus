#include "CPU.h"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <bitset>
void CPU::step() {
	u16 pc = ctx.reg.read(RT_PC);
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

	if (debug_logging) {
		update_logfile();
	}


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
	std::cout << "IME: " << ctx.ime << "\n";
	std::bitset<8> interrupt_flag(bus->read(0xFF0F));
	std::cout << "IF:" << interrupt_flag << "\n";
	std::bitset<8> interrupt_enable_flag(bus->read(0xFFFF));
	std::cout << "IEF:" << interrupt_enable_flag << "\n";
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
		ctx.fetched_data = bus->read(ctx.reg.read(RT_PC)) | 0xFF00;
		increment_cycles(1);
		ctx.reg.add(RT_PC, 1);
		break;
	}

	case AM_A8_R:
	{
		ctx.memory_destination = bus->read(ctx.reg.read(RT_PC)) | 0xFF00;
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

void CPU::tick_ppu()
{
	if (lcd->get_lcd_mode() == LM_XFER) {
		ppu->fifo.map_y = lcd->ly + lcd->scroll_y;
		ppu->fifo.map_x = ppu->fifo.fifo_x + lcd->scroll_x;
		ppu->fifo.tile_y = ((lcd->ly + lcd->scroll_y) % 8) * 2;
	}
	if (lcd->get_lcd_mode() == LM_XFER && !((ppu->line_ticks + 1) & 0b1)) {
		switch (ppu->fifo.current_fetch_state) {
		case FS_TILE:
		{
			//		u16 address = lcd->get_bg_tile_map() + (((ppu->fifo.fifo_x + lcd->scroll_x / 8) & 0x1F
			//			+ (((ppu->fifo.map_y & 0xFF) / 8) * 32)) & 0x3FF);

			bool lcdc_flag = 0;
			if (lcd->get_bg_tile_map() == 0x9C00) {
				lcdc_flag = 1;
			}
			u8 address_high_6 = 0b100110 | lcdc_flag;
			u16 address_low_10 = ((((lcd->ly + lcd->scroll_y)  & 0xFF) / 8) << 5) | ((((ppu->fifo.fetch_x + lcd->scroll_x) & 0xFF) / 8));
			u16 address = (address_high_6 << 10) | address_low_10;
			
			u8 fetch = bus->read(address);
			ppu->tick(&ctx, fetch);
			break;
		}
		case FS_DATA_0:
		{
			bool bit_12 = 0;
			if (lcd->get_bgw_tile_data_am() == 0x8800) {
				bit_12 = !(ppu->fifo.bgw_fetch_data[0] >> 7);
			}
			u8 bottom_4 = ((lcd->ly + lcd->scroll_y) % 8) << 1;
			u16 address = (0b100 << 13) | (bit_12 << 12) | (ppu->fifo.bgw_fetch_data[0] << 4)
				| bottom_4;

			u8 fetch = bus->read(address);
			ppu->tick(&ctx, fetch);
			break;
		}
		case FS_DATA_1:
		{
			bool bit_12 = 0;
			if (lcd->get_bgw_tile_data_am() == 0x8800) {
				bit_12 = !(ppu->fifo.bgw_fetch_data[0] >> 7);
			}
			u8 bottom_4 = (((lcd->ly + lcd->scroll_y) % 8) << 1) | 1;
			u16 address = (0b100 << 13) | (bit_12 << 12) | (ppu->fifo.bgw_fetch_data[0] << 4)
				| bottom_4;
			u8 fetch = bus->read(address);
			ppu->tick(&ctx, fetch);
			break;
		}
		default:
			ppu->tick(&ctx);
			break;
		}
	}
	else {
		ppu->tick(&ctx);
	}

	check_ppu_interrupts();

}

void CPU::check_ppu_interrupts()
{
	for (InterruptType interrupt : ctx.ppu_interrupt_queue) {
		request_interrupt(interrupt);
	}
	ctx.ppu_interrupt_queue.clear();
}

void CPU::update_logfile()
{
	std::ofstream log_file("log.txt", std::ios::out | std::ios::app);
	if (log_file.is_open()) {
		log_file << "A:" << std::setw(2) << std::setfill('0') << std::hex << std::uppercase << ctx.reg.read(RT_A);
		log_file << " F:" << std::setw(2) << std::setfill('0') << std::hex << std::uppercase << ctx.reg.read(RT_F);
		log_file << " B:" << std::setw(2) << std::setfill('0') << std::hex << std::uppercase << ctx.reg.read(RT_B);
		log_file << " C:" << std::setw(2) << std::setfill('0') << std::hex << std::uppercase << ctx.reg.read(RT_C);
		log_file << " D:" << std::setw(2) << std::setfill('0') << std::hex << std::uppercase << ctx.reg.read(RT_D);
		log_file << " E:" << std::setw(2) << std::setfill('0') << std::hex << std::uppercase << ctx.reg.read(RT_E);
		log_file << " H:" << std::setw(2) << std::setfill('0') << std::hex << std::uppercase << ctx.reg.read(RT_H);
		log_file << " L:" << std::setw(2) << std::setfill('0') << std::hex << std::uppercase << ctx.reg.read(RT_L);
		log_file << " SP:" << std::setw(4) << std::setfill('0') << std::hex << std::uppercase << ctx.reg.read(RT_SP);
		u16 pc = ctx.reg.read(RT_PC);
		log_file << " PC:" << std::setw(4) << std::setfill('0') << std::hex << std::uppercase << pc;
		u16 mem1 = bus->read(pc);
		u16 mem2 = bus->read(pc + 1);
		u16 mem3 = bus->read(pc + 2);
		u16 mem4 = bus->read(pc + 3);
		log_file << " PCMEM:" << std::setw(2) << std::setfill('0') << std::hex << std::uppercase << mem1;
		log_file << "," << std::setw(2) << std::setfill('0') << std::hex << std::uppercase << mem2;
		log_file << "," << std::setw(2) << std::setfill('0') << std::hex << std::uppercase << mem3;
		log_file << "," << std::setw(2) << std::setfill('0') << std::hex << std::uppercase << mem4 << "\n";
		log_file.close();
	}
	else {
		std::cout << "Error updating log file.\n";
		exit(-1);
	}
	
}

void CPU::init()
{
	// clear log file
	if (debug_logging) {
		std::ofstream log_file;
		log_file.open("log.txt", std::ios::out | std::ios::trunc);
		log_file.close();
	}
	
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

			tick_ppu();
			
		}
		bus->dma_tick();
	}
}

void CPU::int_handle(u16 address)
{
	increment_cycles(5);
	u16 pc = ctx.reg.read(RT_PC);
	executor.stack_push16(&ctx, bus, pc);
	ctx.reg.write(RT_PC, address);
}

void CPU::request_interrupt(InterruptType type)
{
	u8 interrupt_flag = bus->read(0xFF0F);
	u8 new_flag = interrupt_flag | type;
	bus->write(0xFF0F, new_flag);
}


bool CPU::interrupt_check(u16 address, InterruptType type)
{
	u8 interrupt_flag = bus->read(0xFF0F);
	u8 interrupt_enable = bus->read(0xFFFF);

	//std::cout << "INTERRUPT CHECK\n";
	//std::bitset<8> interrupt_flag_bs(bus->read(0xFF0F));
	//std::cout << "IF : " << interrupt_flag_bs << "\n";
	//std::bitset<8> interrupt_enable_flag(bus->read(0xFFFF));
	//std::cout << "IEF: " << interrupt_enable_flag << "\n";


	if ((interrupt_flag & type) && (interrupt_enable & type)) {
		u8 complement = ~static_cast<u8>(type);
		int_handle(address);
		bus->write(0xFF0F, interrupt_flag & complement);
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

void CPU::connect(Bus* b, Timer* t, PPU* p, LCD* l)
{
	bus = b;
	timer = t;
	ppu = p;
	lcd = l;
}
