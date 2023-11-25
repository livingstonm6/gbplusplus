#pragma once
#include <string>
#include <vector>
#include "Common.h"

class ROMHeader
{
	// TODO
};

class Cartridge
{
public:
	std::string filename;
	std::vector<unsigned char> data;


	Cartridge() {}
	void load_rom(std::string fn);

	u8 read(u16 address);
	void write(u16 address, u8 value);
};

