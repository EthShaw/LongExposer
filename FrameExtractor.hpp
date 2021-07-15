#pragma once

#ifndef FRAME_EXTRACTOR_H__
#define FRAME_EXTRACTOR_H__

#include "AVFormatContextHolder.hpp"
#include "AVCodecContextHolder.hpp"
#include "AVFrameHolder.hpp"
extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
}
#include <vector>
#include <string>
#include <memory>

struct AVFormatDeleter
{
    void operator()(AVFormatContext* ctx)
    {
        avformat_close_input(&ctx);
    }
};

class FrameExtractor
{
private:
    AVFormatContextHolder fmtContext;
    AVPacketHolder packetHolder;
    std::unique_ptr<AVCodecContextHolder> codecContext;
    int videoStreamIndex;
    FrameExtractor(const FrameExtractor&) = delete;
    FrameExtractor(FrameExtractor&&) = delete;

    //uint8_t *YUVToRGB(AVFrame *frame);
    int process_frame(AVPacket* pPacket, AVCodecContext* pCodecContext, AVFrame* pFrame,
                      std::vector<uint64_t>& rsums, std::vector<uint64_t>& gsums, std::vector<uint64_t>& bsums,
                      int& frameCount);

public:
    FrameExtractor(const std::string& inputFile);
    // Reads the next frame and puts it in the frame holder. Returns
    // 0 on success and less than 0 on error or EOF.
    int NextFrame(AVFrameHolder& frameHolder);
    // Gets the width of frames from the codec
    inline int GetFrameWidth();
    // Gets the height of frames from the codec
    inline int GetFrameHeight();
};

inline int FrameExtractor::GetFrameWidth()
{
    return codecContext->GetWidth();
}

inline int FrameExtractor::GetFrameHeight()
{
    return codecContext->GetHeight();
}

#endif //FRAME_EXTRACTOR_H__
