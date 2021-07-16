cmake_minimum_required(VERSION 3.16.3)
project(LongExposer VERSION 0.1.0)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

include(CTest)
enable_testing()

add_executable(LongExposer ${CMAKE_CURRENT_LIST_DIR}/main.cpp 
    ${CMAKE_CURRENT_LIST_DIR}/FrameExtractor.cpp ${CMAKE_CURRENT_LIST_DIR}/FrameExtractor.hpp 
    ${CMAKE_CURRENT_LIST_DIR}/AVCodecContextHolder.cpp ${CMAKE_CURRENT_LIST_DIR}/AVCodecContextHolder.hpp
    ${CMAKE_CURRENT_LIST_DIR}/AVFormatContextHolder.cpp ${CMAKE_CURRENT_LIST_DIR}/AVFormatContextHolder.hpp
    ${CMAKE_CURRENT_LIST_DIR}/RGBFrameConverter.cpp ${CMAKE_CURRENT_LIST_DIR}/RGBFrameConverter.hpp
    ${CMAKE_CURRENT_LIST_DIR}/AVPacketHolder.cpp ${CMAKE_CURRENT_LIST_DIR}/AVPacketHolder.hpp
    ${CMAKE_CURRENT_LIST_DIR}/AVFrameHolder.cpp ${CMAKE_CURRENT_LIST_DIR}/AVFrameHolder.hpp
    ${CMAKE_CURRENT_LIST_DIR}/AVExceptions.hpp
    ${CMAKE_CURRENT_LIST_DIR}/thirdparty/argparse/argparse.hpp)

set_target_properties(LongExposer PROPERTIES CXX_STANDARD 20)
target_compile_features(LongExposer PUBLIC cxx_std_20)

# REMEMBER TO ADD libavutil (avutil) TO THE DEPENDENCIES OR IT WON'T WORK
target_link_libraries(LongExposer avcodec avformat avutil swscale)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
