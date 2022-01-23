#ifndef AUDIOTHREAD_H
#define AUDIOTHREAD_H

#include <QThread>
#include <QMutex>
#include <QFile>

struct AVCodecParameters;
struct AudioSwrSpec;
struct AVPacket;
struct AVCodec;
struct AVCodecContext;
class Decode;
class AudioReSample;
class AudioDevice;
struct AVStream;

class AudioThread : public QThread
{
public:
    AudioThread();

    /**
     * @brief AudioThread::Init   打开解码器（音频、视频）
     * @param par
     * @return  >= 0 on success, a negative AVERROR code on failure.
     */
    bool Init(AVCodecParameters *par, AVStream *stream, AudioSwrSpec* outSpec);

    void Push(AVPacket *pkt);

    double GetClockTime();

protected:
    virtual void run() Q_DECL_OVERRIDE;

private:
    Decode *m_decode;
    AudioReSample* m_as;
    AudioDevice* m_ad;
    AVCodecContext* m_aDecodeCtx = nullptr;

    QMutex mutex;
    std::list<AVPacket*> m_pktList;
    bool m_StopStatus;

    QFile outFile;

    AVStream* m_stream;
    double a_clock = 0;
};

#endif // AUDIOTHREAD_H
