#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QDesktopWidget>

#include "videoplayer.h"
#include "iowidget.h"

#include "encryptwidget.h"

///
/// 1. 导入视频播放
/// 2. 导出不同格式的视频
/// 3. 插件（DoFun_PC)
///
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    this->centralWidget()->layout()->setMargin(0);

    QDesktopWidget *screenResolution = QApplication::desktop();

    qRegisterMetaType<VideoPlayer::State>("State");
    qRegisterMetaType<VideoDesc>("VideoDesc");


    iow = new IOWidget(this);
    iow->raise();

//    int appWindowWidth = iow->geometry().width();
//    int appWindowHeight = iow->geometry().height();

//    int center_y = screenResolution->height()/2 - appWindowHeight/2;
//    int center_x = screenResolution->width()/2 - appWindowWidth/2;

    //TODO: 界面上添加IO用于导入导出
//    iow->setGeometry(center_x, center_y,
//                            appWindowWidth, appWindowHeight);


    m_player = new VideoPlayer();
    m_player->InitVideoDevice(ui->openGLWidget);


//    connect(ui->openBtn, SIGNAL(clicked()), this, SLOT(on_openFileBtn_clicked()));
    connect(m_player, &VideoPlayer::sig_initFinished, this, &MainWindow::on_playerInitFinished);
    connect(m_player, &VideoPlayer::sig_stateChanged, this, &MainWindow::on_playerStateChanged);
    connect(m_player, &VideoPlayer::sig_timeChanged, this, &MainWindow::on_playerTimeChanged);
//    connect(m_player, &VideoPlayer::stateChanged,
//            ui->videoWidget, &VideoWidget::onPlayerStateChanged);

    // 监听时间滑块的点击
    connect(ui->timeSlider, &VideoSlider::clicked,
            this, &MainWindow::onSliderClicked);
}

void MainWindow::onSliderClicked(VideoSlider *slider) {
    qDebug() << __FUNCTION__ << "slider->value():" << slider->value();
    m_player->setTime(slider->value());
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_stopBtn_clicked() {
    m_player->setStop();
}

void MainWindow::on_playBtn_clicked() {
    static bool isPause = true;
    if(isPause)
        m_player->setPause(isPause);
    else
        m_player->setPause(isPause);

    isPause = !isPause;
}

void MainWindow::on_openFileBtn_clicked()
{
    QString filename = QFileDialog::getOpenFileName(nullptr,
                       QStringLiteral("选择多媒体文件"),
                       "G:/Resource",
                       QStringLiteral("多媒体文件 (*.mp4 *.avi *.mkv *.mp3 *.aac)"));
    if (filename.isEmpty()) return;

    m_player->SetFileName(filename);

//    m_player->SetFileName("G:\\Resource\\Room.mp4");
    m_player->Play();

//    ui->playWidget->setCurrentWidget(ui->videoPage);
//    ui->playWidget->show();
}

void MainWindow::on_playerInitFinished(VideoDesc* desc)
{
    ui->timeSlider->setRange(0, desc->duration);

    ui->durationLabel->setText(getTimeFromSec(desc->duration));

    qDebug() << "Width:" << desc->width << " " << desc->height;
    this->resize(desc->width, desc->height);
}

void MainWindow::on_playerStateChanged(int value)
{
    qDebug() << "State:" << value;
    VideoPlayer::State state = (VideoPlayer::State)value;
    if(state == VideoPlayer::Playing) {
        ui->playBtn->setEnabled(true);
        ui->stopBtn->setEnabled(true);
        ui->timeSlider->setEnabled(true);
        ui->volumnSlider->setEnabled(true);
        ui->muteBtn->setEnabled(true);
        ui->openGLWidget->SetStatus(true);

    } else if(state == VideoPlayer::Stopped) {
        ui->playBtn->setEnabled(false);
        ui->stopBtn->setEnabled(false);
        ui->timeSlider->setEnabled(false);
        ui->volumnSlider->setEnabled(false);
        ui->muteBtn->setEnabled(false);

        ui->openGLWidget->SetStatus(false);
    }
}

void MainWindow::on_playerTimeChanged(int value)
{
    qDebug() << __FUNCTION__ << "value:" << value;
    ui->timeSlider->setValue(value);
}


//void MainWindow::on_timeSlider_valueChanged(int value) {
//    ui->timeLabel->setText(getTimeFromSec(value));
//}


QString MainWindow::getTimeFromSec(int value)
{
    QLatin1Char fill = QLatin1Char('0');
    return QString("%1:%2:%3").arg(value/3600, 2, 10, fill)
            .arg((value / 60) % 60, 2, 10, fill)
            .arg(value % 60, 2, 10, fill);
}

void MainWindow::on_toolBtn_clicked()
{
    EncryptWidget* widget = new EncryptWidget();

    widget->show();
}

