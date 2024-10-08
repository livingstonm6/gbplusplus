#pragma once
#include "utility/Common.h"
#include "SquareWaveChannel.h"
#include "..\CustomWaveChannel.h"
#include "..\NoiseWaveChannel.h"
#include "SDL.h"
#include <vector>
#include <queue>

const int SAMPLE_RATE = 44100;
const int BUFFER_SIZE = 1024;//4096;
class APU
{
	int tick_count = 0;
	int total_output_samples = 0;
	int frame_sequencer = 0;


	std::vector<float> front_buffer;
	std::vector<float> back_buffer;

	// Registers

	u8 audio_master_control{};
	u8 sound_panning{};
	u8 master_volume_vin_panning{};

	// Channel 1 - Pulse w/ Sweep
	SquareWaveChannel c1;

	// Channel 2 - Pulse
	SquareWaveChannel c2;

	// Channel 3 - Custom Wave
	CustomWaveChannel c3;

	// Channel 4 - Noise
	NoiseWaveChannel c4;

	// Methods
	void gather_samples();
	void output_audio();

public:
	void write(u16 address, u8 value);
	u8 read(u16 address);

	bool apu_enabled();

	void tick();


	SDL_AudioDeviceID device;

	APU(SDL_AudioDeviceID dev) : device(dev) {}

};

