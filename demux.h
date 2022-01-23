#ifndef DEMUX_H
#define DEMUX_H

#include <QtCore>
class AVCodecParameters;
class AVFormatContext;
class AVPacket;
struct AVStream;

class Demux
{
public:


    enum MediaType {
        AUDIO_TYPE,
        VIDEO_TYPE,
        NONE_TYPE
    };

    enum ErrorType {
        EOF_TYPE,
        OTHER_TYPE,
        NORMAL_TYPE
    };

    Demux();
    bool Open(QString file);
    AVPacket* ReadPkt(ErrorType errtype = NORMAL_TYPE);

    MediaType GetPktType(AVPacket* pkt);

    AVCodecParameters* CopyVideoPara();
    AVCodecParameters* CopyAudioPara();

    AVStream* CopyVideoStream();
    AVStream* CopyAudioStream();

private:
    AVFormatContext* m_fmtCtx = nullptr;

    int m_vStreamIndex, m_aStreamIndex;

};

#endif // DEMUX_H
