#include "FrameExtractor.hpp"
#include "AVFrameHolder.hpp"
#include "RGBFrameConverter.hpp"
extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
}
#include <cstdio>
#include <memory>
#include <cstdint>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>

using std::string;
using std::unique_ptr;
using std::vector;

static void saveImage(uint8_t* imgBytes, const string& filename, int width, int height)
{
    std::ofstream file;
    file.open(filename.c_str(), std::ios::out | std::ios::binary | std::ios::trunc);

    if (!file.is_open())
    {
        throw std::runtime_error("Could not open output file.");
    }

    // Write the PPM header (see https://en.wikipedia.org/wiki/Netpbm_format#PPM_example)
    file << "P6\n"
         << width << " " << height << "\n255\n";

    file.write(reinterpret_cast<char*>(imgBytes), width * height * 3);

    file.close();
}

// Returns an RGB frame
AVFrameHolder long_expose_image(string filename)
{
    FrameExtractor extractor(filename);
    RGBFrameConverter rgbConvert;
    int width = extractor.GetFrameWidth();
    int height = extractor.GetFrameHeight();

    vector<uint64_t> rsums(width * height, 0);
    vector<uint64_t> gsums(width * height, 0);
    vector<uint64_t> bsums(width * height, 0);

    int frameCount = 0;
    AVFrameHolder frameHolder;

    while (extractor.NextFrame(frameHolder) >= 0)
    {
        vector<uint8_t> rgbBytes = rgbConvert.ToRGBBytes(frameHolder);

        for (int i = 0; i < width * height; i++)
        {
            int bytePos = i * 3;
            rsums[i] += rgbBytes[bytePos];
            gsums[i] += rgbBytes[bytePos + 1];
            bsums[i] += rgbBytes[bytePos + 2];
        }
        frameCount++;
    }

    // TODO review https://stackoverflow.com/a/35682306/5515842 and consider what this
    // align parameter should be for different purposes
    AVFrameHolder outFrame = AVFrameHolder::AllocateImageFrame(width, height, AV_PIX_FMT_RGB24, 32);

    uint8_t* imgBytes = outFrame.GetPointer()->data[0];

    for (int i = 0; i < width * height; i++)
    {
        int bytePos = i * 3;
        imgBytes[bytePos] = rsums[i] / frameCount;
        imgBytes[bytePos + 1] = gsums[i] / frameCount;
        imgBytes[bytePos + 2] = bsums[i] / frameCount;
    }

    return outFrame;
}

int main(int argc, char** argv)
{
    if (argc < 3)
    {
        printf("Error: you must specify a file to squishify and an output name.\n");
        return -1;
    }

    string filename(argv[1]);
    string outputName(argv[2]);

    AVFrameHolder longExposure = long_expose_image(filename);
    uint8_t* imgBytes = longExposure.GetPointer()->data[0];

    saveImage(imgBytes, outputName, longExposure.GetWidth(), longExposure.GetHeight());

    std::cout << "releasing all the resources" << std::endl;
}

/*int main2(int argc, char **argv)
{
    if (argc < 3)
    {
        printf("Error: you must specify a file to squishify and an output name.\n");
        return -1;
    }

    AVFormatContext *fmtContext = avformat_alloc_context();

    if (!fmtContext)
    {
        puts("ERROR could not allocate an AVFormatContext.");
        return -1;
    }

    const char *filename = argv[1];
    const char *outputName = argv[2];

    if (avformat_open_input(&fmtContext, filename, nullptr, nullptr) != 0)
    {
        printf("Error: could not open the file.\n");
        return -1;
    }

    if (avformat_find_stream_info(fmtContext, nullptr) < 0)
    {
        printf("Error: failed to get the stream info.\n");
        return -1;
    }

    AVCodec *pCodec = nullptr;
    AVCodecParameters *codecParams = nullptr;
    int video_stream_index = -1;

    // loop though all the streams and print its main information
    for (int i = 0; i < fmtContext->nb_streams; i++)
    {
        AVCodecParameters *localCodecParams = nullptr;
        localCodecParams = fmtContext->streams[i]->codecpar;

        AVCodec *localCodec = nullptr;

        // finds the registered decoder for a codec ID
        // https://ffmpeg.org/doxygen/trunk/group__lavc__decoding.html#ga19a0ca553277f019dd5b0fec6e1f9dca
        localCodec = avcodec_find_decoder(localCodecParams->codec_id);

        if (localCodec == NULL)
        {
            printf("ERROR unsupported codec!\n");
            // In this example if the codec is not found we just skip it
            continue;
        }

        // when the stream is a video we store its index, codec parameters and codec
        if (localCodecParams->codec_type == AVMEDIA_TYPE_VIDEO)
        {
            if (video_stream_index == -1)
            {
                video_stream_index = i;
                pCodec = localCodec;
                codecParams = localCodecParams;
            }

            printf("Video Codec: resolution %d x %d\n", localCodecParams->width, localCodecParams->height);
        }
        else if (localCodecParams->codec_type == AVMEDIA_TYPE_AUDIO)
        {
            printf("Audio Codec: %d channels, sample rate %d\n", localCodecParams->channels, localCodecParams->sample_rate);
        }

        // print its name, id and bitrate
        printf("\tCodec %s ID %d bit_rate %" PRId64 "\n", localCodec->name, localCodec->id, localCodecParams->bit_rate);
    }

    if (video_stream_index == -1)
    {
        printf("File %s does not contain a video stream!\n", argv[1]);
        return -1;
    }

    // https://ffmpeg.org/doxygen/trunk/structAVCodecContext.html
    AVCodecContext *pCodecContext = avcodec_alloc_context3(pCodec);
    if (!pCodecContext)
    {
        printf("failed to allocated memory for AVCodecContext\n");
        return -1;
    }

    // Fill the codec context based on the values from the supplied codec parameters
    // https://ffmpeg.org/doxygen/trunk/group__lavc__core.html#gac7b282f51540ca7a99416a3ba6ee0d16
    if (avcodec_parameters_to_context(pCodecContext, codecParams) < 0)
    {
        printf("failed to copy codec params to codec context\n");
        return -1;
    }

    // Initialize the AVCodecContext to use the given AVCodec.
    // https://ffmpeg.org/doxygen/trunk/group__lavc__core.html#ga11f785a188d7d9df71621001465b0f1d
    if (avcodec_open2(pCodecContext, pCodec, NULL) < 0)
    {
        printf("failed to open codec through avcodec_open2\n");
        return -1;
    }

    int width = pCodecContext->width;
    int height = pCodecContext->height;

    // https://ffmpeg.org/doxygen/trunk/structAVFrame.html
    AVFrame *pFrame = av_frame_alloc();
    if (!pFrame)
    {
        printf("failed to allocated memory for AVFrame\n");
        return -1;
    }
    // https://ffmpeg.org/doxygen/trunk/structAVPacket.html
    AVPacket *pPacket = av_packet_alloc();
    if (!pPacket)
    {
        printf("failed to allocated memory for AVPacket\n");
        return -1;
    }

    int response = 0;
    //int how_many_packets_to_process = 8;

    uint64_t **rsums = nullptr;
    uint64_t **gsums = nullptr;
    uint64_t **bsums = nullptr;
    int frameCount = 0;

    // fill the Packet with data from the Stream
    // https://ffmpeg.org/doxygen/trunk/group__lavf__decoding.html#ga4fdb3084415a82e3810de6ee60e46a61
    while (av_read_frame(fmtContext, pPacket) >= 0)
    {
        // if it's the video stream
        if (pPacket->stream_index == video_stream_index)
        {
            //printf("AVPacket->pts %" PRId64, pPacket->pts);

            response = process_frame(pPacket, pCodecContext, pFrame, rsums, gsums, bsums, frameCount);

            if (response < 0)
                break;
        }
        // https://ffmpeg.org/doxygen/trunk/group__lavc__packet.html#ga63d5a489b419bd5d45cfd09091cbcbc2
        av_packet_unref(pPacket);
    }

    uint8_t *imgBytes = new uint8_t[width * height * 3];

    for (int i = 0; i < width; i++)
    {
        for (int j = 0; j < height; j++)
        {
            int pos = (i + j * width) * 3;
            imgBytes[pos] = rsums[i][j] / frameCount;
            imgBytes[pos + 1] = gsums[i][j] / frameCount;
            imgBytes[pos + 2] = bsums[i][j] / frameCount;
        }
    }

    saveImage(imgBytes, outputName, width, height);
    delete imgBytes;

    printf("releasing all the resources\n");

    avformat_close_input(&fmtContext);
    av_packet_free(&pPacket);
    av_frame_free(&pFrame);
    avcodec_free_context(&pCodecContext);
    return 0;
}*/
