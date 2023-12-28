#include "APU.h"

void APU::write(u16 address, u8 value)
{
	// Only FF26 writeable when apu is off
	if (!apu_enabled() && address != 0xFF26) {
		return;
	}

	if (address == 0xFF10) {
		c1_sweep = value;
	}
	else if (address == 0xFF11) {
		c1_timer = value;
	}
	else if (address == 0xFF12) {
		c1_volume_envelope = value;
	}
	else if (address == 0xFF13) {
		c1_period_low = value;
	}
	else if (address == 0xFF14) {
		c1_period_high_control = value;
	}
	else if (address == 0xFF16) {
		c2_timer = value;
	}
	else if (address == 0xFF17) {
		c2_volume_envelope = value;
	}
	else if (address == 0xFF18) {
		c2_period_low = value;
	}
	else if (address == 0xFF19) {
		c2_period_high_control = value;
	}
	else if (address == 0xFF1A) {
		c3_dac_enable = value;
	}
	else if (address == 0xFF1B) {
		c3_timer = value;
	}
	else if (address == 0xFF1C) {
		c3_output_level = value;
	}
	else if (address == 0xFF1D) {
		c3_period_low = value;
	}
	else if (address == 0xFF1E) {
		c3_period_high_control = value;
	}
	else if (address == 0xFF20) {
		c4_timer = value;
	}
	else if (address == 0xFF21) {
		c4_volume_envelope = value;
	}
	else if (address == 0xFF22) {
		c4_frequency_randomness = value;
	}
	else if (address == 0xFF23) {
		c4_control = value;
	}
	else if (address == 0xFF24) {
		master_volume_vin_panning = value;
	}
	else if (address == 0xFF25) {
		sound_panning = value;
	}
	else if (address == 0xFF26) {  // only bit 7 writeable
		audio_master_control = (audio_master_control & 0b1111111) | (value & 0b10000000);
	}
	else if (0xFF30 <= address <= 0xFF3F) {
		c3_wave_pattern_ram[address & 0xF] = value;
	}
}

u8 APU::read(u16 address)
{

	if (address == 0xFF10) {
		return c1_sweep;
	}
	else if (address == 0xFF11) {
		return c1_timer;
	}
	else if (address == 0xFF12) {
		return c1_volume_envelope;
	}
	else if (address == 0xFF13) {
		return c1_period_low;
	}
	else if (address == 0xFF14) {
		return c1_period_high_control;
	}
	else if (address == 0xFF16) {
		return c2_timer;
	}
	else if (address == 0xFF17) {
		return c2_volume_envelope;
	}
	else if (address == 0xFF18) {
		return c2_period_low;
	}
	else if (address == 0xFF19) {
		return c2_period_high_control;
	}
	else if (address == 0xFF1A) {
		return c3_dac_enable;
	}
	else if (address == 0xFF1B) {
		return c3_timer;
	}
	else if (address == 0xFF1C) {
		return c3_output_level;
	}
	else if (address == 0xFF1D) {
		return c3_period_low;
	}
	else if (address == 0xFF1E) {
		return c3_period_high_control;
	}
	else if (address == 0xFF20) {
		return c4_timer;
	}
	else if (address == 0xFF21) {
		return c4_volume_envelope;
	}
	else if (address == 0xFF22) {
		return c4_frequency_randomness;
	}
	else if (address == 0xFF23) {
		return c4_control;
	}
	else if (address == 0xFF24) {
		return master_volume_vin_panning;
	}
	else if (address == 0xFF25) {
		return sound_panning;
	}
	else if (address == 0xFF26) {
		return audio_master_control;
	}
	else if (0xFF30 <= address <= 0xFF3F) {
		return c3_wave_pattern_ram[address & 0xF];
	}

}

bool APU::channel_enabled(AudioChannelType channel)
{
	switch (channel) {
	case ACT_1:
		return audio_master_control & 1;
	case ACT_2:
		return audio_master_control & 0b10;
	case ACT_3:
		return audio_master_control & 0b100;
	case ACT_4:
		return audio_master_control & 0b1000;
	}

}

bool APU::apu_enabled()
{
	return audio_master_control >> 7;
}

void APU::tick()
{
}
