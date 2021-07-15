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
