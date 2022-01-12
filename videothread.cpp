#include "videothread.h"

#include <QDebug>

#include "decode.h"
#include "videorescale.h"
#include "codecformatspec.h"
#include "IVideoDevice.h"

#define MAX_LIST_SIZE 100


VideoThread::VideoThread()
    :m_StopStatus(false)
{
    m_decode = nullptr;
}


bool VideoThread::Init(AVCodecParameters *par, VideoSwsSpec* outSpec, IVideoDevice* videoDevice)
{
    bool re = true;

    /// 打开解码器
    if (!m_decode)     m_decode = new Decode();
    int ret = m_decode->Open(par);
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

    /// 显示窗口  videoDevice
    if (videoDevice) {
        this->m_videoDevice = videoDevice;
        videoDevice->Init(outSpec->width, outSpec->height);
    }

    return re;
}

void VideoThread::Push(AVPacket *pkt)
{
    if (!pkt)    return;

    while (!m_StopStatus) {
        m_mutex.lock();
        if (m_pktList.size() < MAX_LIST_SIZE)//如果已经等于最大值的时候应该继续轮询检测，不能丢弃该pkt
        {
            m_pktList.push_back(pkt);
            m_mutex.unlock();
            break;
        }
        m_mutex.unlock();
        msleep(10);
    }
}

void VideoThread::run()
{
    while (!m_StopStatus) {

        if (!m_decode || !m_vs)  return;

        m_mutex.lock();

        // if m_pktList is empty, then sleep and continue
        // TODO: change other ways to deal when m_pktList is empty
        if(m_pktList.empty()) {
            m_mutex.unlock();
            msleep(10);
            continue;
        }

        qDebug() << "Video _pktList.size:" << m_pktList.size();

        AVPacket* pkt = m_pktList.front();
        m_pktList.pop_front();

        m_mutex.unlock();

        int ret = m_decode->SendPkt(pkt);
        if(ret != 0)
            qDebug() << "Video SendPkt :" << ret;

        while (!m_StopStatus) {

            AVFrame * frame = m_decode->RecvFrame();// receive frame after decode
            if (!frame) break;
            qDebug() << "Video RecvFrame ";

            AVFrame *outFrame = m_vs->AllocFrameData();
            if (!outFrame) {
                qDebug() << "av_frame_alloc error";
                return;
            }

            ret = m_vs->SwsScale(frame, outFrame);
            if (ret < 0)
                continue;
            qDebug() << "Prepare Repaint";
//            if(m_videoDevice)
//                m_videoDevice->Repaint(outFrame);
        }
    }
}
