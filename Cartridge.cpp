#include "Cartridge.h"
#include <fstream>
#include <iostream>
#include "MBC1.h"
#include "MBC3.h"

void Cartridge::load_rom(std::string fname)
{
	filename = fname;

	std::ifstream file(fname, std::ios::binary);

	if (!file.is_open()) {
		std::cerr << "Error opening file: " << fname << "\n";
		exit(-1);
	}

	file.seekg(0, std::ios::end);
	std::streampos fileSize = file.tellg();
	file.seekg(0, std::ios::beg);

	data.resize(static_cast<std::size_t>(fileSize));

	file.read(reinterpret_cast<char*>(data.data()), fileSize);

	file.close();

	cartridge_type = data[0x147];

	u8 total_ram_banks = 0;

	switch (data[0x149]) {
	case 2:
		total_ram_banks = 1;
	case 3:
		total_ram_banks = 4;
	case 4:
		total_ram_banks = 16;
	case 5:
		total_ram_banks = 8;
	}


	switch (cartridge_type) {
	case CT_MBC1:
		mbc = std::make_unique<MBC1>(total_ram_banks, false, filename);
		break;
	case CT_MBC1_RAM:
		mbc = std::make_unique<MBC1>(total_ram_banks, false, filename);
		break;
	case CT_MBC1_RAM_BAT:
		mbc = std::make_unique<MBC1>(total_ram_banks, true, filename);
		break;
	case CT_MBC3:
		mbc = std::make_unique<MBC3>(total_ram_banks, false, filename);
		break;
	case CT_MBC3_RAM:
		mbc = std::make_unique<MBC3>(total_ram_banks, false, filename);
		break;
	case CT_MBC3_RAM_BAT:
		mbc = std::make_unique<MBC3>(total_ram_banks, true, filename);
		break;
	case CT_MBC3_TIMER_BAT:
		mbc = std::make_unique<MBC3>(total_ram_banks, true, filename);
		break;
	case CT_MBC3_TIMER_RAM_BAT:
		mbc = std::make_unique<MBC3>(total_ram_banks, true, filename);
		break;
	}

}

u8 Cartridge::read(u16 address) {
	if (mbc == nullptr) {
		return data[address];
	}

	return mbc->read(address, data);
	
}

void Cartridge::write(u16 address, u8 value)
{
	if (mbc != nullptr) {
		mbc->write(address, value);
	}
	
}

void Cartridge::save_battery()
{
	if (mbc != nullptr) {
		mbc->save_battery();
	}
}
