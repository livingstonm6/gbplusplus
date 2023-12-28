#include "Bus.h"
#include <iostream>

u8 RAM::read_wram(u16 address) {
	address -= 0xC000;
	return wram[address];
}

void RAM::write_wram(u16 address, u8 value) {
	address -= 0xC000;
	wram[address] = value;
}

u8 RAM::read_hram(u16 address) {
	address -= 0xFF80;
	return hram[address];
}

void RAM::write_hram(u16 address, u8 value) {
	address -= 0xFF80;
	hram[address] = value;
}

u8 IORegisters::read(u16 address)
{

	if (address == 0xFF00) {
		return joypad.read();
	}
	else if (address == 0xFF01) {
		return serial_data[0];
	}
	else if (address == 0xFF02) {
		return serial_data[1];
	}
	else if (address == 0xFF4D) {
		return 0xFF;
	}
	else if (address == 0xFF0F) {
		return interrupt_flag;
	}
	else if (address == 0xFFFF) {
		return interrupt_enable;
	}
	//std::cout << "Error reading IORegister: 0x" << std::uppercase << std::hex << address << "\n";
	//exit(-1);
	return 0;
}

void IORegisters::write(u16 address, u8 value) {

	if (address == 0xFF00) {
		joypad.write(value);
	}
	else if (address == 0xFF01) {
		serial_data[0] = value;
	}
	else if (address == 0xFF02) {
		serial_data[1] = value;
	}
	else if (address == 0xFF0F) {
		interrupt_flag = value;
	}
	else if (address == 0xFFFF) {
		interrupt_enable = value;
	}

}

void Bus::connect(PPU* p, LCD* l, Cartridge* c, Timer* t, APU* a) {
	ppu = p;
	lcd = l;
	cartridge = c;
	timer = t;
	apu = a;
	cartridge_type = static_cast<CartridgeType>(cartridge->cartridge_type);
}

u8 Bus::read(u16 address) {
	if (address < 0x4000) {
		return cartridge->read(address);
	}
	else if (address < 0x8000) {
		return cartridge->read(address);
	}
	else if (address < 0xA000) {
		return ppu->vram_read(address);
	}
	else if (address < 0xC000) {
		return cartridge->read(address);
	}
	else if (address < 0xE000) {
		return ram.read_wram(address);
	}
	else if (address < 0xFE00){
		return 0;
	}
	else if (address < 0xFE9F) {
		if (dma_active) {
			return 0xFF;
		}
		return ppu->oam_read(address);
	}
	else if (address < 0xFF00) {
		return 0;
	}
	else if (address < 0xFF04) {
		return io_reg.read(address);
	}
	else if (address < 0xFF08) {
		return timer->read(address);
	}
	else if (address < 0xFF10) {
		return io_reg.read(address);
	}
	else if (address < 0xFF40) {
		return apu->read(address);
	}
	else if (address < 0xFF4C) {
		return lcd->read(address);
	}
	else if (address < 0xFF80) {
		return io_reg.read(address);
	}
	else if (address == 0xFFFF) {
		return io_reg.read(address);
	}
	else {
		return ram.read_hram(address);
	}
}

u16 Bus::read16(u16 address)
{
	u8 low = read(address);
	u8 high = read(address + 1);
	return low | (high << 8);
}

void Bus::write(u16 address, u8 value) {

	if (address < 0x8000) {
		cartridge->write(address, value);
	}
	else if (address < 0xA000) {
		ppu->vram_write(address, value);
	}
	else if (address < 0xC000) {
		cartridge->write(address, value);
	}
	else if (address < 0xE000) {
		ram.write_wram(address, value);
	}
	else if (address < 0xFE00) {
		return;
	}
	else if (address < 0xFEA0) {
		if (dma_active) {
			return;
		}
		ppu->oam_write(address, value);
	}
	else if (address < 0xFF00) {
		return;
	}
	else if (address < 0xFF04) {
		io_reg.write(address, value);
	}
	else if (address < 0xFF08) {
		timer->write(address, value);
	}
	else if (address < 0xFF10) {
		io_reg.write(address, value);
	}
	else if (address < 0xFF40) {
		apu->write(address, value);
	}
	else if (address < 0xFF46) {
		lcd->write(address, value);
	}
	else if (address == 0xFF46) {
		dma_start(value);
	}
	else if (address < 0xFF4C) {
		lcd->write(address, value);
	}
	else if (address < 0xFF80) {
		io_reg.write(address, value);
	}
	else if (address == 0xFFFF) {
		io_reg.write(address, value);
	}
	else {
		ram.write_hram(address, value);
	}
}

void Bus::write16(u16 address, u16 value)
{
	write(address + 1, (value >> 8));
	write(address, value & 0xFF);
}

void Bus::debug_update()
{
	if (read(0xFF02) == 0x81) {
		debug_message.push_back(read(0xFF01));
		write(0xFF02, 0);
		debug_print();
	}
}

void Bus::debug_print()
{
	if (!debug_message.empty()) {
		std::cout << "DBG:";
		for (const char c : debug_message) {
			std::cout << c;
		}
		std::cout << "\n";
	}
}

void Bus::dma_start(u8 start)
{
	dma_active = true;
	dma_byte = 0;
	dma_start_delay = 2;
	dma_value = start;
}

void Bus::dma_tick()
{
	if (!dma_active) {
		return;
	}
	if (dma_start_delay > 0) {
		dma_start_delay--;
		return;
	}
	u8 value = read((dma_value * 0x100) + dma_byte);
	ppu->oam_write(dma_byte, value);
	dma_byte++;
	dma_active = dma_byte < 0xA0;
}

void Bus::update_joypad(JoypadInputType input_type, bool pressed)
{

	// pressed = true if button is pressed
	// in register 0xFF00 and joypad class, bit = 0 if button is pressed

	switch (input_type) {
	case JIT_SELECT:
		io_reg.joypad.select = !pressed;
		break;
	case JIT_START:
		io_reg.joypad.start = !pressed;
		break;
	case JIT_A:
		io_reg.joypad.a = !pressed;
		break;
	case JIT_B:
		io_reg.joypad.b = !pressed;
		break;
	case JIT_LEFT:
		io_reg.joypad.left = !pressed;
		break;
	case JIT_RIGHT:
		io_reg.joypad.right = !pressed;
		break;
	case JIT_DOWN:
		io_reg.joypad.down = !pressed;
		break;
	case JIT_UP:
		io_reg.joypad.up = !pressed;
		break;
	}
}




