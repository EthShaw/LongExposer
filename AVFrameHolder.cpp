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
#include "AVFrameHolder.hpp"
#include "AVExceptions.hpp"

extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/imgutils.h>
}

AVFrameHolder::AVFrameHolder()
{
    frame = av_frame_alloc();

    if (frame == nullptr)
    {
        throw AVException("Failed to allocated memory for AVFrame.", AVERROR_UNKNOWN);
    }
}

AVFrameHolder::AVFrameHolder(AVFrameHolder&& obj)
{
    frame = obj.frame;
    obj.frame = nullptr;
}

AVFrameHolder::~AVFrameHolder()
{
    av_frame_free(&frame);
}

AVFrameHolder AVFrameHolder::AllocateImageFrame(int width, int height, AVPixelFormat pixFormat, int bufAlign)
{
    AVFrameHolder frame;
    AVFrame* framePtr = frame.GetPointer();

    framePtr->width = width;
    framePtr->height = height;
    framePtr->format = pixFormat;

    int err = av_image_alloc(framePtr->data, framePtr->linesize, width, height, pixFormat, bufAlign);

    if (err < 0)
    {
        throw AVException("Unable to allocate the output frame.", err);
    }

    return frame;
}
