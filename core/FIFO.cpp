#include "FIFO.h"
#include <iostream>

void FIFO::push(u32 value)
{
	queue.push_back(value);
}

u32 FIFO::pop()
{
	u32 value = queue.front();
	queue.pop_front();
	return value;
}

void FIFO::reset()
{
	while (!queue.empty()) {
		pop();
	}
}

u32 FIFO::fetch_sprite_pixels(u8 bit, u32 color, u8 bg_color, LCD* lcd, std::vector<OAMEntry>* oam_ram)
{

	for (int i = 0; i < oam_ram->size(); i++) {
		int sprite_x = (oam_ram->at(i).x - 8) + (lcd->scroll_x % 8);

		if (sprite_x + 8 < fifo_x) {
			continue;
		}

		int offset = fifo_x - sprite_x;

		if (offset < 0 || offset > 7) {
			continue;
		}

		bit = (7 - offset);

		if (oam_ram->at(i).x_flip) {
			bit = offset;
		}

		u8 high = !!(fetch_entry_data[i * 2] & (1 << bit));
		u8 low = !!(fetch_entry_data[(i * 2) + 1] & (1 << bit)) << 1;

		bool bg_priority = oam_ram->at(i).bgp;

		if (!(high | low)) {
			continue;
		}

		if (!bg_priority || bg_color == 0) {
			color = oam_ram->at(i).pn ? lcd->sp2_colors[high | low] : lcd->sp1_colors[high | low];
			if (high | low) {
				break;
			}
		}
	}

	return color;

}

bool FIFO::add(LCD* lcd, std::vector<OAMEntry>* oam_ram)
{

	if (queue.size() > 8) {
		return false;
	}
	int x = fetch_x - (8 - (lcd->scroll_x % 8));

	for (u8 i = 0; i < 8; i++) {
		u8 bit = 7 - i;
		u8 low = !!(bgw_fetch_data[1] & (1 << bit));
		u8 high = !!(bgw_fetch_data[2] & (1 << bit)) << 1;
		u32 color = lcd->bg_colors[high | low];

		if (!lcd->get_bgw_enable()) {
			color = lcd->bg_colors[0];
		}

		if (lcd->get_obj_enable()) {
			color = fetch_sprite_pixels(bit, color, high | low, lcd, oam_ram);
		}

		if (x >= 0) {
			push(color);
			fifo_x++;
		}
	}
	return true;
}

u8 FIFO::size()
{
	return queue.size();
}

