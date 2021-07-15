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

#ifndef LONG_EXPOSER_AV_FRAME_HOLDER_HPP_INCLUDED
#define LONG_EXPOSER_AV_FRAME_HOLDER_HPP_INCLUDED

extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
}

class AVFrameHolder
{
private:
    AVFrame* frame;

public:
    AVFrameHolder();
    ~AVFrameHolder();
    AVFrameHolder(const AVFrameHolder&) = delete;
    AVFrameHolder(AVFrameHolder&&);

    inline AVFrame* GetPointer() const;
    inline int GetWidth() const;
    inline int GetHeight() const;

    // Allocates an AVFrame with the given parameters, passing them to av_image_alloc
    static AVFrameHolder AllocateImageFrame(int width, int height, AVPixelFormat pixFormat, int bufAlign);
};

inline AVFrame* AVFrameHolder::GetPointer() const
{
    return frame;
}

inline int AVFrameHolder::GetWidth() const
{
    return frame->width;
}

inline int AVFrameHolder::GetHeight() const
{
    return frame->height;
}

#endif // LONG_EXPOSER_AV_FRAME_HOLDER_HPP_INCLUDED