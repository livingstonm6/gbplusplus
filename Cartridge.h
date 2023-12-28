#pragma once
#include <string>
#include <vector>
#include "Common.h"
#include <memory>
#include "MBC.h"

class ROMHeader
{
	// TODO
};

class Cartridge
{
	std::unique_ptr<MBC> mbc = nullptr;

public:
	std::string filename;
	std::vector<unsigned char> data;
	u8 cartridge_type{};
	ROMHeader header;

	Cartridge() {}
	void load_rom(std::string fn);

	u8 read(u16 address);
	void write(u16 address, u8 value);
};

