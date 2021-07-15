#include "AVFrameHolder.hpp"
extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
}
#include <vector>

class RGBFrameConverter
{
private:
    SwsContext* swsContext;

public:
    RGBFrameConverter();
    ~RGBFrameConverter();
    RGBFrameConverter(const RGBFrameConverter&) = delete;
    RGBFrameConverter(RGBFrameConverter&&);

    std::vector<uint8_t> ToRGBBytes(AVFrameHolder& frameHolder);
};
