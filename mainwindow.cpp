#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include "videoplayer.h"

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

    m_player = new VideoPlayer();


}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_openFileBtn_clicked()
{
    QString filename = QFileDialog::getOpenFileName(nullptr,
                       QStringLiteral("选择多媒体文件"),
                       "G:/Resource",
                       QStringLiteral("多媒体文件 (*.mp4 *.avi *.mkv *.mp3 *.aac)"));
    if (filename.isEmpty()) return;

    m_player->SetFileName(filename);

    m_player->Play();

    ui->playWidget->setCurrentWidget(ui->videoPage);
    ui->openGLWidget->Play();
}

