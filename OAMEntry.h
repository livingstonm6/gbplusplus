#pragma once
#include "Common.h"
class OAMEntry {
public:
	u8 y = 0;
	u8 x = 0;
	u8 tile = 0;
	int flags = 0;
	u8 cgb_palette = 0;
	bool cgb_vram_bank = 0;
	bool pn = 0;
	bool x_flip = 0;
	bool y_flip = 0;
	bool bgp = 0;

	void update(u8 value, u8 byte_number);

};