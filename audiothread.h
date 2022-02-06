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
    Q_OBJECT

public:
    AudioThread();

    /**
     * @brief AudioThread::Init   打开解码器（音频、视频）
     * @param par
     * @return  >= 0 on success, a negative AVERROR code on failure.
     */
    bool Init(AVCodecParameters *par, AVStream *stream, AudioSwrSpec* outSpec);

    void SetPause(bool);

    void SetStop();

    bool HasRemainFrames();

    void Push(AVPacket *pkt);

    void ClearPktList();

    double GetClockTime();
    void ClearClockTime();

protected:
    virtual void run() Q_DECL_OVERRIDE;

signals:
    void sig_updateTime(double);

private:
    Decode *m_decode;
    AudioReSample* m_as;
    AudioDevice* m_ad;
    AVCodecContext* m_aDecodeCtx = nullptr;

    QMutex mutex;
    std::list<AVPacket*> m_pktList;
    bool m_StopStatus;
    bool m_isPause{false};


    QFile outFile;

    AVStream* m_stream;
    double a_clock = 0;
};

#endif // AUDIOTHREAD_H
