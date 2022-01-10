#include "videorescale.h"

#include <QDebug>

extern "C" {
#include <libswscale/swscale.h>
#include <libavutil/imgutils.h>
}
#include "codecformatspec.h"

VideoReScale::VideoReScale()
    :m_vSwsCtx(nullptr),
      m_vSwsInFrame(nullptr),
      m_vSwsOutFrame(nullptr)
{

}


int VideoReScale::InitSws(VideoSwsSpec *inputSpec, VideoSwsSpec *outputSpec)
{
    qDebug() << inputSpec->width << " " << inputSpec->height;
    // 如果输出的格式未指定就使用如下默认的参数
    if(outputSpec->width == 0 || outputSpec->height == 0) {
        outputSpec->width = inputSpec->width;
        outputSpec->height = inputSpec->height;
        outputSpec->pixFmt = AV_PIX_FMT_RGB24;// TODO:输入格式
    }

    // Allocate and return an SwsContext. You need it to perform scaling/conversion operations using sws_scale().
    m_vSwsCtx = sws_getContext(inputSpec->width, inputSpec->height, inputSpec->pixFmt,
                               outputSpec->width, outputSpec->height, outputSpec->pixFmt,
                               SWS_BILINEAR, nullptr, nullptr, nullptr);
    if(!m_vSwsCtx) {
        qDebug() << "sws_getContext error";
        return -1;
    }

    return 0;
}

AVFrame *VideoReScale::AllocFrameData()
{
    // Allocate an AVFrame and set its fields to default values
    // this only allocates the AVFrame itself, not the data buffers.
    // freed using av_frame_free().
    AVFrame* OutFrame = av_frame_alloc();
    if (!OutFrame) {
        qDebug() << "av_frame_alloc error";
        return nullptr;
    }

    // Allocate an image data buffers for frame
    // freed by using av_freep(&OutFrame->data[0]).
    int ret = av_image_alloc(OutFrame->data,
                    OutFrame->linesize,
                    m_outputSpec->width, m_outputSpec->height,
                    m_outputSpec->pixFmt, 1);
    if (ret < 0) {
        char errbuf[1024];
        av_strerror(ret, errbuf, sizeof(errbuf));
        qDebug() << "av_image_alloc error" << errbuf;
        return nullptr;
    }

    return OutFrame;
}

int VideoReScale::SwsScale(AVFrame *srcframe, AVFrame *dstframe)
{
    if (!m_vSwsCtx || ! dstframe->data[0])
        return -1;

    // scale frame
    sws_scale(m_vSwsCtx,
              srcframe->data, srcframe->linesize,
              0, m_inputSpec->height,
              dstframe->data, dstframe->linesize);
    return 0;
}
