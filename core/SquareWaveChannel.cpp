#include "SquareWaveChannel.h"
#include <iostream>
#include <math.h>

void SquareWaveChannel::trigger()
{
	enabled = true;
	current_volume = volume_envelope >> 4;
	period_counter = get_period_value();
	duty_cycle_position = 0;
	envelope_counter = 0;
	sweep_pace = (sweep >> 4) & 0b111;

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
	if (!enabled || sweep_pace == 0) {
		return;
	}

	sweep_counter++;

	if (sweep_counter % sweep_pace != 0) {
		return;
	}

	sweep_pace = (sweep >> 4) & 0b111;

	int step = sweep & 0b111;

	int current_period = get_period_value();

	int decreasing = (sweep >> 3) & 1;

	int new_period = current_period;

	if (decreasing) {
		new_period -= current_period / pow(2, step);
	}
	else {
		int temp = new_period + (current_period / pow(2, step));
		if (temp > 0x7FF) {
			enabled = false;
			return;
		}
		new_period = temp;
	}

	period_low = new_period & 0xFF;

	period_high_control = (period_high_control >> 3) << 3;
	period_high_control |= (new_period >> 8);


}

void SquareWaveChannel::write(u16 address, u8 value)
{
	u16 register_index = (address & 0xF) % 5;

	switch (register_index) {
	case 0:
		sweep = value;
		break;
	case 1:
		timer = value;
		break;
	case 2:
		volume_envelope = value;
		if ((value >> 3) == 0b11111) {
			enabled = false;
		}
		break;
	case 3:
		period_low = value;
		break;
	case 4:
		period_high_control = value;
		if (value >> 7) {
			trigger();
		}
		if ((value >> 6) & 1) {
			length_enable();
		}
		break;
	default:
		std::cerr << "Invalid SquareWaveChannel register index\n";
		exit(-2);
	}

}

u8 SquareWaveChannel::read(u16 address)
{
	u16 register_index = (address & 0xF) % 5;

	switch (register_index) {
	case 0:
		return sweep;
	case 1:
		return timer;
	case 2:
		return volume_envelope;
	case 3:
		return period_low;
	case 4:
		return period_high_control;
	default:
		std::cerr << "Invalid SquareWaveChannel register index\n";
		exit(-2);
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
