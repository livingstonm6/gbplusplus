#pragma once
#include "core\utility\Common.h"
class NoiseWaveChannel
{
private:
	u8 timer{};
	u8 volume_envelope{};
	u8 frequency_randomness{};
	u8 control{};

	u16 lfsr{};

	bool enabled = false;
	bool length_timer_enabled = false;
	int length_timer = 0;
	int tick_counter = 0;
	int envelope_counter = 0;
	int current_volume = 0;
	int period_counter = 0;

	void trigger();
	void length_enable();
	int get_period_value();
	void tick_lfsr();
public:
	u8 read(u16 address);
	void write(u16 address, u8 value);
	float get_sample();
	void tick();
	void tick_envelope();
	void tick_length();
	bool is_enabled();
};

