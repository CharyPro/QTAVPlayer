#include "decode.h"

#include <QDebug>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>
#include <libswresample/swresample.h>
#include <libswscale/swscale.h>
}

#include "codecformatspec.h"
char av_error[AV_ERROR_MAX_STRING_SIZE] = { 0 };
#define av_err2str(errnum) av_make_error_string(av_error, AV_ERROR_MAX_STRING_SIZE, errnum)

Decode::Decode()
    :m_codec(nullptr), m_decodeCtx(nullptr)
{

}

int Decode::Open(AVCodecParameters* par, AVStream* stream)
{
    int ret = 0;
    if (!par) return -1;

    //查找解码器根据默认的解码器id
    m_codec = avcodec_find_decoder(par->codec_id);
    if (!m_codec) {
        avcodec_parameters_free(&par);
        qDebug() << "can't find the codec id " << par->codec_id;
        return -1;
    }

    // 分配AvcodeContext并将其字段设置为默认值
    m_decodeCtx = avcodec_alloc_context3(m_codec);
    if (!m_decodeCtx) {
        avcodec_parameters_free(&par);
        qDebug() << "can't avcodec_alloc_context3 : " << par->codec_id;
        return -1;
    }
    qDebug() << "OVER CODEC";

    // 根据提供的编解码器参数中的值填充编解码器上下文 AvcodeContext
    ret = avcodec_parameters_to_context(m_decodeCtx, par);
    if (ret < 0) {
        avcodec_parameters_free(&par);
        qDebug() << "avcodec_parameters_to_context : " << par->codec_id;
        return ret;
    }

    avcodec_parameters_free(&par);

    // Initialize the AVCodecContext to use the given AVCodec  初始化打开解码器
    ret = avcodec_open2(m_decodeCtx, m_codec, NULL);
    if (ret < 0) {
        qDebug() << "avcodec_open2 : " << par->codec_id;
        return ret;
    }

    this->stream = stream;

    qDebug() << "OVER CODEC";
    return 0;
}

AudioSwrSpec *Decode::GetAudioCodecSpec()
{
    AudioSwrSpec *spec = (struct AudioSwrSpec *)malloc(sizeof(struct AudioSwrSpec));
    spec->sampleFmt = m_decodeCtx->sample_fmt;//采样格式
    spec->sampleRate = m_decodeCtx->sample_rate;//采样率
    spec->chLayout = m_decodeCtx->channel_layout;//通道数
    spec->chs = m_decodeCtx->channels;

    return spec;
}

VideoSwsSpec *Decode::GetVideoCodecSpec()
{
    VideoSwsSpec *spec = (struct VideoSwsSpec *)malloc(sizeof(struct VideoSwsSpec));
    spec->width = m_decodeCtx->width;// 宽
    spec->height = m_decodeCtx->height;// 高
    spec->pixFmt = m_decodeCtx->pix_fmt;// 像素格式

    return spec;
}

int Decode::SendPkt(AVPacket *pkt)
{
    if (!pkt || pkt->size <= 0 || !pkt->data)
        return -1;

    if (!m_decodeCtx)
        return -1;

    int ret = avcodec_send_packet(m_decodeCtx, pkt);
    av_packet_free(&pkt); // free packet whatever success or false

    if (ret != 0) {
        char errbuf[1024];
        av_strerror(ret, errbuf, sizeof(errbuf));
        qDebug() << "avcodec_send_packet error" << errbuf;
        return -1;
    }
    qDebug() << "SendPkt Success";
    return 0;
}

double Decode::synchronize(AVFrame *srcFrame, double pts)
{
    double frame_delay;

//    if (pts != 0)
//        video_clock = pts; // Get pts,then set video clock to it
//    else
//        pts = video_clock; // Don't get pts,set it to video clock

//    frame_delay = av_q2d(stream->codec->time_base);
//    frame_delay += srcFrame->repeat_pict * (frame_delay * 0.5);

//    video_clock += frame_delay;

    return pts;
}
AVFrame* Decode::RecvFrame()
{
    if (!m_decodeCtx)
        return nullptr;

    AVFrame *frame = av_frame_alloc();
    // Return decoded output data from a decoder.
    int ret = avcodec_receive_frame(m_decodeCtx, frame);
    if (ret < 0) {
        if (ret == AVERROR_EOF || ret == AVERROR(EAGAIN))
            return 0;
        fprintf(stderr, "Error during decoding (%s)\n", av_err2str(ret));

        av_frame_free(&frame);
        return nullptr;
    }

    // 音视频同步相关代码

#if 0

    double pts;
    if ((pts = av_frame_get_best_effort_timestamp(frame)) == AV_NOPTS_VALUE)
        pts = 0;

    pts *= av_q2d(stream->time_base);

    pts = video->synchronize(frame, pts);

    frame->opaque = &pts;
#endif

    qDebug() << "[" << frame->linesize[0] << "] ";
    return frame;
}
