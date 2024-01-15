#include "MBC1.h"
#include <fstream>
#include <iostream>

void MBC1::init_ram_banking()
{
	for (int i = 0; i < total_ram_banks; i++) {
		ram_banks.push_back(MemoryBank());
	}
}

void MBC1::load_battery()
{
	std::ifstream file(save_filename, std::ios::binary);

	if (!file.is_open()) {  // No saved data
		std::cerr << "Failed to read save file.\n";
		return;
	}

	int memory_bank_size = 0x4000;

	file.read(reinterpret_cast<char*>(ram_banks[ram_bank_number].data), memory_bank_size);

	file.close();

}

void MBC1::save_battery()
{
	if (battery_needs_save) {
		std::ofstream file(save_filename, std::ios::binary);

		if (!file.is_open()) {
			std::cerr << "Failed to save file.\n";
			exit(-1);
		}

		int memory_bank_size = 0x4000;

		file.write(reinterpret_cast<char*>(ram_banks[ram_bank_number].data), memory_bank_size);

		file.close();
	}
}

u8 MBC1::read(u16 address, std::vector<u8>& data)
{

	if (address < 0x4000) {
		return data[address];
	}

	else if (address < 0x8000) {
		if (rom_bank_number == 0) {
			return data[address];
		}
		int new_address = (0x4000 * rom_bank_number) + (address - 0x4000);
		return data[new_address];
	}

	else if (0xA000 <= address <= 0xBFFF) {
		if (ram_banking_enabled) {
			return ram_banks[ram_bank_number].data[address - 0xA000];
		}
		return 0xFF;
	}
}

void MBC1::write(u16 address, u8 value)
{
	if (address < 0x2000) {
		ram_banking_enabled = (value & 0xF) == 0xA;
	}
	else if (address < 0x4000) {
		rom_bank_number = value & 0b11111;
	}
	else if (address < 0x6000) {
		ram_bank_number = value & 0b11;
	}
	else if (address < 0x8000) {
		banking_mode = value & 1;
	}
	else if (0xA000 <= address <= 0xBFFF) {
		if (ram_banking_enabled) {
			ram_banks[ram_bank_number].data[address - 0xA000] = value;
			if (has_battery) {
				battery_needs_save = true;
			}
		}
	}
}
