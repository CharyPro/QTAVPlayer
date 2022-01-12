#ifndef AUDIORESAMPLE_H
#define AUDIORESAMPLE_H

struct AVCodecContext;
struct AudioSwrSpec;
struct AVFrame;
struct SwrContext;

class AudioReSample
{
public:
    AudioReSample();

    /**
     * @brief InitSwr     Init Resample Audio Spec
     * @param inputSpec
     * @param outputSpec
     * @return
     */
    int InitSwr(AudioSwrSpec *inputSpec, AudioSwrSpec *outputSpec);

    AVFrame* AllocFrameData();

    int SwrConvertByFrame(AVFrame* inframe, AVFrame* swrOutFrame);
    int SwrConvert(AVFrame* frame, unsigned char *pcm);

private:
    AudioSwrSpec* m_outputSpec;
    AudioSwrSpec* m_inputSpec;

    AVFrame *m_swrOutFrame1;

    SwrContext *m_aSwrCtx = nullptr;

};

#endif // AUDIORESAMPLE_H
