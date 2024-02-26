#include "PPU.h"
#include <iostream>
#include <SDL.h>
#include <algorithm>

bool PPU::oam_is_empty()
{
	return line_oam.empty();
}

void PPU::connect(LCD* l, PPUMemory* p, Bus* b)
{
	lcd = l;
	memory = p;
	bus = b;
}

u32 PPU::read_video_buffer(u16 address)
{
	return video_buffer[address];
}

void PPU::write_video_buffer(u16 address, u32 value)
{
	video_buffer[address] = value;
}

void PPU::tick(CPUContext* cpu)
{
	if (lcd->lcd_on()) {
		line_ticks++;
		switch (lcd->get_lcd_mode()) {
		case LM_OAM:
			mode_oam();
			break;
		case LM_XFER:
			mode_xfer(cpu);
			break;
		case LM_V_BLANK:
			mode_vblank(cpu);
			break;
		case LM_H_BLANK:
			mode_hblank(cpu);
			break;
		}
	}
}

void PPU::fetch_bg_tile() 
{
	bool lcdc_flag = lcd->get_bg_tile_map() == 0x9C00;

	u8 address_high_6 = 0b100110 | lcdc_flag;
	u16 address_low_10 = ((((lcd->ly + lcd->scroll_y) & 0xFF) / 8) << 5) | ((((fifo.fetch_x + lcd->scroll_x) & 0xFF) / 8));
	u16 address = (address_high_6 << 10) | address_low_10;

	u8 bg_fetch = bus->read(address);

	fifo.bgw_fetch_data[0] = bg_fetch;
}

void PPU::fetch_window_tile()
{
	if (fifo.fetch_x + 7 >= lcd->window_x &&
		fifo.fetch_x < lcd->window_x + X_RES + 7 &&
		lcd->ly >= lcd->window_y &&
		lcd->ly < lcd->window_y + Y_RES) {

		u8 window_tile_y = lcd->window_line / 8;
		u16 address = lcd->get_window_tile_map() + ((fifo.fetch_x + 7 - lcd->window_x) / 8) + (window_tile_y * 32);
		
		u8 data = bus->read(address);

		fifo.bgw_fetch_data[0] = data;
		window_fetched = true;
	}
}

void PPU::fetch_sprite_tile()
{
	for (auto& object : line_oam) {
		int sprite_x = (object.x - 8) + (lcd->scroll_x % 8);

		if ((sprite_x >= fifo.fetch_x && sprite_x < (fifo.fetch_x + 8)) ||
			((sprite_x + 8) >= fifo.fetch_x && (sprite_x + 8) < (fifo.fetch_x + 8))) {
			fetched_entries.push_back(object);
			sprite_fetched = true;

			if (fetched_entries.size() >= 3) {
				break;
			}
		}
	}
}

void PPU::fetch_sprite_data(bool offset) {

	int i = 0;
	for (auto& object : fetched_entries) {
		u8 tile_y = ((lcd->ly + 16) - object.y) * 2;

		if (object.y_flip) {
			tile_y = ((lcd->get_sprite_height() * 2) - 2) - tile_y;
		}

		u8 tile_index = object.tile;
		if (lcd->get_sprite_height() == 16) {
			tile_index &= ~(1);
		}

		u16 address2 = 0x8000 + (tile_index * 16) + tile_y + offset;

		fifo.fetch_entry_data[(i * 2) + offset] = bus->read(address2);

		i++;
	}
}

void PPU::pipeline_fetch()
{

	switch (fifo.current_fetch_state) {
	case FS_TILE: {
		fetched_entries.clear();
		window_fetched = false;

		if (lcd->get_bgw_enable()) {
			if (lcd->window_visible()) {
				fetch_window_tile();
			}
			if (!window_fetched) {
				fetch_bg_tile();
			}
		}

		if (lcd->get_obj_enable() && !oam_is_empty()) {
			fetch_sprite_tile();
		}

		fifo.current_fetch_state = FS_DATA_0;
		fifo.fetch_x += 8;
		break;
	}
	case FS_DATA_0: {
		// BG Fetch
		bool bit_12 = 0;
		if (lcd->get_bgw_tile_data_am() == 0x8800) {
			bit_12 = !(fifo.bgw_fetch_data[0] >> 7);
		}
		u8 bottom_4 = ((lcd->ly + lcd->scroll_y) % 8) << 1;

		if (window_fetched) {
			bottom_4 = (lcd->window_line % 8) << 1;
		}

		u16 address = (0b100 << 13) | (bit_12 << 12) | (fifo.bgw_fetch_data[0] << 4)
			| bottom_4;

		u8 bg_fetch = bus->read(address);

		fifo.bgw_fetch_data[1] = bg_fetch;

		fetch_sprite_data(0);		

		fifo.current_fetch_state = FS_DATA_1;
		break;
	}
	case FS_DATA_1: {

		bool bit_12 = 0;
		if (lcd->get_bgw_tile_data_am() == 0x8800) {
			bit_12 = !(fifo.bgw_fetch_data[0] >> 7);
		}
		u8 bottom_4 = (((lcd->ly + lcd->scroll_y) % 8) << 1) | 1;

		if (window_fetched) {
			bottom_4 = ((lcd->window_line % 8) << 1) | 1;
		}

		u16 address = (0b100 << 13) | (bit_12 << 12) | (fifo.bgw_fetch_data[0] << 4)
			| bottom_4;

		u8 fetch = bus->read(address);
		fifo.bgw_fetch_data[2] = fetch;

		fetch_sprite_data(1);

		fifo.current_fetch_state = FS_IDLE;
		break;
	}
	case FS_IDLE:
		fifo.current_fetch_state = FS_PUSH;
		break;
	case FS_PUSH:
		if (fifo.add(lcd, &fetched_entries)) {
			fifo.current_fetch_state = FS_TILE;
		}
		break;
	}
}

void PPU::pipeline_push_pixel()
{
	if (fifo.size() >= 8) {
		u32 pixel_data = fifo.pop();

		if (fifo.line_x >= (lcd->scroll_x % 8) || window_fetched) {
			u16 index = fifo.pushed_x + (lcd->ly * X_RES);
			write_video_buffer(index, pixel_data);
			fifo.pushed_x++;
		}
		fifo.line_x++;
	}
}

void PPU::pipeline_process()
{
	if (!(line_ticks & 0b1)) {
		pipeline_fetch();
	}
	pipeline_push_pixel();
}

bool oam_sort_by_x(OAMEntry entry_1, OAMEntry entry_2) {
	return entry_1.x < entry_2.x;
}

void PPU::mode_oam()
{
	if (line_ticks >= 80) {
		lcd->set_lcd_mode(LM_XFER);
		fifo.current_fetch_state = FS_TILE;
		fifo.line_x = 0;
		fifo.fetch_x = 0;
		fifo.pushed_x = 0;
		fifo.fifo_x = 0;
	}
	else if (line_ticks == 1) { // Fetch all on the first tick
		line_oam.clear();

		u8 height = lcd->get_sprite_height();
		int i = 0;
		for (auto& object : memory->oam_ram) {
			if ((object.x > 0) &&		// conflicting info, is x checked or not?
				((lcd->ly + 16) >= object.y) &&
				((lcd->ly + 16) < (object.y + height))) {

				line_oam.push_back(object);
				
			}
			if (line_oam.size() >= 10) {
				break;
			}
			i++;
		}
		std::sort(line_oam.begin(), line_oam.end(), oam_sort_by_x);

	}
}

void PPU::mode_xfer(CPUContext* cpu)
{
	pipeline_process();
	if (fifo.pushed_x >= X_RES) {
		fifo.reset();
		lcd->set_lcd_mode(LM_H_BLANK);
		if (lcd->get_interrupt_status(ISM_H_BLANK)) {
			cpu->ppu_interrupts.push_back(IT_LCD_STAT);
		}
	}
}

void PPU::mode_vblank(CPUContext* cpu)
{ 
	if (line_ticks >= TICKS_PER_LINE) {
		lcd->increment_ly(cpu);
		if (lcd->ly >= LINES_PER_FRAME) {
			lcd->set_lcd_mode(LM_OAM);
			lcd->ly = 0;
			lcd->ly_passed_window_y = false;
			lcd->window_line = 0;
		}
		line_ticks = 0;
	}
}

void PPU::mode_hblank(CPUContext* cpu)
{
	if (line_ticks >= TICKS_PER_LINE) {
		lcd->increment_ly(cpu);
		if (lcd->ly >= Y_RES) {
			lcd->set_lcd_mode(LM_V_BLANK);
			cpu->ppu_interrupts.push_back(IT_VBLANK);
			if (lcd->get_interrupt_status(ISM_V_BLANK)) {
				cpu->ppu_interrupts.push_back(IT_LCD_STAT);
			}
			current_frame++;

			// Calculate frame rate

			u32 end = SDL_GetTicks();
			u32 frame_time = end - prev_frame_time;

			// 60 FPS framerate cap
			if (frame_time < frame_time_target) {
				SDL_Delay((frame_time_target - frame_time));
			}

			if (end - start_timer >= 1000) {
				u32 fps = frame_count;
				start_timer = end;
				frame_count = 0;
				std::cout << "FPS: " << fps << "\n"; 
			}

			frame_count++;
			prev_frame_time = SDL_GetTicks();

		}
		else {
			lcd->set_lcd_mode(LM_OAM);
		}
		line_ticks = 0;

	}
}
