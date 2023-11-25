#include "Cartridge.h"
#include <fstream>
#include <iostream>
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

}

u8 Cartridge::read(u16 address) {
	//std::cout << "Cart read address: 0x" << std::hex << address << "\n";
	return data[address];
}

void Cartridge::write(u16 address, u8 value)
{
	return;
}
