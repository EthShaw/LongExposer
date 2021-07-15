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
#include "FrameExtractor.hpp"
#include "AVFrameHolder.hpp"
#include "RGBFrameConverter.hpp"
extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
}
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
        std::cerr << "Error: you must specify a file to squishify and an output name.\n\n"
                  << "Program arguments:\n"
                  << argv[0] << " input_file output_file" << std::endl;
        return -1;
    }

    string filename(argv[1]);
    string outputName(argv[2]);

    AVFrameHolder longExposure = long_expose_image(filename);
    uint8_t* imgBytes = longExposure.GetPointer()->data[0];

    saveImage(imgBytes, outputName, longExposure.GetWidth(), longExposure.GetHeight());

    std::cout << "Done!" << std::endl;
}
