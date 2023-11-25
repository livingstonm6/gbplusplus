#pragma once
#include "common.h"
#include "PPU.h"
#include "Timer.h"
#include "Cartridge.h"
#include "LCD.h"

class RAM
{
private:
	u8 wram[0x2000];
	u8 hram[0x80];
public:
	u8 read_wram(u16 address);
	u8 read_hram(u16 address);
	
	void write_wram(u16 address, u8 value);
	void write_hram(u16 address, u8 value);
};

class IORegisters
{
public:
	u8 serial_data[2];
	u8 interrupt_enable;
	u8 interrupt_flag;

	u8 read(u16 address);
	void write(u16 address, u8 value);
};

class Bus
{
	bool dma_active{};
	u8 dma_byte{};
	u8 dma_value{};
	u8 dma_start_delay{};

	IORegisters io_reg{};
	std::vector<char> debug_message{};


	RAM ram{};
	PPU* ppu = nullptr;
	LCD* lcd = nullptr;
	Cartridge* cartridge = nullptr;
	Timer* timer = nullptr;

public:
	u8 read(u16 address);
	u16 read16(u16 address);
	void write(u16 address, u8 value);
	void write16(u16 address, u16 value);

	void debug_update();
	void debug_print();

	void dma_start(u8 start);
	void dma_tick();

	void connect(PPU* p, LCD* l, Cartridge* c, Timer* t);
	

};

