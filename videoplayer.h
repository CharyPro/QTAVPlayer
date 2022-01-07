#ifndef VIDEOPLAYER_H
#define VIDEOPLAYER_H

#include <QObject>

class VideoPlayer : public QObject
{
public:
    VideoPlayer();

    void SetFileName(QString name);

    void Play();
};

#endif // VIDEOPLAYER_H
