#include "demux.h"
extern "C" {
#include <libavformat/avformat.h>
}

Demux::Demux()
{
    av_register_all();
}

bool Demux::Open(QString m_filename)
{
    // 1.
    // 传递给输入设备的参数
    AVDictionary *options = nullptr;
//    av_dict_set(&options, "video_size", "640x480", 0);
//    av_dict_set(&options, "pixel_format", "yuyv422", 0);
//    av_dict_set(&options, "framerate", "30", 0);
    qDebug() << m_filename.toLocal8Bit();
    int ret = avformat_open_input(&m_fmtCtx, m_filename.toLocal8Bit(), nullptr, &options);
    if (ret < 0) {
        char errbuf[1024];
        av_strerror(ret, errbuf, sizeof (errbuf));
        qDebug() << "avformat_open_input" << " error: " << errbuf;
        return false;
    }

    // 2.
    ret = avformat_find_stream_info(m_fmtCtx, nullptr);
    if (ret < 0) {
        char errbuf[1024];
        av_strerror(ret, errbuf, sizeof (errbuf));
        qDebug() << "avformat_find_stream_info" << " error: " << errbuf;
        return false;
    }

    av_dump_format(m_fmtCtx, 0, m_filename.toLocal8Bit(), 0);

    m_vStreamIndex = av_find_best_stream(m_fmtCtx, AVMEDIA_TYPE_VIDEO, -1, -1, nullptr, 0);
    m_aStreamIndex = av_find_best_stream(m_fmtCtx, AVMEDIA_TYPE_AUDIO, -1, -1, nullptr, 0);

    AVStream *vs = m_fmtCtx->streams[m_vStreamIndex];
    qDebug() << "codec_id = " << vs->codecpar->codec_id;
    qDebug() << "format = " << vs->codecpar->format;
    qDebug() << "width=" << vs->codecpar->width;
    qDebug() << "height=" << vs->codecpar->height;

    AVStream *as = m_fmtCtx->streams[m_aStreamIndex];
    qDebug() << "codec_id = " << as->codecpar->codec_id;
    qDebug() << "format = " << as->codecpar->format;
    qDebug() << "sample_rate = " << as->codecpar->sample_rate;
    qDebug() << "channel=" << as->codecpar->channels;
    qDebug() << "channel2=" << as->codecpar->channel_layout;

}

// 记得释放 av_packet_free
AVPacket* Demux::ReadPkt(ErrorType error)
{
    if (!m_fmtCtx)	return nullptr;

    AVPacket* pkt = av_packet_alloc();
    int ret = av_read_frame(m_fmtCtx, pkt);
    if (ret == 0) {
        error = NORMAL_TYPE;
    } else if (ret == AVERROR_EOF) { // 读到了文件的尾部
        error = EOF_TYPE;
        return nullptr;
    } else {
        char errbuf[1024];
        av_strerror(ret, errbuf, sizeof (errbuf));
        qDebug() << __FUNCTION__ << " error: " << errbuf;

        error = OTHER_TYPE;
        av_packet_free(&pkt);
        return nullptr;
    }

    return pkt;
}

Demux::MediaType Demux::GetPktType(AVPacket *pkt)
{
    if(!pkt)    return NONE_TYPE;

    if (pkt->stream_index == m_aStreamIndex) { // 读取到的是音频数据
            return AUDIO_TYPE;
    }
    else if (pkt->stream_index == m_vStreamIndex) { // 读取到的是视频数据
        return VIDEO_TYPE;
    }
    else
        return NONE_TYPE;
}

/**
 * @brief DemuxThread::CopyVideoPara
 * @return  具有视频编码流的属性 AVCodecParameters ，在解码时需要视频流的编码ID
 */
AVCodecParameters* Demux::CopyVideoPara()
{
    //Allocate a new AVCodecParameters and set its fields to default values.  must free avcodec_parameters_free()
    AVCodecParameters *p = avcodec_parameters_alloc();
    avcodec_parameters_copy(p, m_fmtCtx->streams[m_vStreamIndex]->codecpar);

    return p;
}
/**
 * @brief DemuxThread::CopyAudioPara
 * @return  具有音频编码流的属性 AVCodecParameters ，在解码时需要音频流的编码ID
 */
AVCodecParameters* Demux::CopyAudioPara()
{
    AVCodecParameters *p = avcodec_parameters_alloc();
    avcodec_parameters_copy(p, m_fmtCtx->streams[m_aStreamIndex]->codecpar);

    return p;
}
