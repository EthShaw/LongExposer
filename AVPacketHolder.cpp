#include "AVPacketHolder.hpp"
#include "AVExceptions.hpp"

extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
}

AVPacketHolder::AVPacketHolder()
{
    packet = av_packet_alloc();

    if (packet == nullptr)
    {
        throw AVException("Failed to allocated memory for AVPacket.", AVERROR_UNKNOWN);
    }
}

AVPacketHolder::~AVPacketHolder()
{
    if (IsRefed())
    {
        Unref();
    }

    av_packet_free(&packet);
}

AVPacketHolder::AVPacketHolder(AVPacketHolder&& obj)
{
    packet = obj.packet;
    obj.packet = nullptr;
}

AVPacket* AVPacketHolder::GetPointer()
{
    return packet;
}

void AVPacketHolder::RequireUnref()
{
    isRefed = true;
}

void AVPacketHolder::Unref()
{
    av_packet_unref(packet);
}

bool AVPacketHolder::IsRefed() const
{
    return isRefed;
}
