#include "../../include/http/HttpResponse.h"

namespace http
{
    void HttpResponse::appendToBuffer(muduo::net::Buffer* output) const
    {
        //将HttpResponse对象的内容格式化为HTTP响应格式，并追加到output缓冲区中
        char buf[32];
        snprintf(buf,sizeof buf,"%s %d ",httpVersion_.c_str(),statusCode_);
        output->append(buf);
        if(!statusMessage_.empty())
        {
            output->append(statusMessage_);
        }
        output->append("\r\n");

        if(closeConnection_)
        {
            output->append("Connection: close\r\n");
        }
        else
        {
            output->append("Connection: keep-alive\r\n");
        }

        for(auto it = headers_.begin(); it != headers_.end(); ++it)
        {
            output->append(it->first);
            output->append(": ");
            output->append(it->second);
            output->append("\r\n");
        }

        output->append("\r\n");

        output->append(body_);
    }

    void HttpResponse::setStatusLine(
    const std::string& version,
    HttpStatusCode code,
    const std::string& message)
    {
        httpVersion_ = version;
        statusCode_ = code;
        statusMessage_ = message;
    }
}

