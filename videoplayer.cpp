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
    m_vt = nullptr;
    m_at = nullptr;
    m_demux = nullptr;
    m_videoDevice = nullptr;
}

void VideoPlayer::SetFileName(QString name)
{
    m_filename = name;
}

void VideoPlayer::InitVideoDevice(IVideoDevice *device)
{
    this->m_videoDevice = device;
}

void VideoPlayer::Play()
{
    if (m_state != Stopped) return;

    setState(Playing);

    // 1. 解封装
    m_demux = new Demux();
    if (!m_demux->Open(m_filename))
        return;
    qDebug() << "Demux Open Success";

    //初始化解码器
    m_vt = new VideoThread();
    m_at = new AudioThread();
    connect(m_at, &AudioThread::sig_updateTime, this, &VideoPlayer::sig_timeChanged);

    AudioSwrSpec *outputASpec = (struct AudioSwrSpec *)malloc(sizeof(struct AudioSwrSpec));
    outputASpec->sampleFmt = AV_SAMPLE_FMT_S16;
    outputASpec->sampleRate = 44100;
    outputASpec->chLayout = AV_CH_LAYOUT_STEREO;
    outputASpec->chs = av_get_channel_layout_nb_channels(outputASpec->chLayout);

    bool re = m_at->Init(m_demux->CopyAudioPara(), m_demux->CopyAudioStream(), outputASpec);
    if (!re) {
        return;
    }
    qDebug() << "AudioThread Init Success";

    VideoSwsSpec *outputVSpec = new VideoSwsSpec();

    re = m_vt->Init(m_demux->CopyVideoPara(), outputVSpec,  m_demux->CopyVideoStream(), m_videoDevice);
    if (!re)
        return;
    qDebug() << "VideoThread Init Success";

    VideoDesc desc = {outputVSpec->width, outputVSpec->height, m_demux->GetDuration()};
    emit sig_initFinished(&desc);
    // 2. 开始解封装-》解码-》显示
    this->start();

}


void VideoPlayer::setStop(bool playover)
{
    if(m_state == Stopped) return;

    m_state = Stopped;

    if(playover) {
        while(m_vt->HasRemainFrames() && m_at->HasRemainFrames()) {
            msleep(5);
            continue;
        }
    }

    //释放资源
    //线程停止
    m_vt->SetStop();
    m_at->SetStop();

    emit sig_stateChanged(m_state);
}

void VideoPlayer::setPause(bool pause)
{

    if(pause)
        setState(Paused);
    else
        setState(Playing);

    //线程暂停
    m_vt->SetPause(pause);
    m_at->SetPause(pause);
}

void VideoPlayer::setTime(int seekTime)
{
    m_seekTime = seekTime;
}

void VideoPlayer::run()
{
    m_vt->start();
    m_at->start();

    while (m_state != Stopped) {

        if (m_state == Paused) {
            msleep(5);
            continue;
        }

        if(m_seekTime > 0 && m_demux->SeekFrame(m_seekTime)) {
            // 清空之前读取的数据包
            m_at->ClearPktList();
//            m_at->ClearClockTime();

            m_vt->ClearPktList();
            m_vt->ClearClockTime();

            m_seekTime = -1;
        }

        // 1.获取解封装后的pkt
        AVPacket *pkt = m_demux->ReadPkt();
        if(pkt == nullptr || pkt->size == 0) {
            break; // 没有数据了
        }

        // 2. 处理解封装后的数据
        // 如果要导出，在此处调用重新封装的函数

        // 解码，播放
        // 发送pkt到解码线程
        if (m_demux->GetPktType(pkt) == Demux::AUDIO_TYPE) {
            qDebug() << "Audio Packet";
            m_at->Push(pkt);

        } else if(m_demux->GetPktType(pkt) == Demux::VIDEO_TYPE) {
            qDebug() << "Video Packet";
            m_vt->Push(pkt);
        } else {
            continue;
        }

//        msleep(10);
    }

    setStop(true);

    qDebug() << "OVER";

}

void VideoPlayer::setState(State s)
{
    if(m_state == s) return;

    m_state = s;
    emit sig_stateChanged(m_state);
}
