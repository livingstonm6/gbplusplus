#pragma once
#include "utility/Common.h"
class AudioChannel
{
public:
	AudioChannelType channel_type;
	virtual bool enabled();
	virtual void tick();
};