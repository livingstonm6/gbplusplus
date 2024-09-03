#include "APU.h"
#include <iostream>

void APU::gather_samples()
{
	float sample = c1.get_sample() + c2.get_sample() + c3.get_sample();
	back_buffer.push_back(sample);
	if (back_buffer.size() > BUFFER_SIZE) {
		back_buffer.erase(back_buffer.begin());
	}
	
}

void APU::output_audio()
{
	int queued_audio = SDL_GetQueuedAudioSize(device);

	if (queued_audio) {
		return;
	}


	front_buffer = std::vector<float>(back_buffer);

	const int sample_size = BUFFER_SIZE * sizeof(float);
	SDL_QueueAudio(device, front_buffer.data(), sample_size);

	total_output_samples += BUFFER_SIZE;

	back_buffer.clear();

}

void APU::write(u16 address, u8 value)
{
	// Only FF26 writeable when apu is off
	if (!apu_enabled() && address != 0xFF26) {
		return;
	}

	if (address < 0xFF15) {
		c1.write(address, value);
	}
	else if (address < 0xFF1A) {
		c2.write(address, value);
	}
	else if (address < 0xFF20) {
		c3.write(address, value);
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
		c3.write(address, value);
	}
}

u8 APU::read(u16 address)
{

	if (address < 0xFF15) {
		return c1.read(address);
	}
	else if (address < 0xFF1A) {
		return c2.read(address);
	}
	else if (address < 0xFF20) {
		return c3.read(address);
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
		return (apu_enabled() << 7) | (c2.is_enabled() << 1);
	}
	else if (0xFF30 <= address <= 0xFF3F) {
		return c3.read(address);
	}

}

bool APU::apu_enabled()
{
	return audio_master_control >> 7;
}

void APU::tick()
{
	if (!apu_enabled()) {
		return;
	}

	tick_count++;
	c1.tick();
	c2.tick();
	c3.tick();
	if (tick_count % 95 == 0) { // ~44100hz sample rate
		gather_samples();
		if (back_buffer.size() == BUFFER_SIZE) {
			output_audio();
		}

	}
	if (tick_count % 8192 == 0) {
		frame_sequencer++;
		if (frame_sequencer > 7) {
			frame_sequencer = 0;
		}

		if (frame_sequencer % 2 == 0) {
			c1.tick_length();
			c2.tick_length();
			c3.tick_length();
		}
		if (frame_sequencer == 7) {
			c1.tick_envelope();
			c2.tick_envelope();
		}
		if (frame_sequencer == 2 || frame_sequencer == 6) {
			c1.tick_sweep();
		}
	}
}