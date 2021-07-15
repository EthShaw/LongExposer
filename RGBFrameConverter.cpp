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
