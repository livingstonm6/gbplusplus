#include "CustomWaveChannel.h"

u8 CustomWaveChannel::get_wave_pattern_value()
{
	if (wave_pattern_index % 2 == 0) { // upper nibble then lower
		int index = wave_pattern_index / 2;
		return wave_pattern_ram[index] >> 4;
	}

	int index = (wave_pattern_index - 1) / 2;
	return wave_pattern_ram[index] & 0xF;
	
}

void CustomWaveChannel::length_enable()
{
	length_timer_enabled = true;
	length_timer = timer;
}

int CustomWaveChannel::get_period_value()
{
	return period_low | ((period_high_control & 0b111) << 8);
}

u8 CustomWaveChannel::read(u16 address)
{
	if (address == 0xFF1A) {
		return dac_enable;
	}
	else if (address == 0xFF1B) {
		return timer;
	}
	else if (address == 0xFF1C) {
		return output_level;
	}
	else if (address == 0xFF1D) {
		return period_low;
	}
	else if (address == 0xFF1E) {
		return period_high_control;
	}
	else if (0xFF30 <= address <= 0xFF3F) {
		return wave_pattern_ram[address & 0xF];
	}
}

void CustomWaveChannel::write(u16 address, u8 value)
{
	if (address == 0xFF1A) {
		dac_enable = value;
		if (dac_enable >> 7) {
			trigger();
		}
		else {
			enabled = false;
		}
	}
	else if (address == 0xFF1B) {
		timer = value;
	}
	else if (address == 0xFF1C) {
		output_level = value;
	}
	else if (address == 0xFF1D) {
		period_low = value;
	}
	else if (address == 0xFF1E) {
		period_high_control = value;
		if (period_high_control >> 7) {
			trigger();
		}
		if ((period_high_control >> 6) & 1) {
			length_enable();
		}
	}
	else if (0xFF30 <= address <= 0xFF3F) {
		wave_pattern_ram[address & 0xF] = value;
	}
}

float CustomWaveChannel::get_sample()
{
	int output_value = (output_level >> 5) & 0b11;

	if (!enabled || output_value == 0) {
		return 0.0f;
	}

	int sample_position = get_wave_pattern_value();
	int shift = 0;
	if (output_value == 0b10) {
		shift = 1;
	}
	else if (output_value == 0b11) {
		shift = 2;
	}

	return (float)(sample_position >> shift) / 1000.0f;

}

void CustomWaveChannel::tick()
{
	if (!enabled) {
		return;
	}

	tick_counter++;
	if (tick_counter < 4) {
		return;
	}

	tick_counter = 0;

	if (period_counter == 0x7FF) {
		wave_pattern_index++;
		if (wave_pattern_index > 31) {
			wave_pattern_index = 0;
		}

		period_counter = get_period_value();
	}
	else {
		period_counter++;
	}

}

void CustomWaveChannel::tick_length()
{
	if (!length_timer_enabled || !enabled) {
		return;
	}

	if (length_timer == 0xFF) {
		length_timer_enabled = false;
		enabled = false;
		return;
	}
	length_timer++;

}

bool CustomWaveChannel::is_enabled()
{
	return enabled;
}

void CustomWaveChannel::trigger()
{
	if (dac_enable >> 7 == 0) {
		return;
	}

	enabled = true;
	period_counter = get_period_value();
	wave_pattern_index = 1;
}
