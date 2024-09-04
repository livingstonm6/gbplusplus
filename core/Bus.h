#pragma once
#include "utility/Common.h"
#include "Timer.h"
#include "Cartridge.h"
#include "LCD.h"
#include "Joypad.h"
#include "APU.h"
#include "PPUMemory.h"

class Bus
{
	Joypad joypad;
	bool dma_active{};
	u8 dma_byte{};
	u8 dma_value{};
	u8 dma_start_delay{};

	std::vector<char> debug_message{};

	u8 memory[0x10000]{};

	PPUMemory* ppu_memory = nullptr;
	LCD* lcd = nullptr;
	Cartridge* cartridge = nullptr;
	Timer* timer = nullptr;
	APU* apu = nullptr;

	CartridgeType cartridge_type{};
	u8 rom_bank_number{};

public:
	u8 read(u16 address);
	u16 read16(u16 address);
	void write(u16 address, u8 value);
	void write16(u16 address, u16 value);

	void debug_update();
	void debug_print();

	void dma_start(u8 start);
	void tick_dma();

	void update_joypad(JoypadInputType, bool);

	void connect(PPUMemory* p, LCD* l, Cartridge* c, Timer* t, APU* a);
	

};

