#ifndef VIDEOPLAYER_H
#define VIDEOPLAYER_H

#include <QThread>
class AVFormatContext;
class AVStream;
class Demux;
class VideoThread;
class AudioThread;
class IVideoDevice;

class VideoPlayer : public QThread
{
    Q_OBJECT

public:
    VideoPlayer();

    void SetFileName(QString name);

    void Play();
protected:
    virtual void run() Q_DECL_OVERRIDE;

private:
    Demux *m_demux;

    QString m_filename;

    VideoThread* m_vt;
    AudioThread* m_at;

    IVideoDevice* m_videoDevice;
};

#endif // VIDEOPLAYER_H
