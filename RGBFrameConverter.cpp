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
#include "RGBFrameConverter.hpp"
#include "AVFrameHolder.hpp"

extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
}
#include <vector>

using std::vector;

RGBFrameConverter::RGBFrameConverter()
{
    swsContext = nullptr;
}

vector<uint8_t> RGBFrameConverter::ToRGBBytes(AVFrameHolder& frameHolder)
{
    AVFrame* frame = frameHolder.GetPointer();
    int width = frame->width;
    int height = frame->height;

    swsContext = sws_getCachedContext(swsContext, width, height, (AVPixelFormat)frame->format, width, height,
                                      AV_PIX_FMT_RGB24, SWS_BILINEAR, nullptr, nullptr, nullptr);

    vector<uint8_t> rgbBytes(width * height * 3);
    int stride[1] = {width * 3};
    uint8_t* const bufRef = &rgbBytes.front();

    sws_scale(swsContext, frame->data, frame->linesize, 0, height, &bufRef, stride);

    return rgbBytes;
}

RGBFrameConverter::~RGBFrameConverter()
{
    sws_freeContext(swsContext);
}

RGBFrameConverter::RGBFrameConverter(RGBFrameConverter&& obj)
{
    swsContext = obj.swsContext;
    obj.swsContext = nullptr;
}
