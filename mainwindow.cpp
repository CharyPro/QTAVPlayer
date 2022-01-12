#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QDesktopWidget>

#include "videoplayer.h"
#include "iowidget.h"

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

    iow = new IOWidget(this);
    iow->raise();

//    int appWindowWidth = iow->geometry().width();
//    int appWindowHeight = iow->geometry().height();

//    int center_y = screenResolution->height()/2 - appWindowHeight/2;
//    int center_x = screenResolution->width()/2 - appWindowWidth/2;

    //TODO: 界面上添加IO用于导入导出
//    iow->setGeometry(center_x, center_y,
//                            appWindowWidth, appWindowHeight);

//    ui->openGLWidget->Play();
    ui->muteBtn->setEnabled(true);
    connect(ui->muteBtn, SIGNAL(clicked()), this, SLOT(openFileBtn_clicked()));
    m_player = new VideoPlayer();

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::openFileBtn_clicked()
{
    QString filename = QFileDialog::getOpenFileName(nullptr,
                       QStringLiteral("选择多媒体文件"),
                       "G:/Resource",
                       QStringLiteral("多媒体文件 (*.mp4 *.avi *.mkv *.mp3 *.aac)"));
    if (filename.isEmpty()) return;

//    m_player->SetFileName(filename);

    m_player->SetFileName("G:\\Resource\\Room.mp4");
    m_player->Play();

//    ui->playWidget->setCurrentWidget(ui->videoPage);
//    ui->playWidget->show();
}

