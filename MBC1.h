#pragma once
#include "MBC.h"
#include <string>

class MBC1 : public MBC
{
	bool ram_banking_enabled{};
	u8 rom_bank_number{};
	u8 ram_bank_number{};
	bool banking_mode{};
	bool has_battery{};
	bool battery_needs_save{};
	u8 total_ram_banks{};
	std::vector<MemoryBank> ram_banks{};
	std::string save_filename{};
	
	void init_ram_banking();
	void load_battery();

public:
	MBC1(u8 num_ram_banks, bool battery, std::string fname){
		total_ram_banks = num_ram_banks;
		has_battery = battery;
		save_filename = fname.substr(0, fname.find_last_of('.')) + ".sav";

		init_ram_banking();

		if (has_battery) {
			load_battery();
		}
	}

	u8 read(u16 address, std::vector<u8>& data) override;
	void write(u16 address, u8 value) override;
	void save_battery() override;
};

