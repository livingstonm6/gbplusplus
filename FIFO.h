#pragma once
#include "common.h"
#include "LCD.h"
#include "OAMEntry.h"

enum FetchState {
	FS_TILE = 1,
	FS_DATA_0 = 2,
	FS_DATA_1 = 3,
	FS_PUSH = 4,
	FS_IDLE = 5
};

struct FIFOEntry
{
	struct FIFOEntry *next;
	u32 value;
};


class FIFO
{
	struct FIFOEntry* head;
	struct FIFOEntry* tail;
	
	
	
	
public:
	int map_y = 0;
	int map_x = 0;
	int tile_y = 0;
	int fifo_x = 0;
	int pushed_x = 0;
	int line_x = 0;
	int size = 0;
	int fetch_x = 0;
	u8 bgw_fetch_data[3]{};
	u8 fetch_entry_data[6]{};
	FetchState current_fetch_state = FS_TILE;
	void push(u32 value);
	u32 pop();
	void reset();
	u32 fetch_sprite_pixels(u8 bit, u32 color, u8 bg_color, LCD* lcd, std::vector<OAMEntry>* oam_ram);
	bool add(LCD*, std::vector<OAMEntry>*);

};

