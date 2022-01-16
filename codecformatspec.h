#ifndef CODECFORMATSPEC_H
#define CODECFORMATSPEC_H
extern "C" {
#include <libavformat/avformat.h>
}

struct AudioSwrSpec {
    int sampleRate;
    AVSampleFormat sampleFmt;
    int chLayout;
    int chs;
    int bytesPerSampleFrame;
};

struct VideoSwsSpec {
    int width = 0;
    int height = 0;
    AVPixelFormat pixFmt = AV_PIX_FMT_YUV420P;
    int size;
};
#endif // CODECFORMATSPEC_H
