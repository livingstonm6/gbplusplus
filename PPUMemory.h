#pragma once
#include "Common.h"
#include <vector>
#include "OAMEntry.h"
class PPUMemory
{
public:

	std::vector<OAMEntry> oam_ram{};
	u8 vram[0x2000]{};

	u8 oam_read(u16 address);
	void oam_write(u16 address, u8 value);

	u8 vram_read(u16 address);
	void vram_write(u16 address, u8 value);

	PPUMemory() {
		OAMEntry blank_entry{};
		for (int i = 0; i < 0xA0; i++) {
			oam_ram.push_back(blank_entry);
		}

	}

};

