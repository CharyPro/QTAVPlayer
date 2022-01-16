#include "audiodevice.h"
#include <QAudioFormat>
#include <QAudioOutput>
#include <QDebug>

AudioDevice::AudioDevice()
    : output(nullptr), io(nullptr)
{

}

AudioDevice::~AudioDevice()
{

}

bool AudioDevice::Open()
{
    QAudioFormat fmt;
    fmt.setSampleRate(sampleRate);
    fmt.setSampleSize(sampleSize);
    fmt.setChannelCount(channels);
    fmt.setCodec("audio/pcm");
    fmt.setByteOrder(QAudioFormat::LittleEndian);
    fmt.setSampleType(QAudioFormat::UnSignedInt);
    output = new QAudioOutput(fmt);
    io = output->start(); //开始播放
//    if (io)
//        return true;

//    fp.setFileName("D:/test.pcm"); //打开音频文件audio1.pcm获取文件指针fp。r是以只读方式打开资源,b是不转义数据,就是不认转义字符,告诉函数库打开的文件为二进制文件，而非纯文字文件。注意如果写成FILE *fp = fopen("audio1.pcm", "r")会播放不了音频文件
//    if(fp.open(QFile::ReadOnly))
//        qDebug() << "Open Success" ;

    return true;
}

void AudioDevice::Close()
{
    if (io) {
        io->close();
        io = nullptr;
    }
    if (output) {
        output->stop();
        delete output;
        output = nullptr;
    }
}

bool AudioDevice::Write(const unsigned char *data, int datasize)
{
    if (!data || datasize <= 0)return false;

    if (!output || !io)
        return false;

    int free = output->bytesFree();
    if (free < datasize)//剩余空间不够
        return false;

    int size = io->write((char *)data, datasize);
    if (datasize != size)
        return false;

    qDebug() << __FUNCTION__ << datasize;

    return true;
}

AudioDevice *AudioDevice::GetInterface()
{
    static AudioDevice play;
    return &play;
}
