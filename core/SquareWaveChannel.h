#pragma once
#include "utility/Common.h"
#include <vector>
#include <queue>

class SquareWaveChannel
{

	int duty_cycle_position = 0;
	bool enabled = false;
	bool length_timer_enabled = false;
	int length_timer = 0;
	int tick_counter = 0;
	int envelope_counter = 0;

	std::vector<int> duty_cycle1 = { 1, 1, 1, 1, 1, 1, 1, 0 };
	std::vector<int> duty_cycle2 = { 0, 1, 1, 1, 1, 1, 1, 0 };
	std::vector<int> duty_cycle3 = { 0, 1, 1, 1, 1, 0, 0, 0 };
	std::vector<int> duty_cycle4 = { 1, 0, 0, 0, 0, 0, 0, 1 };

	std::vector<std::vector<int>> duty_cycles = { duty_cycle1, duty_cycle2, duty_cycle3, duty_cycle4 };


	// Registers
	u8 sweep;  // ch1 only
	u8 timer;
	u8 volume_envelope;
	u8 period_low;
	u8 period_high_control;

	int current_volume = 0;
	int period_counter = 0;

	void trigger();
	void length_enable();
	int get_period_value();

	
public:
	AudioChannelType channel_type;
	void tick();
	void tick_length();
	void tick_envelope();
	void tick_sweep();
	void write(u16 address, u8 value);
	u8 read(u16 address);
	float get_sample();
	bool is_enabled();

	std::vector<float> sample_buffer;


};