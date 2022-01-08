#ifndef VIDEOPLAYER_H
#define VIDEOPLAYER_H

#include <QObject>
class AVFormatContext;
class AVStream;
class VideoPlayer : public QObject
{
public:
    VideoPlayer();

    void SetFileName(QString name);

    void Play();

private:
    void InitValue();
    void Demux();

    QString m_filename;
    AVFormatContext* m_fmtCtx;

    AVStream *m_stream;
};

#endif // VIDEOPLAYER_H
