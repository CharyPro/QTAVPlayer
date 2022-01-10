#ifndef IVIDEODEVICE_H
#define IVIDEODEVICE_H

struct AVFrame;
class IVideoDevice
{
public:
    virtual void Init(int width, int height) = 0;
    virtual void Repaint(AVFrame *frame) = 0;
};

#endif // IVIDEODEVICE_H
