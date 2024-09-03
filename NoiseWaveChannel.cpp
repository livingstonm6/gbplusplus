#include "NoiseWaveChannel.h"
#include <iostream>

void NoiseWaveChannel::trigger()
{
	enabled = true;
	current_volume = volume_envelope >> 4;
	period_counter = get_period_value();
	envelope_counter = 0;
	lfsr = 0;

}

void NoiseWaveChannel::length_enable()
{
	length_timer_enabled = true;
	length_timer = timer & 0b111111;
}

int NoiseWaveChannel::get_period_value()
{
	u8 div = frequency_randomness & 0b111;
	u8 shift = frequency_randomness >> 4;
	if (div == 0) {
		u8 result = 1;
		for (int i = 0; i < shift; i++) {
			result *= 2;
		}
		return result / 2;
		
	}
	
	return div << shift;
}

void NoiseWaveChannel::tick_lfsr()
{
	u8 bit0 = lfsr & 1;
	u8 bit1 = (lfsr >> 1) & 1;
	int new_bit15 = (int)(bit1 == bit0);
	bool short_mode = (frequency_randomness >> 3) & 1;

	if (new_bit15 == 1) {
		lfsr |= (1 << 15);
		if (short_mode) {
			lfsr |= (1 << 7);
		}
	}
	else {
		lfsr &= ~(1 << 15);
		if (short_mode) {
			lfsr &= ~(1 << 7);
		}
	}
	lfsr >>= 1;


}

u8 NoiseWaveChannel::read(u16 address)
{
	if (address == 0xFF20) {
		return timer;
	}
	else if (address == 0xFF21) {
		return volume_envelope;
	}
	else if (address == 0xFF22) {
		return frequency_randomness;
	}
	else if (address == 0xFF23) {
		return control;
	}
}

void NoiseWaveChannel::write(u16 address, u8 value)
{
	if (address == 0xFF20) {
		timer = value;
	}
	else if (address == 0xFF21) {
		volume_envelope = value;
		if ((value >> 3) == 0b11111) {
			enabled = false;
		}
	}
	else if (address == 0xFF22) {
		frequency_randomness = value;
	}
	else if (address == 0xFF23) {
		control = value;
		if (value >> 7) {
			trigger();
		}
		if ((value >> 6) & 1) {
			length_enable();
		}
	}
}

float NoiseWaveChannel::get_sample()
{
	if (!enabled) {
		return 0.0f;
	}

	if (lfsr & 1) {
		return (float)current_volume / 1000.0f;
	}

	return 0;

}

void NoiseWaveChannel::tick()
{
	if (!enabled) {
		return;
	}

	tick_counter++;
	if (tick_counter < 4) {
		return;
	}
	tick_counter = 0;

	if (period_counter == 0) {
		tick_lfsr();
		period_counter = get_period_value();
	}
	else {
		period_counter--;
	}


}

void NoiseWaveChannel::tick_envelope()
{
	int pace = volume_envelope & 0b111;

	if (!enabled || pace == 0) {
		return;
	}

	envelope_counter++;

	if (envelope_counter % pace != 0) {
		return;
	}

	int increasing = (volume_envelope >> 3) & 1;

	if (increasing) {
		if (current_volume < 0b1111) {
			current_volume += 1;
		}
	}
	else if (current_volume > 0) {
		current_volume -= 1;
	}
}

void NoiseWaveChannel::tick_length()
{
	if (!length_timer_enabled || !enabled) {
		return;
	}

	if (length_timer == 0b111111) {
		length_timer_enabled = false;
		enabled = false;
		return;
	}

	length_timer++;
}

bool NoiseWaveChannel::is_enabled()
{
	return enabled;
}
