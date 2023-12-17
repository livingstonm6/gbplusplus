#pragma once
#include "Common.h"
#include "LCD.h"
#include "FIFO.h"
#include "CPUContext.h"
#include <vector>
#include "OAMEntry.h"


class PPU
{
	int LINES_PER_FRAME = 154;
	const int TICKS_PER_LINE = 456;
	
	const double frame_time_target = (double)1000 / 60;
	long prev_frame_time{};
	long start_timer{};
	long frame_count{};
	LCD* lcd = nullptr;
	
	u8 vram[0x2000];

	// todo remove magic numbers
	u32 video_buffer[144 * 160];


public:
	std::vector<OAMEntry> line_oam;
	std::vector<OAMEntry> oam_ram;
	std::vector<OAMEntry> fetched_entries;
	const int Y_RES = 144;
	const int X_RES = 160;


	PPU() {
		OAMEntry blank_entry;

		for (int i = 0; i < 0xA0; i++) {
			oam_ram.push_back(blank_entry);
		}

		for (int i = 0; i < 144 * 160; i++) {
			video_buffer[i] = 0;
		}
		for (int i = 0; i < 0xA0; i++) {
			vram[i] = 0;
		}

	}

	long current_frame{};
	FIFO fifo;
	int line_ticks{};
	u8 oam_read(u16 address);
	u8 vram_read(u16 address);

	void oam_write(u16 address, u8 value);
	void vram_write(u16 address, u8 value);
	bool oam_is_empty();

	void connect(LCD* l);

	u32 read_video_buffer(u16 address);
	void write_video_buffer(u16 address, u32 value);

	void tick(CPUContext* cpu, u8 fetch = 0);

	void pipeline_fetch(u8 fetch);
	void pipeline_push_pixel();
	void pipeline_process(u8);

	void mode_oam();
	void mode_xfer(CPUContext*, u8 fetch = 0);
	void mode_vblank(CPUContext*);
	void mode_hblank(CPUContext*);
};

