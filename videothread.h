#ifndef VIDEOTHREAD_H
#define VIDEOTHREAD_H

#include <QList>
#include <QMutex>
#include <QThread>
#include <QFile>


class Decode;
struct AVCodecParameters;
struct VideoSwsSpec;
class IVideoDevice;
class VideoReScale;
struct AVPacket;
struct AVStream;

class VideoThread : public QThread
{
public:
    VideoThread();

    /**
     * @brief AudioThread::Init   打开解码器（视频）
     * @param par
     * @return
     */
    bool Init(AVCodecParameters *par, VideoSwsSpec* outSpec, AVStream *stream, IVideoDevice* videoDevice);


    /**
     * @brief Push  将用于解码的包存到List中
     * @param pkt
     */
    void Push(AVPacket* pkt);

    void SetStop(bool status);

    void SetSycClock(double );

    double GetClockTime();


protected:
    virtual void run() Q_DECL_OVERRIDE;

private:
    Decode* m_decode;
    QMutex m_mutex;

    VideoReScale *m_vs = nullptr;
    IVideoDevice* m_videoDevice = nullptr;

    VideoSwsSpec* m_outSpec;
    bool m_StopStatus;
    QList<AVPacket*> m_pktList;
    AVStream *v_stream;


    QFile file;
    FILE *video_dst_file = NULL;
    int imageSize;
    int video_dst_bufsize;

    double _vTime = 0;
    double m_clocks = 9999;
};

#endif // VIDEOTHREAD_H
