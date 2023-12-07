#include "PPU.h"
#include <iostream>
#include <SDL.h>
u8 PPU::oam_read(u16 address)
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
	}
	;
}

u8 PPU::vram_read(u16 address)
{
	return vram[address - 0x8000];
}

void PPU::oam_write(u16 address, u32 value)
{
	if (address >= 0xFE00) {
		address -= 0xFE00;
	}
	u16 oam_index = address / 4;
	u8 byte_index = address % 4;
	oam_ram[address].update(value, byte_index);
}

void PPU::vram_write(u16 address, u8 value)
{
	vram[address - 0x8000] = value;
}

void PPU::connect(LCD* l)
{
	lcd = l;
}

u32 PPU::read_video_buffer(u16 address)
{
	return video_buffer[address];
}

void PPU::write_video_buffer(u16 address, u32 value)
{
	video_buffer[address] = value;
}

void PPU::tick(CPUContext* cpu, u8 fetch)
{
	if (lcd->lcd_on()) {
		line_ticks++;
		switch (lcd->get_lcd_mode()) {
		case LM_OAM:
			mode_oam();
			break;
		case LM_XFER:
			mode_xfer(cpu, fetch);
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

void PPU::pipeline_fetch(u8 fetch)
{
	switch (fifo.current_fetch_state) {
	case FS_TILE:
		if (lcd->get_bgw_enable()) {
			fifo.bgw_fetch_data[0] = fetch;
			if (lcd->get_bgw_tile_data_am() == 0x8800) {
				fifo.bgw_fetch_data[0] = fetch;
			}
		

		}
		fifo.current_fetch_state = FS_DATA_0;
		fifo.fetch_x += 8;
		break;
	case FS_DATA_0:
		fifo.bgw_fetch_data[1] = fetch;
		fifo.current_fetch_state = FS_DATA_1;
		break;
	case FS_DATA_1:
		fifo.bgw_fetch_data[2] = fetch;
		fifo.current_fetch_state = FS_IDLE;
		break;
	case FS_IDLE:
		fifo.current_fetch_state = FS_PUSH;
		break;
	case FS_PUSH:
		if (fifo.add(lcd->scroll_x, lcd->bg_colors)) {
			fifo.current_fetch_state = FS_TILE;
		}
		break;
	}
}

void PPU::pipeline_push_pixel()
{
	if (fifo.size >= 8) {
		u32 pixel_data = fifo.pop();

		if (fifo.line_x >= (lcd->scroll_x % 8)) {
			u16 index = fifo.pushed_x + (lcd->ly * X_RES);
			write_video_buffer(index, pixel_data);
			//std::cout << "WRITING DATA TO BUFFER: 0x" << std::uppercase << std::hex << pixel_data << "\n";
			fifo.pushed_x++;
		}
		fifo.line_x++;
	}
}

void PPU::pipeline_process(u8 fetch)
{
	if (!(line_ticks & 0b1)) {
		pipeline_fetch(fetch);
	}
	pipeline_push_pixel();
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
}

void PPU::mode_xfer(CPUContext* cpu, u8 fetch)
{
	pipeline_process(fetch);
	if (fifo.pushed_x >= X_RES) {
		fifo.reset();
		lcd->set_lcd_mode(LM_H_BLANK);
		if (lcd->get_interrupt_status(ISM_H_BLANK)) {
			cpu->ppu_interrupt_queue.push_back(IT_LCD_STAT);
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
			cpu->ppu_interrupt_queue.push_back(IT_VBLANK);
			if (lcd->get_interrupt_status(ISM_V_BLANK)) {
				cpu->ppu_interrupt_queue.push_back(IT_LCD_STAT);
			}
			current_frame++;

			// TODO calculate frame rate here

			u32 end = SDL_GetTicks();
			u32 frame_time = end - prev_frame_time;

			if (frame_time < frame_time_target) {
				SDL_Delay((frame_time_target - frame_time));
			}

			if (end - start_timer >= 1000) {
				u32 fps = frame_count;
				start_timer = end;
				frame_count = 0;
				//std::cout << "FPS: " << fps << "\n"; 
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
