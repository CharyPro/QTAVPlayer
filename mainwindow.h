#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
class VideoPlayer;

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

private:
    Ui::MainWindow *ui;

    VideoPlayer *m_player;
};
#endif // MAINWINDOW_H
