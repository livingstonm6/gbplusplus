#pragma once
#include "utility/Common.h"
#include "LCD.h"
#include "FIFO.h"
#include "CPUContext.h"
#include <vector>
#include "OAMEntry.h"
#include "PPUMemory.h"
#include "Bus.h"


class PPU
{
	int LINES_PER_FRAME = 154;
	int TICKS_PER_LINE = 456;

	//double frame_time_target = (double)1000 / 60;
	double frame_time_target = 0;
	long prev_frame_time{};
	long start_timer{};
	long frame_count{};
	LCD* lcd = nullptr;
	PPUMemory* memory = nullptr;
	Bus* bus = nullptr;


	// todo remove magic numbers
	u32 video_buffer[144 * 160]{};

	void fetch_bg_tile();
	void fetch_window_tile();
	void fetch_sprite_tile();
	void fetch_sprite_data(bool offset);


public:
	std::vector<OAMEntry> line_oam;
	std::vector<OAMEntry> fetched_entries;
	int Y_RES = 144;
	int X_RES = 160;
	bool window_fetched = false;
	bool sprite_fetched = false;

	long current_frame{};
	FIFO fifo;
	int line_ticks{};

	bool oam_is_empty();

	void connect(LCD* l, PPUMemory* p, Bus* b);

	u32 read_video_buffer(u16 address);
	void write_video_buffer(u16 address, u32 value);

	void tick(CPUContext* cpu);

	void pipeline_fetch();
	void pipeline_push_pixel();
	void pipeline_process();

	void mode_oam();
	void mode_xfer(CPUContext*);
	void mode_vblank(CPUContext*);
	void mode_hblank(CPUContext*);
	void set_framerate_cap(int framerate);
};

