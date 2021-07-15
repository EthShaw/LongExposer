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
