#pragma once

#include <map>
#include <string>
#include <unordered_map>

#include <muduo/base/Timestamp.h>

namespace http
{


class HttpRequest
{
public:
    enum Method
    {
        kInvalid, kGet, kPost, kHead, kPut, kDelete, kOptions
    };

    HttpRequest()
        : method_(kInvalid)
        , version_("Unknown")
    {}

    void setReceiveTime(muduo::Timestamp t);
    muduo::Timestamp receiveTime() const { return receiveTime_; }

    bool setMethod(const char* start, const char* end);
    Method method() const { return method_; }

    void setPath(const char* start, const char* end);
    const std::string& path() const { return path_; }

    void setPathParameters(const std::string& key, const std::string& value);
    std::string getPathParameter(const std::string& key) const;

    void setQueryParameters(const char* start, const char* end);
    std::string getQueryParameter(const std::string& key) const;

    void setVersion(std::string v) { version_ = v; }
    const std::string& getVersion() const { return version_; }

    void addHeader(const char* start, const char* colon, const char* end);
    std::string getHeader(const std::string& field) const;

    const std::map<std::string, std::string>& headers() const { return headers_; }

    void setBody(const char* start, const char* end)
    {
        if(end >= start)
        {
            body_.assign(start, end);
        }
    }
    void setBody(const std::string& body) { body_ = body; }
    std::string getBody() const { return body_; }

    void setContentLength(uint64_t length) { contentLength_ = length; }
    uint64_t getContentLength() const { return contentLength_; }

    void swap(HttpRequest& that);
private:
    Method                                      method_; // HTTP方法
    std::string                                 version_; // HTTP版本
    std::string                                 path_; // 请求路径
    std::unordered_map<std::string,std::string> pathParameters_; // 路径参数
    std::unordered_map<std::string,std::string> queryParameters_; // 查询参数
    muduo::Timestamp                            receiveTime_; // 接收时间
    std::map<std::string,std::string>           headers_; // HTTP头部
    std::string                                 body_; // 请求体
    uint64_t                                    contentLength_{0}; // 内容长度
};

}