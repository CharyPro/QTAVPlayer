#ifndef VIDEORESCALE_H
#define VIDEORESCALE_H
class SwsContext;
struct VideoSwsSpec;
struct AVFrame;

class VideoReScale
{
public:
    VideoReScale();

    /**
     * @brief VideoReScale::InitSws     init video rescale
     * @param inputSpec                 the input video spec
     * @param outputSpec                the output video spec
     * @return
     */
    int InitSws(VideoSwsSpec *inputSpec, VideoSwsSpec *outputSpec);

    /**
     * @brief AllocFrameData        Alloc frame data for after SwsScale
     * @return
     */
    AVFrame* AllocFrameData();

    /**
     * @brief SwsScale  Scale the srcframe to dstframe
     * @return
     */
    int SwsScale(AVFrame *srcframe, AVFrame *dstframe);

private:
    SwsContext* m_vSwsCtx;

    AVFrame* m_vSwsInFrame;
    AVFrame* m_vSwsOutFrame;

    VideoSwsSpec* m_inputSpec;
    VideoSwsSpec* m_outputSpec;
};

#endif // VIDEORESCALE_H
