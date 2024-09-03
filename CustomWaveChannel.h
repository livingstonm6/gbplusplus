#pragma once
#include "core\utility\Common.h"
class CustomWaveChannel
{
private:
	bool enabled = false;
	bool length_timer_enabled = false;
	int wave_pattern_index = 1;
	int length_timer = 0;
	int period_counter = 0;
	int tick_counter = 0;

	u8 dac_enable{};
	u8 timer{};
	u8 output_level{};
	u8 period_low{};
	u8 period_high_control{};
	u8 wave_pattern_ram[16]{};

	u8 get_wave_pattern_value();
	void length_enable();
	int get_period_value();
public:
	u8 read(u16 address);
	void write(u16 address, u8 value);
	float get_sample();
	void tick();
	void tick_length();
	bool is_enabled();
	void trigger();

};

