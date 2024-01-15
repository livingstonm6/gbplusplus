#pragma once
#include "utility/Common.h"

class APU
{
	// Registers

	u8 audio_master_control{};
	u8 sound_panning{};
	u8 master_volume_vin_panning{};

	// Channel 1 - Pulse w/ Sweep
	u8 c1_sweep{};
	u8 c1_timer{};
	u8 c1_volume_envelope{};
	u8 c1_period_low{};
	u8 c1_period_high_control{};

	// Channel 2 - Pulse
	u8 c2_timer{};
	u8 c2_volume_envelope{};
	u8 c2_period_low{};
	u8 c2_period_high_control{};

	// Channel 3 - Custom Wave
	u8 c3_dac_enable{};
	u8 c3_timer{};
	u8 c3_output_level{};
	u8 c3_period_low{};
	u8 c3_period_high_control{};
	u8 c3_wave_pattern_ram[16];

	// Channel 4 - Noise
	u8 c4_timer{};
	u8 c4_volume_envelope{};
	u8 c4_frequency_randomness{};
	u8 c4_control{};



	// Methods

public:
	void write(u16 address, u8 value);
	u8 read(u16 address);

	bool channel_enabled(AudioChannelType);
	bool apu_enabled();

	void tick();

	// Constructor

	APU() {
		for (int i = 0; i < 16; i++) {
			c3_wave_pattern_ram[i] = 0;
		}
	}

};

