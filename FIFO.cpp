#include "FIFO.h"
#include <iostream>

void FIFO::push(u32 value)
{
	struct FIFOEntry* next_entry = new FIFOEntry{nullptr, value};
	if (head == nullptr) {
		head = next_entry;
		tail = next_entry;
	}
	else {
		tail->next = next_entry;
		tail = next_entry;
	}
	size++;
}

u32 FIFO::pop()
{
	if (size < 1) {
		std::cout << "ERROR, pop at empty pixel fifo\n";
		exit(-1);
	}
	u32 value = head->value;
	
	if (size == 1) {
		delete head;
		head = tail = nullptr;
	}
	else {
		FIFOEntry* next_head = head->next;
		delete head;
		head = next_head;
	}

	size--;
	return value;
}

void FIFO::reset()
{
	while (size > 0) {
		pop();
	}
	head = nullptr;
	tail = nullptr;
}

u32 FIFO::fetch_sprite_pixels(u8 bit, u32 color, u8 bg_color, LCD* lcd, std::vector<OAMEntry>* oam_ram)
{
	bool set_new_color = false;
	u32 new_color = 0;
	int i = 0;
	for (auto& object : *oam_ram) {
		int sprite_x = (object.x - 8) + (lcd->scroll_x % 8);
		if (sprite_x + 8 >= fifo_x) {
			int offset = fifo_x - sprite_x;
			if (0 <= offset <= 7) {
				bit = (7 - offset);
				if (object.x_flip) {
					bit = offset;
				}
				// todo get high and low
				u8 high = !!(fetch_entry_data[i * 2] & (1 << bit));
				u8 low = !!(fetch_entry_data[(i * 2) + 1] & (1 << bit)) << 1;

				if (!(high | low)) {
					continue;
				}
				
				if (!object.bgp || bg_color == 0) {
					if (object.pn) {
						new_color = lcd->sp2_colors[high | low];
						set_new_color = true;
					}
					else {
						new_color = lcd->sp1_colors[high | low];
						set_new_color = true;
					}

					if (high | low) {
						break;
					}
				}
			}
		}
		i++;
	}
	if (set_new_color) {
		return new_color;
	}
	return color;
}

bool FIFO::add(LCD* lcd, std::vector<OAMEntry>* oam_ram)
{

	if (size > 8) { 
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

