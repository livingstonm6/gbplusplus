#include "PPU.h"

OAMEntry PPU::oam_read(u16 address)
{
	if (address >= 0xFE00) {
		address -= 0xFE00;
	}
	return oam_ram[address];
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
	oam_ram[address].update(value);
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
	u8 byte_1 = video_buffer[address];
	u8 byte_2 = video_buffer[address + 1];
	u8 byte_3 = video_buffer[address + 2];
	u8 byte_4 = video_buffer[address + 3];
	return (byte_1 << 24) | (byte_2 << 16) | (byte_3 << 8) | byte_1;
}

void PPU::write_video_buffer(u16 address, u32 value)
{
	video_buffer[address] = (value & 0xFF000000) >> 24;
	video_buffer[address + 1] = (value & 0x00FF0000) >> 16;
	video_buffer[address + 2] = (value & 0x0000FF00) >> 8;
	video_buffer[address + 3] = value & 0x000000FF;
}

PPUInterrupts PPU::tick()
{
	return PPUInterrupts();
}

void PPU::pipeline_fetch(u8 fetch)
{
	switch (fifo.current_fetch_state) {
	case FS_TILE:
		break;
	case FS_DATA_0:
		break;
	case FS_DATA_1:
		break;
	case FS_IDLE:
		break;
	case FS_PUSH:
		break;
	}
}

void PPU::pipeline_push_pixel()
{
}

void PPU::pipeline_process()
{
}

void PPU::mode_oam()
{
}

void PPU::mode_xfer()
{
}

void PPU::mode_vblank()
{
}

void PPU::mode_hblank()
{
}

void OAMEntry::update(u32 value)
{
	y = value >> 24;
	x = value >> 16;
	tile = value >> 8;
	cgb_palette = value & 0b111;
	cgb_vram_bank = value & (1 << 3);
	pn = value & (1 << 4);
	x_flip = value & (1 << 5);
	y_flip = value & (1 << 6);
	bgp = value & (1 << 7);
}
