#include "FIFO.h"
#include <iostream>

void FIFO::push(u32 value)
{
	struct FIFOEntry* next_entry = new FIFOEntry{nullptr, value};
	if (head == nullptr) {
		head = next_entry;
		tail = next_entry;
	}
	else {
		tail->next = next_entry;
		tail = next_entry;
	}
	size++;
}

u32 FIFO::pop()
{
	if (size < 1) {
		std::cout << "ERROR, pop at empty pixel fifo\n";
		exit(-1);
	}
	u32 value = head->value;
	
	if (size == 1) {
		delete head;
		head = tail = nullptr;
	}
	else {
		FIFOEntry* next_head = head->next;
		delete head;
		head = next_head;
	}

	size--;
	return value;
}

void FIFO::reset()
{
	while (size > 0) {
		pop();
	}
	head = nullptr;
	tail = nullptr;
}

bool FIFO::add(int scroll_x, u32 colors[4])
{
	//for (int i = 0; i < 4; i++) {
	//	std::cout << "color[" << i << "]: 0x" << std::uppercase << std::hex << std::uppercase << colors[i] << "\n";
	//}
	if (size > 8) { //todo 16?
		return false;
	}
	int x = fetch_x - (8 - (scroll_x % 8));
	for (u8 i = 0; i < 8; i++) {
		u8 bit = 7 - i;
		u8 low = !!(bgw_fetch_data[1] & (1 << bit));
		//std::cout << "bgw_fetch_data[1]:" << std::hex << std::uppercase << bgw_fetch_data[1] << "\n";
		u8 high = !!(bgw_fetch_data[2] & (1 << bit)) << 1;
		//std::cout << "bgw_fetch_data[2]:" << std::hex << std::uppercase << bgw_fetch_data[2] << "\n";
		//std::cout << "high | low: " << (high | low) << "\n";
		u32 color = colors[high | low];
		//u32 color = colors[3];
		u32 test_1 = colors[0];
		u32 test_2 = colors[1];
		u32 test_3 = colors[2];
		u32 test_4 = colors[3];
		//std::cout << "color:" << std::hex << std::uppercase << color << "\n";
		if (x >= 0) {
			push(color);
			fifo_x++;
		}
	}
	return true;
}

