#include "CPU.h"

void CPU::connect(Bus* bus_param, Timer* timer_param, PPU* ppu_param, APU* apu_param)
{
	bus = bus_param;
	timer = timer_param;
	ppu = ppu_param;
	apu = apu_param;

}

void CPU::step() {
	u16 pc = ctx.reg.read(RT_PC);
	if (!ctx.halted) {
		fetcher.fetch_instruction(&ctx, bus);
		fetcher.fetch_data(&ctx, bus);
		executor.execute_instruction(&ctx, bus);
		bus->debug_update(); // check serial debug output
		if (ctx.cycles_to_increment > 0) {
			// cycles passed during fetching and instruction execution
			increment_cycles(ctx.cycles_to_increment);
			ctx.cycles_to_increment = 0;
		}
//		apu->tick();
	}
	else {
		increment_cycles(1);
		// Check interrupt flag and interrupt enable flag
		if (bus->read(0xFF0F) && bus->read(0xFFFF)) {
			ctx.halted = false;
		}
	}

	if (ctx.interrupt_master_enable) {
		handle_interrupts();
		ctx.enabling_ime = false;
	}

	// Interrupt master enable flag is delayed by 1 instruction
	if (ctx.enabling_ime) {
		ctx.interrupt_master_enable = true;
	}

}

void CPU::tick_ppu()
{
	ppu->tick(&ctx);
	for (InterruptType interrupt : ctx.ppu_interrupts) {
		request_interrupt(interrupt);
	}
	ctx.ppu_interrupts.clear();

}


void CPU::increment_cycles(const int num_cycles)
{
	for (int i = 0; i < num_cycles; i++) {
		for (int j = 0; j < 4; j++) {
			if (timer->tick()) {
				request_interrupt(IT_TIMER);
			}
			tick_ppu();
			apu->tick();
		}
		bus->tick_dma();	// DMA ticks every cpu cycle if active
//		apu->tick();
	}
}



void CPU::request_interrupt(const InterruptType type)
{
	u8 interrupt_flag = bus->read(0xFF0F);
	u8 new_flag = interrupt_flag | type;
	bus->write(0xFF0F, new_flag);
}

void CPU::handle_interrupts()
{
	if (check_interrupt(0x40, IT_VBLANK)) { // Only handle the first
	}
	else if (check_interrupt(0x48, IT_LCD_STAT)) {
	}
	else if (check_interrupt(0x50, IT_TIMER)) {
	}
	else if (check_interrupt(0x58, IT_SERIAL)) {
	}
	else if (check_interrupt(0x60, IT_JOYPAD)) {
	}
}

bool CPU::check_interrupt(const u16 address, const InterruptType type)
{
	u8 interrupt_flag = bus->read(0xFF0F);
	u8 interrupt_enable = bus->read(0xFFFF);

	if ((interrupt_flag & type) && (interrupt_enable & type)) {
		u8 complement = ~static_cast<u8>(type);  // use complement to set bit corresponding to type to 0
		jump_to_interrupt_handler(address);
		bus->write(0xFF0F, interrupt_flag & complement);
		ctx.halted = false;
		ctx.interrupt_master_enable = false;	 // IME is reset after one interrupt
		return true;
	}
	return false;

}

void CPU::jump_to_interrupt_handler(const u16 address)
{
	u16 pc = ctx.reg.read(RT_PC);
	executor.stack_push16(&ctx, bus, pc);
	ctx.reg.write(RT_PC, address);
	increment_cycles(5);
}

