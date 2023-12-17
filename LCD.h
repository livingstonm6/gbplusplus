#pragma once
#include "common.h"
#include "CPUContext.h"

class LCD
{
	u8 lcd_control = 0x91;
	u8 lcd_status = 2;
	
	
	u8 ly_compare{};
	u8 dma{};
	u8 bgp_palette = 0xFC;
	u8 obj_palette[2];
	u8 window_y{};
	u8 window_x{};
	


public:
	u32 colors_default[4];
	u32 sp1_colors[4];
	u32 sp2_colors[4];
	u32 bg_colors[4];
	u8 ly = 0;
	u8 scroll_x{};
	u8 scroll_y{};

	u8 window_line{};


	LCD() {
		obj_palette[0] = 0xFF;
		obj_palette[1] = 0xFF;

		colors_default[0] = 0xFFFFFFFF;
		colors_default[1] = 0xFFAAAAAA;
		colors_default[2] = 0xFF555555;
		colors_default[3] = 0xFF000000;

		for (int i = 0; i < 4; i++) {
			bg_colors[i] = colors_default[i];
			sp1_colors[i] = colors_default[i];
			sp2_colors[i] = colors_default[i];
		}

	}
	
	

	u8 read(u16 address);
	void write(u16 address, u8 value);

	bool get_bgw_enable();
	bool get_obj_enable();
	u16 get_bg_tile_map();
	u16 get_bgw_tile_data_am();
	u16 get_window_tile_map();
	void set_lcd_mode(LCDMode);
	LCDMode get_lcd_mode();
	void set_lyc_flag();
	void reset_lyc_flag();
	bool get_interrupt_status(InterruptSourceMode);
	u8 get_sprite_height();
	bool window_visible();

	void update_palette(u8 value, u8 palette_number);
	void increment_ly(CPUContext*);

	bool lcd_on();


};

