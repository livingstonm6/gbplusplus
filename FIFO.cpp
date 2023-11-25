#include "FIFO.h"
#include <iostream>

void FIFO::push(u32 value)
{
	struct FIFOEntry next_entry;
	next_entry.next = nullptr;
	next_entry.value = value;
	if (head == nullptr) {
		head = &next_entry;
		tail = &next_entry;
	}
	else {
		tail->next = &next_entry;
		tail = &next_entry;
	}
	size++;
}

FIFOEntry FIFO::pop()
{
	if (size < 1) {
		std::cout << "ERROR, pop at empty pixel fifo\n";
		exit(-1);
	}
	FIFOEntry* popped = head;
	head = head->next;
	size--;
	return *popped;
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
	if (size > 8) {
		return false;
	}
	int x = fetch_x - (8 - (scroll_x % 8));
	for (u8 i = 0; i < 8; i++) {
		u8 bit = 7 - i;
		u8 low = !!(bgw_fetch_data[1] & (1 << bit));
		u8 high = !!(bgw_fetch_data[2] & (1 << bit)) << 1;
		u32 color = colors[high | low];
		if (x >= 0) {
			push(color);
			fifo_x++;
		}
	}
	return true;
}

