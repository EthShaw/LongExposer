#include "FrameExtractor.hpp"
#include "AVFormatContextHolder.hpp"
#include "AVCodecContextHolder.hpp"
#include "RGBFrameConverter.hpp"
#include "AVFrameHolder.hpp"
#include "AVPacketHolder.hpp"
extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
}
#include <cstdio>
#include <memory>
#include <cstdint>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>

using std::string;
using std::unique_ptr;
using std::vector;

FrameExtractor::FrameExtractor(const string& inputFileName)
{
    fmtContext.OpenInput(inputFileName, nullptr, nullptr);
    fmtContext.FindStreamInfo(nullptr);

    AVFormatContext* fmtCtxPtr = fmtContext.GetContextPtr();

    AVCodec* pCodec = nullptr;
    AVCodecParameters* codecParams = nullptr;
    videoStreamIndex = -1;

    // loop though all the streams and print its main information
    for (int i = 0; i < fmtCtxPtr->nb_streams; i++)
    {
        AVCodecParameters* localCodecParams = fmtCtxPtr->streams[i]->codecpar;

        // finds the registered decoder for a codec ID
        // https://ffmpeg.org/doxygen/trunk/group__lavc__decoding.html#ga19a0ca553277f019dd5b0fec6e1f9dca
        AVCodec* localCodec = avcodec_find_decoder(localCodecParams->codec_id);

        // Skip codecs that cannot be decoded.
        if (localCodec == nullptr)
        {
            std::cout << "Warning: unsupported codec ID " << localCodecParams->codec_id
                      << " for stream " << i << ". If this is not the video stream,"
                      << " this can be ignored." << std::endl;
            continue;
        }

        // Use the first video stream we find.
        if (localCodecParams->codec_type == AVMEDIA_TYPE_VIDEO)
        {
            videoStreamIndex = i;
            pCodec = localCodec;
            codecParams = localCodecParams;

            std::cout << "Video Codec " << localCodec->name << " ID " << localCodec->id
                      << " bit rate " << localCodecParams->bit_rate
                      << " resolution " << localCodecParams->width << " x "
                      << localCodecParams->height << std::endl;

            break;
        }

        // print its name, id and bitrate
    }

    if (videoStreamIndex == -1)
    {
        std::ostringstream msgStream;
        msgStream << "File " << inputFileName << " does not contain a video stream!";
        throw std::runtime_error(msgStream.str());
    }

    // https://ffmpeg.org/doxygen/trunk/structAVCodecContext.html
    codecContext = std::make_unique<AVCodecContextHolder>(pCodec);

    // Fill the codec context based on the values from the supplied codec parameters
    // https://ffmpeg.org/doxygen/trunk/group__lavc__core.html#gac7b282f51540ca7a99416a3ba6ee0d16
    codecContext->ParametersToContext(codecParams);

    // Initialize the AVCodecContext to use the given AVCodec.
    // https://ffmpeg.org/doxygen/trunk/group__lavc__core.html#ga11f785a188d7d9df71621001465b0f1d
    codecContext->Open(pCodec, nullptr);
}

int FrameExtractor::NextFrame(AVFrameHolder& frameHolder)
{
    int error;

    while (true)
    {
        // Return decoded output data (into a frame) from a decoder
        // https://ffmpeg.org/doxygen/trunk/group__lavc__decoding.html#ga11e6542c4e66d3028668788a1a74217c
        error = codecContext->ReceiveFrame(frameHolder);

        if (error == AVERROR(EAGAIN) || error == AVERROR_EOF)
        {
            // Need to write another packet to the decoder

            // fill the Packet with data from the Stream
            // https://ffmpeg.org/doxygen/trunk/group__lavf__decoding.html#ga4fdb3084415a82e3810de6ee60e46a61

            while (true)
            {
                int pkt_error = fmtContext.ReadFrame(packetHolder);

                if (pkt_error < 0)
                {
                    // Failure or EOF
                    return pkt_error;
                }

                // if it's the video stream
                if (packetHolder.GetStreamIndex() == videoStreamIndex)
                {
                    // Supply raw packet data as input to a decoder
                    // https://ffmpeg.org/doxygen/trunk/group__lavc__decoding.html#ga58bc4bf1e0ac59e27362597e467efff3
                    pkt_error = codecContext->SendPacket(packetHolder);

                    if (pkt_error >= 0)
                    {
                        // Packet successfully read from the input and written to the decoder,
                        // so now we can read a frame from the decoder.
                        break;
                    }
                    else
                    {
                        //printf("Error while sending a packet to the decoder: %s", av_err2str(response));
                        // Failure
                        return pkt_error;
                    }
                }
            }
        }
        else
        {
            // Once a frame is successfully (or unsuccessfully in the case that error < 0)
            // read from the stream, break out of the loop.
            return error;
        }
    }
}
