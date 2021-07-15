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
