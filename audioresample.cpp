#include "audioresample.h"

#include <QDebug>

extern "C" {
#include <libswresample/swresample.h>
#include <libswscale/swscale.h>
}
#include "codecformatspec.h"

AudioReSample::AudioReSample()
{

}

int AudioReSample::InitSwr(AudioSwrSpec *inputSpec, AudioSwrSpec *outputSpec)
{
    qDebug() << "outputSpec:" << outputSpec->chLayout << " " << outputSpec->sampleFmt << " " << outputSpec->sampleRate;
    qDebug() << "inputSpec:" << inputSpec->chLayout << " " << inputSpec->sampleFmt << " " << inputSpec->sampleRate;

    this->m_inputSpec = inputSpec;
    this->m_outputSpec = outputSpec;

    // Allocate SwrContext if needed and set/reset common parameters.
    m_aSwrCtx = swr_alloc_set_opts(m_aSwrCtx,
                                   av_get_default_channel_layout(outputSpec->chs), outputSpec->sampleFmt, outputSpec->sampleRate,
                                   av_get_default_channel_layout(inputSpec->chs), inputSpec->sampleFmt, inputSpec->sampleRate,
                                   0, nullptr);

    if (!m_aSwrCtx) {
        qDebug() << "swr_alloc_set_opts error";
        return -1;
    }

    // Initialize context after user parameters have been set.
    int ret = swr_init(m_aSwrCtx);
    if (ret < 0) {
        char errbuf[1024];
        av_strerror(ret, errbuf, sizeof (errbuf));
        qDebug() << "swr_init error:" << errbuf;
        swr_free(&m_aSwrCtx);
        return ret;
    }

    return 0;
}

AVFrame *AudioReSample::AllocFrameData()
{
    // TODO: use other alloc buffer
//    ret = av_samples_alloc_array_and_samples(
//              &outData,
//              &outLinesize,
//              outChs,
//              outSamples,
//              outSampleFmt,
//              1);


    // 初始化重采样的输出frame
    AVFrame* swrOutFrame = av_frame_alloc();
    if (!swrOutFrame) {
        qDebug() << "av_frame_alloc error";
        return nullptr;
    }

    //qDebug() << "Frame alloc Success";

    //初始化 outframe->data
    //qDebug() << "_outputSpec->chs:" << _outputSpec->chs << " " << _outputSpec->sampleFmt;
    int ret = av_samples_alloc(swrOutFrame->data, swrOutFrame->linesize,
                                m_outputSpec->chs, 4096, m_outputSpec->sampleFmt, 1);
    if (ret < 0) {
        char errbuf[1024];
        av_strerror(ret, errbuf, sizeof(errbuf));
        qDebug() << "av_samples_alloc error" << errbuf;
        return nullptr;
    }

    return swrOutFrame;
}

int AudioReSample::SwrConvertByFrame(AVFrame *inframe, AVFrame *swrOutFrame)
{
    if (!inframe || !m_aSwrCtx)    return -1;
#if 1
    /*
     inSampleRate     inSamples
     ------------- = -----------
     outSampleRate    outSamples

     outSamples = outSampleRate * inSamples / inSampleRate
     */
     // 缓冲区的样本数量（AV_ROUND_UP是向上取整）
    int outSamples = av_rescale_rnd(m_outputSpec->sampleRate, inframe->nb_samples, m_inputSpec->sampleRate, AV_ROUND_UP);

    if (!swrOutFrame->data[0])	return -1;

    //返回 成功转换的样本数
    int ret = swr_convert(m_aSwrCtx, swrOutFrame->data, inframe->nb_samples,//1024
                        (const uint8_t **)inframe->data, inframe->nb_samples);//1024
    if (ret < 0) {
        char errbuf[1024];
        av_strerror(ret, errbuf, sizeof(errbuf));
        qDebug() << "av_samples_alloc error" << errbuf;
        return -1;
    }
#endif
    return ret * m_outputSpec->chs * av_get_bytes_per_sample(m_outputSpec->sampleFmt);// 成功转换的字节数
}
