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
