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
#include "AVFormatContextHolder.hpp"
#include "AVPacketHolder.hpp"
#include "AVExceptions.hpp"
#include <string>
extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
}

AVFormatContextHolder::AVFormatContextHolder()
{
    fmtContext = avformat_alloc_context();

    if (fmtContext == nullptr)
    {
        throw AVException("ERROR could not allocate an AVFormatContext.", AVERROR_UNKNOWN);
    }
}

AVFormatContextHolder::~AVFormatContextHolder()
{
    avformat_close_input(&fmtContext);
}

AVFormatContextHolder::AVFormatContextHolder(AVFormatContextHolder&& obj)
{
    fmtContext = obj.fmtContext;
    obj.fmtContext = nullptr;
}

AVFormatContext* AVFormatContextHolder::GetContextPtr()
{
    return fmtContext;
}

int AVFormatContextHolder::TryOpenInput(std::string url, AVInputFormat* fmt, AVDictionary** options)
{
    return avformat_open_input(&fmtContext, url.c_str(), fmt, options);
}

void AVFormatContextHolder::OpenInput(std::string url, AVInputFormat* fmt, AVDictionary** options)
{
    int error = TryOpenInput(url, fmt, options);

    if (error != 0)
    {
        throw AVException("Error: could not open the file.", error);
    }
}

int AVFormatContextHolder::TryFindStreamInfo(AVDictionary** options)
{
    return avformat_find_stream_info(fmtContext, nullptr);
}

void AVFormatContextHolder::FindStreamInfo(AVDictionary** options)
{
    int error = TryFindStreamInfo(options);

    if (error < 0)
    {
        throw AVException("Error: failed to get the stream info.", error);
    }
}

int AVFormatContextHolder::ReadFrame(AVPacketHolder& pktHolder)
{
    if (pktHolder.IsRefed())
    {
        pktHolder.Unref();
    }

    AVPacket* pkt = pktHolder.GetPointer();
    int error = av_read_frame(fmtContext, pkt);

    if (error == 0)
    {
        // Unref will need to be called since av_read_frame read a frame
        pktHolder.RequireUnref();
    }

    return error;
}
