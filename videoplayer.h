#ifndef VIDEOPLAYER_H
#define VIDEOPLAYER_H

#include <QThread>
class AVFormatContext;
class AVStream;
class Demux;
class VideoThread;
class AudioThread;
class IVideoDevice;

struct VideoDesc {
    int width;
    int height;
    int duration;
};

class VideoPlayer : public QThread
{
    Q_OBJECT

public:
    // Status
    typedef enum {
        Stopped = 0,
        Playing,
        Paused
    } State;


    VideoPlayer();

    void SetFileName(QString name);
    void InitVideoDevice(IVideoDevice *device);
    void Play();

    void setStop(bool isover = false);
    void setPause(bool);
    void setContinue();
    void setTime(int seekTime);

protected:
    virtual void run() Q_DECL_OVERRIDE;

signals:
    void sig_initFinished(VideoDesc*);
    void sig_timeChanged(double);
    void sig_stateChanged(State);

private:
    Demux *m_demux;

    QString m_filename;

    VideoThread* m_vt;
    AudioThread* m_at;

    IVideoDevice* m_videoDevice;

    State m_state{Stopped};
    int m_seekTime{-1};

    void setState(State);
};

#endif // VIDEOPLAYER_H
