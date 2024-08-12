//
#include "Motherboard.h"

int main(int arc, char* argv[]) {

	Motherboard emulator;

	emulator.run();

	return 0;
}


//#include <stdio.h>
//#include <SDL.h>
//#include <iostream>
//
//
//int main(int arc, char* argv[]) {
//
//    SDL_Init(SDL_INIT_AUDIO);
//
//	SDL_AudioDeviceID audio_device;
//
//    // opening an audio device:
//    SDL_AudioSpec audio_spec;
//    SDL_zero(audio_spec);
//    audio_spec.freq = 44100;
//    audio_spec.format = AUDIO_F32;
//    audio_spec.channels = 1;
//    audio_spec.samples = 1024;
//    audio_spec.callback = NULL;
//
//    audio_device = SDL_OpenAudioDevice(
//        NULL, 0, &audio_spec, NULL, 0);
//
//    
//
//    // pushing 3 seconds of samples to the audio buffer:
//    int x = 0;
//    for (int i = 0; i < audio_spec.freq * 3; i++) {
//        
//        float sample = 1000.0f / 100.0f;
//        if (i % 80 == 0) {
//            x++;
//            if (x % 8 == 0 || x % 8 == 1) {
//                sample = 0;
//            }
//        }
//
//        // SDL_QueueAudio expects a signed 16-bit value
//        // note: "5000" here is just gain so that we will hear something
//        //int16_t sample = sin(x * 4) * 5000;
//        std::cout << sample << "\n";
//
//        const int sample_size = sizeof(float) * 1;
//        SDL_QueueAudio(audio_device, &sample, sample_size);
//    }
//
//    std::cout << SDL_GetQueuedAudioSize(audio_device) / sizeof(int16_t);
//
//    SDL_PauseAudioDevice(audio_device, 0);
//
//    // unpausing the audio device (starts playing):
//    //SDL_PauseAudioDevice(audio_device, 0);
//
//    SDL_Delay(3000);
//
//    SDL_CloseAudioDevice(audio_device);
//    SDL_Quit();
//
//	return 0;
//}




//#pragma once
//#include <SDL.h>
//#include <iostream>
//
//const int SAMPLE_RATE = 44100;
//const int BUFFER_SIZE = 4096;
//
//typedef struct {
//	float current_step;
//	float step_size;
//	float volume;
//} oscillator;
//
//oscillator oscillate(float rate, float volume) {
//	oscillator o;
//	o.current_step = 0;
//	o.volume = volume;
//	o.step_size = (2 * M_PI) / rate;
//
//	return o;
//}
//
//float next(oscillator* os) {
//	os->current_step += os->step_size;
//	return sinf(os->current_step) * os->volume;
//}
//
//
//float a4_freq = (float)SAMPLE_RATE / 440.00f;
//oscillator a4 = oscillate(a4_freq, 0.1f);
//
//void callback(void* userData, uint8_t* stream, int len) {
//	float* fstream = (float*)stream;
//	for (int i = 0; i < BUFFER_SIZE; i++) {
//		float v = next(&a4);
//		fstream[i] = v;
//	}
//
//}
//
//int main(int arc, char* argv[]) {
//	if (SDL_Init(SDL_INIT_AUDIO | SDL_INIT_EVENTS) < 0){
//		std::cerr << "SDL initialization failed: " << SDL_GetError() << std::endl;
//		return 1;
//	}
//
//	SDL_AudioSpec spec;
//
//	spec.format = AUDIO_F32;
//	spec.channels = 1;
//	spec.freq = SAMPLE_RATE;
//	spec.samples = BUFFER_SIZE;
//	spec.callback = callback;
//
//	if (SDL_OpenAudio(&spec, NULL) < 0) {
//		std::cerr << "Failed to open audio device: " << SDL_GetError() << std::endl;
//		return 1;
//	}
//
//	SDL_PauseAudio(0);
//
//	while (true) {
//		SDL_Event e;
//
//		while (SDL_PollEvent(&e)) {
//			switch (e.type) {
//			case SDL_QUIT:
//				return 0;
//			}
//		}
//	}
//
//
//
//
//	SDL_Quit();
//
//	return 0;
//}
