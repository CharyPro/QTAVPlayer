#include "videothread.h"

#include <QDebug>

#include "decode.h"
#include "videorescale.h"
#include "codecformatspec.h"
#include "IVideoDevice.h"
extern "C" {
#include <libavutil/imgutils.h>

}
#include "TimeBase.h"

#define MAX_LIST_SIZE 1000

static uint8_t *video_dst_data[4] = {NULL};
static int      video_dst_linesize[4];

VideoThread::VideoThread()
    :m_StopStatus(false)
{
    m_decode = nullptr;
    m_vs = nullptr;
}


bool VideoThread::Init(AVCodecParameters *par, VideoSwsSpec* outSpec, AVStream* stream, IVideoDevice* videoDevice)
{
    bool re = true;

    v_stream = stream;
    /// 打开解码器
    if (!m_decode)     m_decode = new Decode();
    int ret = m_decode->Open(par, stream);
    if (ret < 0) {
        re = false;
        qDebug() << "OpenDecode :" << ret;
    }

    /// 像素重采样  outSpec
    m_vs = new VideoReScale();
    ret = m_vs->InitSws(m_decode->GetVideoCodecSpec(), outSpec);
    if (ret < 0) {
        re = false;
        qDebug() << "InitSws :" << ret;
    }

    this->m_outSpec = outSpec;
    /// 显示窗口  videoDevice
    if (videoDevice) {
        this->m_videoDevice = videoDevice;
        videoDevice->Init(outSpec->width, outSpec->height);
    }


    // 打开文件
    file.setFileName("out.yuv");
    if (!file.open(QFile::WriteOnly)) {
        qDebug() << "file open error" ;

        // 关闭输入设备
        return false;
    }

    video_dst_file = fopen("out_2.yuv", "wb");

    imageSize = av_image_get_buffer_size(
                        (AVPixelFormat)outSpec->pixFmt,
                        outSpec->width, outSpec->height,
                        1);


    ret = av_image_alloc(video_dst_data, video_dst_linesize,
        m_outSpec->width, m_outSpec->height, (AVPixelFormat)m_outSpec->pixFmt, 1);
    video_dst_bufsize = ret;
    return re;
}

void VideoThread::Push(AVPacket *pkt)
{
    if (!pkt)    return;

//    m_mutex.lock();
//    if (m_pktList.size() < MAX_LIST_SIZE)//如果已经等于最大值的时候应该继续轮询检测，不能丢弃该pkt
//        m_pktList.push_back(pkt);

//    m_mutex.unlock();

    while (!m_StopStatus) {
        m_mutex.lock();
        if (m_pktList.size() < MAX_LIST_SIZE)//如果已经等于最大值的时候应该继续轮询检测，不能丢弃该pkt
        {
            m_pktList.push_back(pkt);
            m_mutex.unlock();
            break;
        }
        m_mutex.unlock();
        msleep(100);
    }
    qDebug() << "Push Success " << m_pktList.size();
}

void VideoThread::SetStop(bool status)
{
    m_StopStatus = status;
}

void VideoThread::SetSycClock(double a_clock)
{
    m_clocks = a_clock;
}

double VideoThread::GetClockTime()
{
    return _vTime;
}

void VideoThread::run()
{
    while (!m_StopStatus) {

        if (m_decode == nullptr || m_vs == nullptr)  return;


        m_mutex.lock();

        // if m_pktList is empty, then sleep and continue
        // TODO: change other ways to deal when m_pktList is empty
        if(m_pktList.empty()) {
            m_mutex.unlock();
//            msleep(2);
            continue;
        }

        qDebug() << "Video _pktList.size:" << m_pktList.size();

        AVPacket* pkt = m_pktList.front();
        m_pktList.pop_front();

        m_mutex.unlock();

        // 视频时钟
        if (pkt->dts != AV_NOPTS_VALUE) {
            _vTime = av_q2d(v_stream->time_base) * pkt->dts;
        }

        double time = TimeBase::GetInterface().GetBaseTime();

        while(_vTime > time) {
            time = TimeBase::GetInterface().GetBaseTime();
            msleep(10);
        }

        int ret = m_decode->SendPkt(pkt);
        if(ret != 0)
            qDebug() << "Video SendPkt :" << ret;

        qDebug() << "m_StopStatus " <<  m_StopStatus;
        while (!m_StopStatus) {

            AVFrame * frame = m_decode->RecvFrame();// receive frame after decode
            if (frame == nullptr) {
                break;
            }
            qDebug() << "Video RecvFrame " << "imageSize:" << imageSize;


//            av_image_copy(video_dst_data, video_dst_linesize,
//                          (const uint8_t **)(frame->data), frame->linesize,
//                          m_outSpec->pixFmt,m_outSpec->width, m_outSpec->height);

//            fwrite(video_dst_data[0], 1, video_dst_bufsize, video_dst_file);
//            AVFrame *outFrame = m_vs->AllocFrameData();
//            if (!outFrame) {
//                qDebug() << "av_frame_alloc error";
//                return;
//            }

//            ret = m_vs->SwsScale(frame, outFrame);
//            if (ret < 0)
//                continue;
            qDebug() << "Prepare Repaint";
            if(m_videoDevice)
                m_videoDevice->Repaint(frame);

//            for(int i = 0;i < frame->height;i++){
//                file.write((char *)(frame->data[0] + i * frame->linesize[0]), frame->width);
//            }

//            int loop = frame->height / 2;
//            int len_uv = frame->width / 2;

//            for(int i = 0;i < loop;i++){
//                file.write((char *)(frame->data[1] + i * frame->linesize[1]), len_uv);
//            }
//            for(int i = 0;i < loop;i++){
//                file.write((char *)(frame->data[2] + i * frame->linesize[2]), len_uv);
//            }
//           file.write((const char*)frame->data[0], imageSize);

        }
    }
    file.close();
}
