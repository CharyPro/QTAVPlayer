#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
class VideoPlayer;
class IOWidget;
struct VideoDesc;
class VideoSlider;

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void on_openFileBtn_clicked();
    void on_stopBtn_clicked();
    void on_playBtn_clicked();
    void on_toolBtn_clicked();
    void on_playerInitFinished(VideoDesc*);
    void on_playerStateChanged(int);
    void on_playerTimeChanged(int);
    void onSliderClicked(VideoSlider *slider);

private:
    Ui::MainWindow *ui;

    IOWidget *iow;
    VideoPlayer *m_player;


    inline QString getTimeFromSec(int);
};
#endif // MAINWINDOW_H
