#ifndef VIDEOTHREAD_H
#define VIDEOTHREAD_H

#include <QList>
#include <QMutex>
#include <QThread>


class Decode;
class AVCodecParameters;
class VideoSwsSpec;
class IVideoDevice;
class VideoReScale;
struct AVPacket;

class VideoThread : public QThread
{
public:
    VideoThread();

    /**
     * @brief AudioThread::Init   打开解码器（视频）
     * @param par
     * @return
     */
    bool Init(AVCodecParameters *par, VideoSwsSpec* outSpec, IVideoDevice* videoDevice);


    /**
     * @brief Push  将用于解码的包存到List中
     * @param pkt
     */
    void Push(AVPacket* pkt);

protected:
    virtual void run() Q_DECL_OVERRIDE;

private:
    Decode* m_decode;
    QMutex m_mutex;

    VideoReScale *m_vs = nullptr;
    IVideoDevice* m_videoDevice = nullptr;

    bool m_StopStatus;
    QList<AVPacket*> m_pktList;
};

#endif // VIDEOTHREAD_H
