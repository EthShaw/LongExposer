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
#include "AVCodecContextHolder.hpp"
#include "AVFrameHolder.hpp"
#include "AVPacketHolder.hpp"
#include "AVExceptions.hpp"
extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
}
#include <sstream>

AVCodecContextHolder::AVCodecContextHolder(const AVCodec* codec)
{
    codecContext = avcodec_alloc_context3(codec);

    if (codecContext == nullptr)
    {
        throw AVException("Failed to allocated memory for AVCodecContext!", AVERROR_UNKNOWN);
    }
}

AVCodecContextHolder::~AVCodecContextHolder()
{
    avcodec_free_context(&codecContext);
}

AVCodecContextHolder::AVCodecContextHolder(AVCodecContextHolder&& obj)
{
    codecContext = obj.codecContext;
    obj.codecContext = nullptr;
}

AVCodecContext* AVCodecContextHolder::GetContextPtr()
{
    return codecContext;
}

int AVCodecContextHolder::TryParametersToContext(const AVCodecParameters* pars)
{
    return avcodec_parameters_to_context(codecContext, pars);
}

void AVCodecContextHolder::ParametersToContext(const AVCodecParameters* pars)
{
    int error = TryParametersToContext(pars);

    if (error < 0)
    {
        throw AVException("Failed to copy codec params to codec context!", error);
    }
}

int AVCodecContextHolder::TryOpen(const AVCodec* codec, AVDictionary** options)
{
    return avcodec_open2(codecContext, codec, options);
}

void AVCodecContextHolder::Open(const AVCodec* codec, AVDictionary** options)
{
    int error = TryOpen(codec, options);

    if (error != 0)
    {
        throw AVException("Failed to open codec through avcodec_open2.", error);
    }
}

int AVCodecContextHolder::ReceiveFrame(AVFrameHolder& frameHolder)
{
    return avcodec_receive_frame(codecContext, frameHolder.GetPointer());
}

int AVCodecContextHolder::SendPacket(AVPacketHolder& packetHolder)
{
    return avcodec_send_packet(codecContext, packetHolder.GetPointer());
}
