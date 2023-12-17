#include "OAMEntry.h"

void OAMEntry::update(u8 value, u8 byte_index)
{
	switch (byte_index) {
	case 0:
		y = value;
		break;
	case 1:
		x = value;
		break;
	case 2:
		tile = value;
		break;
	case 3:
		cgb_palette = value & 0b111;
		cgb_vram_bank = value & (1 << 3);
		pn = value & (1 << 4);
		x_flip = value & (1 << 5);
		y_flip = value & (1 << 6);
		bgp = value & (1 << 7);
	}
}