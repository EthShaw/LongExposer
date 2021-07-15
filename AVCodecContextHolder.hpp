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

#ifndef LONG_EXPOSER_AV_CODEC_CONTEXT_HOLDER_HPP_INCLUDED
#define LONG_EXPOSER_AV_CODEC_CONTEXT_HOLDER_HPP_INCLUDED

#include "AVFrameHolder.hpp"
#include "AVPacketHolder.hpp"
extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
}

/// Automatically allocates an AVFormatContext on initialization and
/// calls avformat_close_input on it upon destruction. Throws an exception
/// if the allocation fails during initialization.
class AVCodecContextHolder
{
private:
    AVCodecContext* codecContext = nullptr;

public:
    AVCodecContextHolder(const AVCodec* codec);
    ~AVCodecContextHolder();
    AVCodecContextHolder(const AVCodecContextHolder&) = delete;
    AVCodecContextHolder(AVCodecContextHolder&&);

    /// Returns a pointer to the AVCodecContext that this holder holds.
    AVCodecContext* GetContextPtr();

    // Calls avcodec_parameters_to_context. Returns the AVERROR on faliure, >=0 on success.
    int TryParametersToContext(const AVCodecParameters* pars);

    // Calls avcodec_parameters_to_context. Throws an exception on failure.
    void ParametersToContext(const AVCodecParameters* pars);

    // Get the width for a video codec
    inline int GetWidth();

    // Get the height for a video codec
    inline int GetHeight();

    // Call avcodec_open2 on the internal AVCodecContext. Returns 0 on success,
    // a negative value (the AVERROR) on failure.
    int TryOpen(const AVCodec* codec, AVDictionary** options);

    // Call avcodec_open2 on the internal AVCodecContext. Throws an exception
    // on failure.
    void Open(const AVCodec* codec, AVDictionary** options);

    // Call avcodec_receive_frame on the internal AVCodecContext with the provided
    // frameHolder and return the resulting return value.
    int ReceiveFrame(AVFrameHolder& frameHolder);

    // Call avcodec_send_packet on the internal AVCodecContext with the provided
    // frameHolder and return the resulting return value.
    int SendPacket(AVPacketHolder& packetHolder);
};

inline int AVCodecContextHolder::GetWidth()
{
    return codecContext->width;
}

inline int AVCodecContextHolder::GetHeight()
{
    return codecContext->height;
}

#endif // LONG_EXPOSER_AV_CODEC_CONTEXT_HOLDER_HPP_INCLUDED
