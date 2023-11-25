#include "Motherboard.h"
#include <fstream>
#include <iostream>

void Motherboard::init()
{
	cartridge.load_rom(filename);
	bus.connect(&ppu, &lcd, &cartridge, &timer);
	cpu.connect(&bus, &timer, &ppu);
}


void Motherboard::run()
{
	std::cout << "MOBO RUNNING" << "\n";

	

	//for (const auto& byte : cartridge.data) {
	//	std::cout << byte << "\n";
	//}


	//for (unsigned char byte : cartridge.data) {
	//	if (byte != 0) {
	//		std::cout << static_cast<int>(byte) << "\n";
	//	}
	//}

	//std::cout << "Finished!" << "\n";
	//std::cout << "Size: "  << cartridge.data.size() << "\n";
	//std::cout << "Byte 0x100:" << cartridge.data[0x100] << "\n";
	//std::cout << (0 == cartridge.data[0x100]) << "\n";

	running = true;

	while (running) {

		cpu.step();


	}



}
