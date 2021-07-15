#pragma once

#ifndef AV_CODEC_CONTEXT_HOLDER_H__
#define AV_CODEC_CONTEXT_HOLDER_H__

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

#endif // AV_CODEC_CONTEXT_HOLDER_H__
