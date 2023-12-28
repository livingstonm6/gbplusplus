#pragma once
#include "Common.h"
#include <vector>

class MemoryBank
{
public:
	u8 data[0x4000];

	MemoryBank() {
		for (u16 i = 0; i < 0x4000; i++) {
			data[i] = 0;
		}
	}
};

class MBC
{
public:
	virtual u8 read(u16 address, std::vector<u8>& data) = 0;
	virtual void write(u16 address, u8 value) = 0;
};
