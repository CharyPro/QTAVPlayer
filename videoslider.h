#ifndef VIDEOSLIDER_H
#define VIDEOSLIDER_H

#include <QSlider>

class VideoSlider : public QSlider
{
    Q_OBJECT

public:
    explicit VideoSlider(QWidget *parent = nullptr);

signals:
    void clicked(VideoSlider *slider);

private:
    virtual void mousePressEvent(QMouseEvent *ev) Q_DECL_OVERRIDE;
};

#endif // VIDEOSLIDER_H
