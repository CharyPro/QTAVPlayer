#include "audiodevice.h"

AudioDevice::AudioDevice()
{

}

AudioDevice::~AudioDevice()
{

}

bool AudioDevice::Open()
{
    return true;
}

void AudioDevice::Close()
{

}

bool AudioDevice::Write(const unsigned char *data, int datasize)
{
    return true;
}

AudioDevice *AudioDevice::GetInterface()
{
    static AudioDevice play;
    return &play;
}
