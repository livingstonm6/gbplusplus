#pragma once
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned long u32;
typedef signed char s8;
typedef signed short s16;
typedef signed long s32;

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
	IT_VBLANK = 0b1,
	IT_LCD_STAT = 0b10,
	IT_TIMER = 0b100,
	IT_SERIAL = 0b1000,
	IT_JOYPAD = 0b10000
};

enum CartridgeType {
	CT_ROM_ONLY = 0x00,
	CT_MBC1 = 0x01,
	CT_MBC1_RAM = 0x02,
	CT_MBC1_RAM_BAT = 0x03,
	CT_MBC2 = 0x05,
	CT_MBC2_BAT = 0x06,
};