# Long Exposer
This is program takes the average of all of the frames in a video. It essentially finds the average R, G, and B components for each pixel across all frames of a video and creates a single `ppm` image from the averages. (See the [Wikipedia article](https://en.wikipedia.org/wiki/Netpbm#PPM_example) on the PPM format for more information.) The program can be used to create things like a "silky waterfall" effect that you could get from a long exposure photo, hence the name.

# License and thirdparty software

This software is licensed under the MIT License.

This software makes use of libraries from the (amazing) <a href=http://ffmpeg.org>FFmpeg</a> project, which is a thirdparty library licensed under the <a href=http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html>LGPLv2.1</a> (with certain optional components under the GPL). See [http://ffmpeg.org/](http://ffmpeg.org/) for more details on the project.

# Compiling the project
The project can compiled on linux by running `./configure` and `cmake --build build`. It requires FFmpeg libraries to be installed. In theory, the project can be compiled on Windows, but you will have to supply your own FFmpeg binaries. If you intend to redistribute compiled binaries of this project, ensure compliance with the FFmpeg licensing. To ensure that the built binary is still licensed under the MIT license and not the LGPL or GPL, ensure that the project is dynamically linked to FFmpeg and that no GPL components are enabled in the FFmpeg build you are using. If you are only using the project for personal use and not redistributing it, this should not be an issue anyway. For more information about FFmpeg licensing, see [https://www.ffmpeg.org/legal.html](https://www.ffmpeg.org/legal.html). **Disclaimer: I am not a lawyer and this is not legal advice. Furthermore, I am not affiliated with the FFmpeg project in any way.**

# Usage

The program arguments are as follows:
```
./LongExposer input_file output_file
```

You can run it on the example file after compiling with the following command (from the project root):
```
./build/LongExposure example.mp4 example.ppm
```

The `ppm` formatted image can be converted to more friendly image formats using a thirdparty program. Here is an example using the commonly available tool FFmpeg:
```
ffmpeg -i ppm_file.ppm jpeg_file.jpg
```

The program primarily works for short videos where the camera taking the video is in a fixed location, such as a tripod. However, I have had a fair amount of success running videos through stabilizers, such as the FFmpeg vidstab filter, before using them. An example use of the vidstab filter in tripod mode is below:
```
ffmpeg -i input_file.mp4 -vf vidstabdetect=tripod=1 -f null -
ffmpeg -i input_file.mp4 -vf vidstabtransform=tripod=1 stabilized.mp4
```
