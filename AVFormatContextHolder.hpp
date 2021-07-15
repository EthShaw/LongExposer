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

#ifndef LONG_EXPOSER_AV_FORMAT_CONTEXT_HOLDER_HPP_INCLUDED
#define LONG_EXPOSER_AV_FORMAT_CONTEXT_HOLDER_HPP_INCLUDED

#include "AVPacketHolder.hpp"
extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
}
#include <string>

/// Automatically allocates an AVFormatContext on initialization and
/// calls avformat_close_input on it upon destruction. Throws an exception
/// if the allocation fails during initialization.
class AVFormatContextHolder
{
private:
    AVFormatContext* fmtContext = nullptr;

public:
    AVFormatContextHolder();
    ~AVFormatContextHolder();
    AVFormatContextHolder(const AVFormatContextHolder&) = delete;
    AVFormatContextHolder(AVFormatContextHolder&&);

    /// Returns a pointer to the AVFormatContext that this holder holds.
    AVFormatContext* GetContextPtr();

    // Calls avformat_open_input. Returns zero on success; returns the AVERROR and
    // frees the internal AVFormatContext on faliure.
    int TryOpenInput(std::string url, AVInputFormat* fmt, AVDictionary** options);

    // Calls avformat_open_input. Throws an exception on failure and frees the
    // internal AVFormatContext.
    void OpenInput(std::string url, AVInputFormat* fmt, AVDictionary** options);

    // Calls avformat_find_stream_info. Returns the AVERROR on faliure, >=0 on success.
    int TryFindStreamInfo(AVDictionary** options);

    // Calls avformat_find_stream_info. Throws an exception on failure.
    void FindStreamInfo(AVDictionary** options);

    // Reads a packet from the AVFormatContext and stores it in the AVPacketHolder
    // using av_read_frame. Returns 0 on success and < 0 on error or end of file.
    int ReadFrame(AVPacketHolder& frameHolder);
};

#endif // LONG_EXPOSER_AV_FORMAT_CONTEXT_HOLDER_HPP_INCLUDED
