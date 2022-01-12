#ifndef AUDIODEVICE_H
#define AUDIODEVICE_H

class QAudioOutput;
class QIODevice;

class AudioDevice
{
public:
    ~AudioDevice();

    bool Open();
    void Close();
    bool Write(const unsigned char *data, int datasize);

    static AudioDevice* GetInterface();

private:
    int sampleRate = 44100;
    int sampleSize = 16;
    int channels = 2;

    QAudioOutput* output;
    QIODevice* io;

    AudioDevice();

};

#endif // AUDIODEVICE_H
