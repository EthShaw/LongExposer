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
#include "thirdparty/argparse/argparse.hpp"
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

static const string MODE_AVERAGE = "average";
static const string MODE_BRIGHTEST = "brightest";
static const string MODE_SUM = "sum";
static const string MODE_SUM_SCALED = "sum-scaled";

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
AVFrameHolder long_expose_image(string filename, const string mode, const double intensity)
{
    if (mode != MODE_AVERAGE && mode != MODE_SUM && mode != MODE_SUM_SCALED && mode != MODE_BRIGHTEST)
    {
        throw std::invalid_argument("The mode argument has an invalid value of " + mode);
    }
    if (intensity < 0 || intensity > 1)
    {
        throw std::invalid_argument("Argument `intensity` must be between 0.0 and 1.0");
    }

    FrameExtractor extractor(filename);
    RGBFrameConverter rgbConvert;
    int width = extractor.GetFrameWidth();
    int height = extractor.GetFrameHeight();

    vector<uint64_t> rsums(width * height, 0);
    vector<uint64_t> gsums(width * height, 0);
    vector<uint64_t> bsums(width * height, 0);

    int frameCount = 0;
    AVFrameHolder frameHolder;
    bool summing = mode != MODE_BRIGHTEST;

    while (extractor.NextFrame(frameHolder) >= 0)
    {
        vector<uint8_t> rgbBytes = rgbConvert.ToRGBBytes(frameHolder);

        for (int i = 0; i < width * height; i++)
        {
            int bytePos = i * 3;

            if (summing)
            {
                rsums[i] += rgbBytes[bytePos];
                gsums[i] += rgbBytes[bytePos + 1];
                bsums[i] += rgbBytes[bytePos + 2];
            }
            else
            {
                rsums[i] = std::max(rgbBytes[bytePos], (uint8_t)rsums[i]);
                gsums[i] = std::max(rgbBytes[bytePos + 1], (uint8_t)gsums[i]);
                bsums[i] = std::max(rgbBytes[bytePos + 2], (uint8_t)bsums[i]);
            }
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

        // Scale the values before using them
        rsums[i] = (uint64_t)std::round(rsums[i] * intensity);
        gsums[i] = (uint64_t)std::round(gsums[i] * intensity);
        bsums[i] = (uint64_t)std::round(bsums[i] * intensity);

        if (mode == MODE_AVERAGE)
        {
            imgBytes[bytePos] = rsums[i] / frameCount;
            imgBytes[bytePos + 1] = gsums[i] / frameCount;
            imgBytes[bytePos + 2] = bsums[i] / frameCount;
        }
        else if (mode == MODE_SUM)
        {
            imgBytes[bytePos] = std::min(rsums[i], (uint64_t)255);
            imgBytes[bytePos + 1] = std::min(gsums[i], (uint64_t)255);
            imgBytes[bytePos + 2] = std::min(bsums[i], (uint64_t)255);
        }
        else if (mode == MODE_SUM_SCALED)
        {
            double scaleVal = 255.0 / std::max({rsums[i], gsums[i], bsums[i]});
            if (scaleVal > 1)
            {
                // None of the components are greater than 255
                scaleVal = 1;
            }
            imgBytes[bytePos] = (uint8_t)std::round(rsums[i] * scaleVal);
            imgBytes[bytePos + 1] = (uint8_t)std::round(gsums[i] * scaleVal);
            imgBytes[bytePos + 2] = (uint8_t)std::round(bsums[i] * scaleVal);
        }
        else if (mode == MODE_BRIGHTEST)
        {
            imgBytes[bytePos] = (uint8_t)rsums[i];
            imgBytes[bytePos + 1] = (uint8_t)gsums[i];
            imgBytes[bytePos + 2] = (uint8_t)bsums[i];
        }
    }

    return outFrame;
}

int main(int argc, char** argv)
{
    static const string INPUT_FILE_ARG = "input_file";
    static const string OUTPUT_FILE_ARG = "output_file";
    static const string MODE_ARG = "--mode";
    static const string INTENSITY_ARG = "--intensity";

    argparse::ArgumentParser program("LongExposer");
    program.add_argument(INPUT_FILE_ARG)
        .help("The name of the video file to take as an input.");
    program.add_argument(OUTPUT_FILE_ARG)
        .help("The name to give to the generated output PPM file.");

    program.add_argument(MODE_ARG, "-m")
        .help("The mode for the program to operate in. See Readme.md for more details.")
        .default_value(MODE_AVERAGE)
        .action([](const string& value)
                {
                    static const vector<string> choices = {MODE_AVERAGE, MODE_BRIGHTEST,
                                                           MODE_SUM, MODE_SUM_SCALED};
                    if (std::find(choices.begin(), choices.end(), value) != choices.end())
                    {
                        return value;
                    }
                    throw std::runtime_error("Invalid value for '" + MODE_ARG + "' argument: " + value);
                });

    program.add_argument(INTENSITY_ARG, "-i")
        .help("Scale the intensity of the image, applied before mode operations. See Readme for how this affects different modes.")
        .default_value(1.0)
        .action([](const string& value)
                {
                    double num;
                    bool failed = false;
                    try
                    {
                        num = std::stod(value);
                    }
                    catch (const std::invalid_argument&)
                    {
                        failed = true;
                    }
                    catch (const std::out_of_range&)
                    {
                        failed = true;
                    }

                    if (num < 0 || num > 1)
                    {
                        failed = true;
                    }

                    if (failed)
                    {
                        throw std::runtime_error("Argument '" + INTENSITY_ARG + "' must be a number between 0.0 and 1.0");
                    }

                    return num;
                });

    try
    {
        program.parse_args(argc, argv);
    }
    catch (const std::runtime_error& err)
    {
        std::cout << err.what() << "\n"
                  << program;
        return 0;
    }

    string filename = program.get<string>(INPUT_FILE_ARG);
    string outputName = program.get<string>(OUTPUT_FILE_ARG);
    string mode = program.get<string>(MODE_ARG);
    double intensity = program.get<double>(INTENSITY_ARG);

    AVFrameHolder longExposure = long_expose_image(filename, mode, intensity);
    uint8_t* imgBytes = longExposure.GetPointer()->data[0];

    saveImage(imgBytes, outputName, longExposure.GetWidth(), longExposure.GetHeight());

    std::cout << "Done!" << std::endl;
}
