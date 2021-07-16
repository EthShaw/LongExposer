# Long Exposer
This is program uses the frames in a video to create a single image with long-exposure type effects (and some other interesting effects). In its default mode, it finds the average R, G, and B components for each pixel across all frames of the input video and creates a single `ppm` image from the averages. (See the [Wikipedia article](https://en.wikipedia.org/wiki/Netpbm#PPM_example) on the PPM format for more information.) The program can be used to create things like a "silky waterfall" effect that you could get from a long-exposure photo, hence the name.

# License and thirdparty software

This software is licensed under the MIT License.

This software makes use of libraries from the (amazing) <a href=http://ffmpeg.org>FFmpeg</a> project, which is a thirdparty library licensed under the <a href=http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html>LGPLv2.1</a> (with certain optional components under the GPL). See [http://ffmpeg.org/](http://ffmpeg.org/) for more details on the project.

This program also makes use of the extremely helpful <a href="http://github.com/p-ranav/argparse">p-ranav/argparse (Link to Repository)</a>, an argument parser for modern C++ by Pranav Srinivas Kumar. (Licensed under the MIT license. See [thirdparty/argparser/argparser.hpp](thirdparty/argparser/argparser.hpp) for more information.)

# Compiling the project
The project can compiled on linux systems by running `./configure` and `cmake --build build`. It requires FFmpeg libraries to be installed. In theory, the project can be compiled on Windows, but you will have to supply your own FFmpeg binaries. If you intend to redistribute compiled binaries of this project, ensure compliance with the FFmpeg licensing. To ensure that the built binary is still licensed under the MIT license and not the LGPL or GPL, you will need to ensure that the project is dynamically linked to FFmpeg and that no GPL components are enabled in the FFmpeg build you are using. If you are only using the project for personal use and not redistributing it, this should not be an issue anyway. For more information about FFmpeg licensing, see [https://www.ffmpeg.org/legal.html](https://www.ffmpeg.org/legal.html). **Disclaimer: I am not a lawyer and this is not legal advice. Furthermore, I am not affiliated with the FFmpeg project in any way.**

# Usage

The program arguments are as follows:
```
./LongExposer [options] input_file output_file
```

You can run it on the example file after compiling with the following command (from the project root):
```
./build/LongExposure example.mp4 example.ppm
```

The `ppm` formatted image can be converted to more widely usable image formats using a thirdparty program. Here is an example using the commonly available tool FFmpeg:
```
ffmpeg -i ppm_file.ppm jpeg_file.jpg
```

The program primarily works for short videos where the camera taking the video is in a fixed location, such as a tripod. However, I have had a fair amount of success running videos through stabilizers, such as the FFmpeg vidstab filter, before using them. An example use of the vidstab filter in tripod mode is below:
```
ffmpeg -i input_file.mp4 -vf vidstabdetect=tripod=1 -f null -
ffmpeg -i input_file.mp4 -vf vidstabtransform=tripod=1 stabilized.mp4
```

### Additional command-line options
**--mode \(short form: -m\) \[default value: average\]:** Set the mode of the program. The average mode is the one I have used most, but the others can create some very interesting effects. The following are possible modes:
 - **average** [default] - Average all of the frames together by averaging each of the RGB color components of each pixel across all frames. Useful for creating interesting "long exposure" types of effects with water.
 - **brightest** - Uses the highest value for each color component for each pixel across the whole video (i.e., uses the highest R component of a pixel across all frames, the highest G component across all frames, and the highest B component across all frames). This can also create some interesting effects (note that this does not use the brightest value for the entire _pixel_, but the brightest value for each _component_ of each pixel).
 - **sum** - sums all of the R, G, and B components for each pixel across all frames (capping the value at 255).
 - **sum-scaled** - sums all of the R, G, and B components for each pixel across all frames, but rather than capping each component individually, the highest component of a pixel is capped at 255 and the lower components are scaled equivalently (in other words, each component sum of a pixel is multiplied by a "scale value" equal to 255/highest_component_sum_value).

**--intensity \(short form: -i\) \[default value: 1.0\]:** A decimal value between 0.0 and 1.0 (inclusive) used to scale the intensity of the image. For "brightest" mode, each component of each pixel is multiplied by this value. For other modes, each sum for each component of each pixel is multiplied by this value _before_ any other operations (capping/scaling/averaging the pixels). For example, using the "sum" mode with an intensity of (1/video_frame_count) is the equivalent of the "average" mode.

The following example applies an intensity of 0.8 in sum mode:
```
./LongExposure -i 0.8 -m sum input.mp4 output.ppm
```
