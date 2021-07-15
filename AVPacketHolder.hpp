#pragma once

#ifndef AV_PACKET_HOLDER_H__
#define AV_PACKET_HOLDER_H__

extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
}

class AVPacketHolder
{
private:
    AVPacket* packet;
    // Whether or not av_packet_unref needs to be called on the packet.
    bool isRefed;

public:
    AVPacketHolder();
    ~AVPacketHolder();
    AVPacketHolder(const AVPacketHolder&) = delete;
    AVPacketHolder(AVPacketHolder&&);

    // Allows direct interaction with the pointer
    AVPacket* GetPointer();
    // Tell the packet holder it will need to unref the packet on
    // free or before the next read. This is used if any actions
    // were taken on the AVPacket* directly, outside of the holder
    // wrapper (which automatically tracks refing and unrefing).
    void RequireUnref();
    // Whether or not Unref() needs to be called
    bool IsRefed() const;
    // Calls av_packet_unref on the packet
    void Unref();
    // Gets the stream_index of the packet
    inline int GetStreamIndex();
};

inline int AVPacketHolder::GetStreamIndex()
{
    return packet->stream_index;
}

#endif // AV_PACKET_HOLDER_H__