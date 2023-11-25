#pragma once
#include "common.h"

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
	int size{};
	int line_x{};
	int pushed_x{};
	int fetch_x{};
	u8 bgw_fetch_data[3]{};
	u8 fetch_entry_data[6]{};
	int map_y = 0;
	int map_x = 0;
	int tile_y = 0;
	int fifo_x = 0;
public:
	FetchState current_fetch_state = FS_TILE;
	void push(u32 value);
	FIFOEntry pop();
	void reset();
	bool add(int scroll_x, u32 colors[4]);

};

