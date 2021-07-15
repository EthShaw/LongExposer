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

#ifndef LONG_EXPOSER_AV_PACKET_HOLDER_HPP_INCLUDED
#define LONG_EXPOSER_AV_PACKET_HOLDER_HPP_INCLUDED

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

#endif // LONG_EXPOSER_AV_PACKET_HOLDER_HPP_INCLUDED