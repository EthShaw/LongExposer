#include "AVFormatContextHolder.hpp"
#include "AVPacketHolder.hpp"
#include "AVExceptions.hpp"
#include <string>
extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
}

AVFormatContextHolder::AVFormatContextHolder()
{
    fmtContext = avformat_alloc_context();

    if (fmtContext == nullptr)
    {
        throw AVException("ERROR could not allocate an AVFormatContext.", AVERROR_UNKNOWN);
    }
}

AVFormatContextHolder::~AVFormatContextHolder()
{
    avformat_close_input(&fmtContext);
}

AVFormatContextHolder::AVFormatContextHolder(AVFormatContextHolder&& obj)
{
    fmtContext = obj.fmtContext;
    obj.fmtContext = nullptr;
}

AVFormatContext* AVFormatContextHolder::GetContextPtr()
{
    return fmtContext;
}

int AVFormatContextHolder::TryOpenInput(std::string url, AVInputFormat* fmt, AVDictionary** options)
{
    return avformat_open_input(&fmtContext, url.c_str(), fmt, options);
}

void AVFormatContextHolder::OpenInput(std::string url, AVInputFormat* fmt, AVDictionary** options)
{
    int error = TryOpenInput(url, fmt, options);

    if (error != 0)
    {
        throw AVException("Error: could not open the file.", error);
    }
}

int AVFormatContextHolder::TryFindStreamInfo(AVDictionary** options)
{
    return avformat_find_stream_info(fmtContext, nullptr);
}

void AVFormatContextHolder::FindStreamInfo(AVDictionary** options)
{
    int error = TryFindStreamInfo(options);

    if (error < 0)
    {
        throw AVException("Error: failed to get the stream info.", error);
    }
}

int AVFormatContextHolder::ReadFrame(AVPacketHolder& pktHolder)
{
    if (pktHolder.IsRefed())
    {
        pktHolder.Unref();
    }

    AVPacket* pkt = pktHolder.GetPointer();
    int error = av_read_frame(fmtContext, pkt);

    if (error == 0)
    {
        // Unref will need to be called since av_read_frame read a frame
        pktHolder.RequireUnref();
    }

    return error;
}
