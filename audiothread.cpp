#include "audiothread.h"

#include "decode.h"
#include "audioresample.h"
#include "audiodevice.h"

#include <QDebug>

extern "C" {
#include <libavformat/avformat.h>
#include <libavutil/samplefmt.h>

}
#include "TimeBase.h"

#define MAX_LIST_SIZE 100

AudioThread::AudioThread()
{
    m_decode = nullptr;
    m_as = nullptr;
    m_ad = nullptr;

    m_StopStatus = false;
}

bool AudioThread::Init(AVCodecParameters *par, AVStream* stream, AudioSwrSpec *outSpec)
{
    bool re = true;

    m_stream = stream;

    // open decoder
    if (!m_decode)     m_decode = new Decode();
    int ret = m_decode->Open(par, stream);
    if (ret < 0) {
        re = false;
        qDebug() << "OpenDecode :" << ret;
    }

    // init audio resample
    if(!m_as)     m_as = new AudioReSample();
    ret = m_as->InitSwr(m_decode->GetAudioCodecSpec(), outSpec);
    if (ret < 0) {
        re = false;
        qDebug() << "InitSwr :" << ret;
    }

    // open audio device
    m_ad = AudioDevice::GetInterface();
    re = m_ad->Open();
    if (!re)
        return false;

#if 1
    outFile.setFileName("D:/out.pcm" );
    if (!outFile.open(QFile::WriteOnly)) {
        qDebug() << "file open error: test.pcm";
        return false;
    }
#endif
    return true;
}

void AudioThread::Push(AVPacket *pkt)
{
    if (!pkt)    return;

    while (!m_StopStatus) {
        mutex.lock();
        if (m_pktList.size() < MAX_LIST_SIZE)//如果已经等于最大值的时候应该继续轮询检测，不能丢弃该pkt
        {
            m_pktList.push_back(pkt);
            mutex.unlock();
            break;
        }
        mutex.unlock();
        msleep(10);
    }
}

double AudioThread::GetClockTime()
{
    return a_clock;
}

void AudioThread::run()
{
    while (!m_StopStatus) {

        if (!m_decode || !m_as)
            return;

        mutex.lock();

        if (m_pktList.empty()) {
            mutex.unlock();
//            msleep(100);
            continue;
        }

        AVPacket *pkt = m_pktList.front();
        m_pktList.pop_front();

        mutex.unlock();


        // 保存音频时钟
        if (pkt->pts != AV_NOPTS_VALUE)
            a_clock = av_q2d(m_stream->time_base) * pkt->pts;

        TimeBase::GetInterface().SetBaseTime(a_clock);

        int ret = m_decode->SendPkt(pkt);
        if(ret != 0)
            qDebug() << "audio SendPkt :" << ret;

        while (!m_StopStatus) {
            AVFrame* frame = m_decode->RecvFrame();
            if (!frame) break;
#if 1
            AVFrame *outFrame = m_as->AllocFrameData();
            if (!outFrame) {
                qDebug() << "av_frame_alloc error";
                return;
            }

            int size = m_as->SwrConvertByFrame(frame, outFrame);
            qDebug() << "RecvFrame size " << size;


            size_t linesize = frame->nb_samples * av_get_bytes_per_sample((AVSampleFormat)frame->format);
            qDebug() << "linesize:" << linesize;

//            outFile.write((char *)outFrame->data[0], size);
#endif
            while (!m_StopStatus) {
                bool re = m_ad->Write(outFrame->data[0], size);
                if (!re) {
                    msleep(1);
                    //cout << "Write Failed" << endl;
                    continue;
                }
                break;
            }
        }
    }
    outFile.close();
}
