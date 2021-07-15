/**
 * MIT License
 * 
 * Copyright (c) 2021 Ethan Shaw
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#pragma once

#ifndef LONG_EXPOSER_FRAME_EXTRACTOR_HPP_INCLUDED
#define LONG_EXPOSER_FRAME_EXTRACTOR_HPP_INCLUDED

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

#endif //LONG_EXPOSER_FRAME_EXTRACTOR_HPP_INCLUDED
