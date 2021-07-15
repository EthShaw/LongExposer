cmake_minimum_required(VERSION 3.16.3)
project(LongExposer VERSION 0.1.0)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

include(CTest)
enable_testing()

add_executable(LongExposer main.cpp FrameExtractor.cpp FrameExtractor.hpp 
    AVCodecContextHolder.cpp AVCodecContextHolder.hpp
    AVFormatContextHolder.cpp AVFormatContextHolder.hpp
    RGBFrameConverter.cpp RGBFrameConverter.hpp
    AVPacketHolder.cpp AVPacketHolder.hpp
    AVFrameHolder.cpp AVFrameHolder.hpp
    AVExceptions.hpp)

set_target_properties(LongExposer PROPERTIES CXX_STANDARD 20)
target_compile_features(LongExposer PUBLIC cxx_std_20)

# REMEMBER TO ADD libavutil (avutil) TO THE DEPENDENCIES OR IT WON'T WORK
target_link_libraries(LongExposer avcodec avformat avutil swscale)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)