#include "SquareWaveChannel.h"
#include <iostream>

void SquareWaveChannel::trigger()
{
	enabled = true;
	current_volume = volume_envelope >> 4;
	period_counter = get_period_value();
	duty_cycle_position = 0;
	envelope_counter = 0;

}

void SquareWaveChannel::length_enable()
{
	length_timer_enabled = true;
	length_timer = timer & 0b11111;

}

int SquareWaveChannel::get_period_value()
{
	return period_low | ((period_high_control & 0b111) << 8);
}


void SquareWaveChannel::tick()
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
		
		duty_cycle_position++;

		if (duty_cycle_position > 7) {
			duty_cycle_position = 0;
		}

		period_counter = get_period_value();
	}
	else {
		period_counter++;
	}

}

void SquareWaveChannel::tick_length()
{
	if (!length_timer_enabled || !enabled) {
		return;
	}
	
	if (length_timer == 0b11111) {
		length_timer_enabled = false;
		enabled = false;
		return;
	}


	length_timer++;
}

void SquareWaveChannel::tick_envelope()
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

void SquareWaveChannel::tick_sweep()
{

}

void SquareWaveChannel::write(u16 address, u8 value)
{
	// ch2

	if ((address & 0xF) == 0x6) {
		timer = value;
		return;
	}
	else if ((address & 0xF) == 0x7) {
		volume_envelope = value;
		if ((value >> 3) == 0b11111) {
			enabled = false;
		}
		return;
	}
	else if ((address & 0xF) == 0x8) {
		period_low = value;
		return;
	}
	else if ((address & 0xF) == 0x9) {
		period_high_control = value;
		if (value >> 7) {
			trigger();
		}
		if ((value >> 6) & 1) {
			length_enable();
		}
		return;
	}


	// ch1
	if ((address & 0xF) == 0x0) {
		sweep = value;
		return;
	}
	else if ((address & 0xF) == 0x1) {
		timer = value;
		return;
	}
	else if ((address & 0xF) == 0x2) {
		volume_envelope = value;
		if ((value >> 3) == 0b11111) {
			enabled = false;
		}
		return;
	}
	else if ((address & 0xF) == 0x3) {
		period_low = value;
		return;
	}
	else if ((address & 0xF) == 0x4) {
		period_high_control = value;
		if (value >> 7) {
			trigger();
		}
		if ((value >> 6) & 1) {
			length_enable();
		}
		return;
	}



}

u8 SquareWaveChannel::read(u16 address)
{

	// ch2
	
	if ((address & 0xF) == 0x6) {
		return timer;
	}
	else if ((address & 0xF) == 0x7) {
		return volume_envelope;
	}
	else if ((address & 0xF) == 0x8) {
		return period_low;
	}
	else if ((address & 0xF) == 0x9) {
		return period_high_control;
	}

	// ch1
	if ((address & 0xF) == 0x0) {
		return sweep;
	}
	else if ((address & 0xF) == 0x1) {
		return timer;
	}
	else if ((address & 0xF) == 0x2) {
		return volume_envelope;
	}
	else if ((address & 0xF) == 0x3) {
		return period_low;
	}
	else if ((address & 0xF) == 0x4) {
		return period_high_control;
	}
}

float SquareWaveChannel::get_sample()
{
	if (!enabled) {
		return 0.0f;
	}

	int cycle_index = timer >> 6;
	
	int sample_position = duty_cycles[cycle_index][duty_cycle_position];

	return (float)sample_position * current_volume / 1000.0f;

}

bool SquareWaveChannel::is_enabled()
{
	return enabled;
}
