#pragma once
#include "Common.h"
#include "LCD.h"
#include "FIFO.h"



class OAMEntry {
public:
	u8 y{};
	u8 x{};
	u8 tile{};
	int flags{};
	u8 cgb_palette{};
	bool cgb_vram_bank{};
	bool pn{};
	bool x_flip{};
	bool y_flip{};
	bool bgp{};

	void update(u32 value);

};


class PPU
{
	int LINES_PER_FRAME = 154;
	const int TICKS_PER_LINE = 456;
	const int Y_RES = 144;
	const int X_RES = 160;
	const double frame_time_target = 1000 / 60;
	LCD* lcd = nullptr;
	FIFO fifo;

	OAMEntry oam_ram[0xA0];
	u8 vram[0x2000];

	// todo remove magic numbers
	u8 video_buffer[144 * 160 * 4];

public:
	OAMEntry oam_read(u16 address);
	u8 vram_read(u16 address);

	void oam_write(u16 address, u32 value);
	void vram_write(u16 address, u8 value);

	void connect(LCD* l);

	u32 read_video_buffer(u16 address);
	void write_video_buffer(u16 address, u32 value);

	PPUInterrupts tick();

	void pipeline_fetch(u8 fetch);
	void pipeline_push_pixel();
	void pipeline_process();

	void mode_oam();
	void mode_xfer();
	void mode_vblank();
	void mode_hblank();
};

