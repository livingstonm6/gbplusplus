#pragma once
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned long u32;
typedef signed char s8;
typedef signed short s16;
typedef signed long s32;

struct PPUInterrupts {
	bool xfer;
	bool vblank;
	bool hblank_1;
	bool hblank_2;
	bool hblank_3;
};

enum LCDMode {
	LM_H_BLANK = 0,
	LM_V_BLANK = 1,
	LM_OAM = 2,
	LM_XFER = 3
};

enum InterruptSourceMode {
	ISM_H_BLANK = (1 << 3),
	ISM_V_BLANK = (1 << 4),
	ISM_OAM = (1 << 5),
	ISM_LYC = (1 << 6)
};

enum InterruptType {
	IT_VBLANK = 1,
	IT_LCD_STAT = 2,
	IT_TIMER = 4,
	IT_SERIAL = 8,
	IT_JOYPAD = 16
};
