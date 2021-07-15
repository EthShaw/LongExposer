#pragma once

#ifndef AV_FRAME_HOLDER_H__
#define AV_FRAME_HOLDER_H__

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

#endif // AV_FRAME_HOLDER_H__