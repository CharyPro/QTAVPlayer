#include "videoplayer.h"

#include <QDebug>
#include "videothread.h"
#include "audiothread.h"
#include "codecformatspec.h"
#include "demux.h"

extern "C" {
#include <libavformat/avformat.h>
}
VideoPlayer::VideoPlayer()
{

}

void VideoPlayer::SetFileName(QString name)
{
    m_filename = name;
}



void VideoPlayer::Play()
{
    // 1. 解封装
    m_demux = new Demux();
    if (!m_demux->Open(m_filename))
        return;
    qDebug() << "Demux Open Success";

    //初始化解码器
    m_vt = new VideoThread();
//    m_at = new AudioThread();

    VideoSwsSpec *outputVSpec = new VideoSwsSpec();

    bool re = m_vt->Init(m_demux->CopyVideoPara(), outputVSpec, m_videoDevice);
    if (!re)
        return;
    qDebug() << "VideoThread Init Success";


    // 2. 开始解封装-》解码-》显示
    this->start();

}

void VideoPlayer::run()
{
    while (1) {
        // 1.获取解封装后的pkt
        AVPacket *pkt = m_demux->ReadPkt();
        if(!pkt || pkt->size == 0) {
            break; // 没有数据了
        }

        // 2. 处理解封装后的数据
        // 如果要导出，在此处调用重新封装的函数

        // 解码，播放
        // 发送pkt到解码线程
        if (m_demux->GetPktType(pkt) == Demux::AUDIO_TYPE) {

        } else if(m_demux->GetPktType(pkt) == Demux::VIDEO_TYPE) {
            m_vt->Push(pkt);
        } else {
            continue;
        }
    }
}
