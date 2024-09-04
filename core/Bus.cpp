#include "Bus.h"
#include <iostream>

void Bus::connect(PPUMemory* p, LCD* l, Cartridge* c, Timer* t, APU* a) {
	ppu_memory = p;
	lcd = l;
	cartridge = c;
	timer = t;
	apu = a;
	cartridge_type = static_cast<CartridgeType>(cartridge->cartridge_type);
}

u8 Bus::read(u16 address) {
	if (address == 0xFF00) {
		return joypad.read();
	}
	else if (address < 0x8000 || (0xA000 <= address && address < 0xC000)) {
		return cartridge->read(address);
	}
	else if (0xFE00 <= address && address < 0xFE9F) {
		if (dma_active) {
			return 0xFF;
		}
		return ppu_memory->oam_read(address);
	}
	else if (0xFF04 <= address && address < 0xFF08) {
		return timer->read(address);
	}
	else if (0xFF10 <= address && address < 0xFF40) {
		return apu->read(address);
	}
	else if (0xFF40 <= address && address < 0xFF4C) {
		return lcd->read(address);
	}
	else {
		return memory[address];
	}
}

u16 Bus::read16(u16 address)
{
	u8 low = read(address);
	u8 high = read(address + 1);
	return low | (high << 8);
}

void Bus::write(u16 address, u8 value) {

	if (address >= 0xFF4C) {
		memory[address] = value;
		return;
	}

	if (address == 0xFF00) {
		joypad.write(value);
	}
	else if (address < 0x8000 || (0xA000 <= address && address < 0xC000)) {
		cartridge->write(address, value);
	}
	else if (0xFE00 <= address && address < 0xFEA0) {
		if (dma_active) {
			return;
		}
		ppu_memory->oam_write(address, value);
	}
	else if (0xFF04 <= address && address < 0xFF08) {
		timer->write(address, value);
	}
	else if (0xFF10 <= address && address < 0xFF40) {
		apu->write(address, value);
	}
	else if (address == 0xFF46) {
		dma_start(value);
	}
	else if (0xFF40 <= address && address < 0xFF4C) {
		lcd->write(address, value);
	}
	else {
		memory[address] = value;
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

void Bus::tick_dma()
{
	if (!dma_active) {
		return;
	}
	if (dma_start_delay > 0) {
		dma_start_delay--;
		return;
	}
	u8 value = read((dma_value * 0x100) + dma_byte);
	ppu_memory->oam_write(dma_byte, value);
	dma_byte++;
	dma_active = dma_byte < 0xA0;
}

void Bus::update_joypad(JoypadInputType input_type, bool pressed)
{

	// pressed = true if button is pressed
	// in register 0xFF00 and joypad class, bit = 0 if button is pressed

	switch (input_type) {
	case JIT_SELECT:
		joypad.select = !pressed;
		break;
	case JIT_START:
		joypad.start = !pressed;
		break;
	case JIT_A:
		joypad.a = !pressed;
		break;
	case JIT_B:
		joypad.b = !pressed;
		break;
	case JIT_LEFT:
		joypad.left = !pressed;
		break;
	case JIT_RIGHT:
		joypad.right = !pressed;
		break;
	case JIT_DOWN:
		joypad.down = !pressed;
		break;
	case JIT_UP:
		joypad.up = !pressed;
		break;
	}
}




