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
	CT_ROM_RAM = 0x08,
	CT_ROM_RAM_BAT = 0x09,
	CT_MBC3_TIMER_BAT = 0x0F,
	CT_MBC3_TIMER_RAM_BAT = 0x10,
	CT_MBC3 = 0x11,
	CT_MBC3_RAM = 0x12,
	CT_MBC3_RAM_BAT = 0x13
};

enum JoypadInputType {
	JIT_START = 1,
	JIT_SELECT = 2,
	JIT_B = 3,
	JIT_A = 4,
	JIT_DOWN = 5,
	JIT_UP = 6,
	JIT_LEFT = 7,
	JIT_RIGHT = 8
};

enum AudioChannelType {
	ACT_1 = 1,
	ACT_2 = 2,
	ACT_3 = 3,
	ACT_4 = 4
};