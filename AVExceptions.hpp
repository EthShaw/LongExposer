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
#pragma once

#ifndef LONG_EXPOSER_AV_EXCEPTIONS_HPP_INCLUDED
#define LONG_EXPOSER_AV_EXCEPTIONS_HPP_INCLUDED

extern "C"
{
#include <libavutil/error.h>
}
#include <exception>
#include <string>
#include <sstream>
#include <memory>

struct AVException : public std::exception
{
    int avError;
    std::string whatMessage;

    AVException(const std::string& msgIn, int avErrorCode)
    {
        avError = avErrorCode;

        std::unique_ptr<char> avMsg = std::make_unique<char>(AV_ERROR_MAX_STRING_SIZE);
        av_strerror(avError, avMsg.get(), AV_ERROR_MAX_STRING_SIZE);

        std::ostringstream stream;
        stream << "An AVERROR occured: " << msgIn
               << " (av_strerror message: " << avMsg.get() << ")";

        whatMessage = stream.str();
    }

    const char* what() const throw()
    {
        return whatMessage.c_str();
    }
};

#endif // LONG_EXPOSER_AV_EXCEPTIONS_HPP_INCLUDED
