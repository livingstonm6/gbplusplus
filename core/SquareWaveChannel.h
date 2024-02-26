#pragma once
#include "AudioChannel.h"

class SquareWaveChannel : public AudioChannel
{


public:
	AudioChannelType channel_type;
	bool enabled() override;
	void tick() override;

};