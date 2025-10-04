#pragma once

#include <iostream>

#include <muduo/net/TcpServer.h>

#include "HttpRequest.h"

namespace http
{
class HttpContext
{
public:
    enum HttpRequestParseState
    {
        kExpectRequestLine, // 期待请求行
        kExpectHeaders,     // 期待请求头
        kExpectBody,        // 期待请求体
        kGotAll,            // 全部获取完毕
    };
    HttpContext()
        : state_(kExpectRequestLine)
    {}

    bool parseRequest(muduo::net::Buffer* buf, muduo::Timestamp receiveTime);
    bool gotAll() const { return state_ == kGotAll; }

    void reset()
    {
        state_ = kExpectRequestLine;
        HttpRequest dummy;
        request_.swap(dummy);
    }

    const HttpRequest& request() const { return request_; }
    HttpRequest&       request()       { return request_; }
private:
    bool processRequestLine(const char* begin, const char* end);
private:
    HttpRequestParseState state_; // 当前解析状态
    HttpRequest           request_;         // HTTP请求对象
};
}