#include "videoplayer.h"

#include <QDebug>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>
#include <libswresample/swresample.h>
#include <libswscale/swscale.h>
}

VideoPlayer::VideoPlayer()
{
    InitValue();
}

void VideoPlayer::SetFileName(QString name)
{
    m_filename = name;
}

void VideoPlayer::Play()
{
    // 1. 解封装


    // 2.
}

void VideoPlayer::InitValue()
{
    m_fmtCtx = nullptr;
}

void VideoPlayer::Demux()
{
    // 1.
    // 传递给输入设备的参数
    AVDictionary *options = nullptr;
//    av_dict_set(&options, "video_size", "640x480", 0);
//    av_dict_set(&options, "pixel_format", "yuyv422", 0);
//    av_dict_set(&options, "framerate", "30", 0);

    int ret = avformat_open_input(&m_fmtCtx, m_filename.toLocal8Bit(), nullptr, &options);
    if (ret < 0) {
        char errbuf[1024];
        av_strerror(ret, errbuf, sizeof (errbuf));
        qDebug() << __FUNCTION__ << " error: " << errbuf;
        return;
    }

    // 2.
    ret = avformat_find_stream_info(m_fmtCtx, nullptr);
    if (ret < 0) {
        char errbuf[1024];
        av_strerror(ret, errbuf, sizeof (errbuf));
        qDebug() << __FUNCTION__ << " error: " << errbuf;
        return;
    }

    av_dump_format(m_fmtCtx, 0, m_filename.toLocal8Bit(), 0);


    // Demux Thread
    AVPacket pkt;
    while(1) {

        ret = av_read_frame(m_fmtCtx, &pkt);
        if(ret == 0) { // Success
            if(pkt.stream_index ) { // video

            } else if(pkt.stream_index) { //audio

            } else {
                av_packet_unref(&pkt);
            }
        } else if(ret == AVERROR_EOF) { // EOF

        } else {
            char errbuf[1024];
            av_strerror(ret, errbuf, sizeof (errbuf));
            qDebug() << __FUNCTION__ << " error: " << errbuf;
            continue;
        }
    }
}
