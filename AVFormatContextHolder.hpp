#pragma once

#ifndef AV_FORMAT_CONTEXT_HOLDER_H__
#define AV_FORMAT_CONTEXT_HOLDER_H__

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

#endif // AV_FORMAT_CONTEXT_HOLDER_H__
