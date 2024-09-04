#include "PPUMemory.h"
u8 PPUMemory::oam_read(u16 address)
{
	if (address >= 0xFE00) {
		address -= 0xFE00;
	}
	u16 oam_index = address / 4;
	u8 byte_index = address % 4;
	switch (byte_index) {
	case 0:
		return oam_ram[address].y;
	case 1:
		return oam_ram[address].x;
	case 2:
		return oam_ram[address].tile;
	case 3:
		u8 value = oam_ram[address].cgb_palette | (oam_ram[address].cgb_vram_bank << 3)
			| (oam_ram[address].pn << 4) | (oam_ram[address].x_flip << 5)
			| (oam_ram[address].y_flip << 6) | (oam_ram[address].bgp << 7);
		return value;
	};
}

void PPUMemory::oam_write(u16 address, u8 value)
{
	if (address >= 0xFE00) {
		address -= 0xFE00;
	}
	u16 oam_index = address / 4;
	u8 byte_index = address % 4;
	oam_ram[oam_index].update(value, byte_index);
}
