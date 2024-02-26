
#include "Motherboard.h"

int main(int arc, char* argv[]) {

	Motherboard emulator;

	emulator.run();

	return 0;
}



//#pragma once
//#include <SDL.h>
//#include <iostream>
//#include <cmath>
//#include "core/utility/Common.h"
//
//const int SAMPLE_RATE = 44100;
//const int AMPLITUDE = 32767;
//const double FREQUENCY = 440.0; // Hz
//
//void generateSineWave(Sint16* buffer, int length) {
//    for (int i = 0; i < length; ++i) {
//        double time = static_cast<double>(i) / SAMPLE_RATE;
//        double value = std::sin(2.0 * M_PI * FREQUENCY * time);
//        buffer[i] = static_cast<Sint16>(AMPLITUDE * value);
//    }
//}
//
//int main() {
//    if (SDL_Init(SDL_INIT_AUDIO) < 0) {
//        std::cerr << "SDL initialization failed: " << SDL_GetError() << std::endl;
//        return 1;
//    }
//
//    SDL_AudioSpec spec;
//    spec.freq = SAMPLE_RATE;
//    spec.format = AUDIO_S16SYS;
//    spec.channels = 1;
//    spec.samples = 4096;
//    spec.callback = nullptr; // No callback function
//
//    SDL_AudioDeviceID audioDevice = SDL_OpenAudioDevice(nullptr, 0, &spec, nullptr, 0);
//    if (audioDevice == 0) {
//        std::cerr << "Failed to open audio device: " << SDL_GetError() << std::endl;
//        SDL_Quit();
//        return 1;
//    }
//
//    SDL_PauseAudioDevice(audioDevice, 0); // Unpause audio device
//
//    const int BUFFER_SIZE = spec.samples;
//    Sint16* buffer = new Sint16[BUFFER_SIZE];
//
//    generateSineWave(buffer, BUFFER_SIZE);
//
//    int index = 0;
//    while (true) {
//        SDL_QueueAudio(audioDevice, buffer, sizeof(buffer));
//        SDL_Delay(100); // Adjust the delay based on your requirements
//
//        // Break the loop after a certain duration or condition
//        if (++index > 100) {
//            break;
//        }
//    }
//
//    delete[] buffer;
//    SDL_CloseAudioDevice(audioDevice);
//    SDL_Quit();
//
//    return 0;
//}
