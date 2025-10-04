#include "../../include/http/HttpContext.h"

using namespace muduo;
using namespace muduo::net;

namespace http
{
    bool HttpContext::parseRequest(Buffer *buf, Timestamp receiveTime)
    {
        //将缓冲区中的数据解析为HTTP请求
        bool ok = true; // 解析每行是否成功
        bool hasMore = true; // 是否还有更多数据需要解析

        while(hasMore)
        {
            const char *crlf = buf->findCRLF(); // 查找\r\n
            if(state_ == kExpectRequestLine) // 期待请求行
            {
                ok = processRequestLine(buf->peek(), crlf);
                if(ok)
                {
                    request_.setReceiveTime(receiveTime);
                    buf->retrieveUntil(crlf + 2); // 移动缓冲区读指针，跳过请求行
                    state_ = kExpectHeaders; // 进入期待请求头状态
                }
                else
                {
                    hasMore = false; // 解析失败，停止解析
                }
            }
            else if(state_ == kExpectHeaders)
            {
                const char *crlf = buf->findCRLF();
                if(crlf) // 找到\r\n
                {
                    const char *colon = std::find(buf->peek(), crlf, ':');
                    if(colon != crlf) // 找到冒号
                    {
                        request_.addHeader(buf->peek(), colon, crlf); // 添加请求头
                    }
                    else if(buf->peek() == crlf) // 空行结束请求头
                    {
                        if(request_.method() == HttpRequest::kPost || request_.method() == HttpRequest::kPut)
                        {
                            std::string contentLength = request_.getHeader("Content-Length");
                            if(!contentLength.empty())
                            {
                                request_.setContentLength(std::stoi(contentLength));
                                if(request_.getContentLength() > 0)
                                {
                                    state_ = kExpectBody; // 进入期待请求体状态
                                }
                                else
                                {
                                    state_ = kGotAll; // 无请求体，全部获取完毕
                                    hasMore = false;
                                }
                            }
                            else 
                            {
                                ok = false; // POST或PUT请求必须有Content-Length头
                                hasMore = false;
                            }
                        }
                        else
                        {
                            state_ = kGotAll; // 非POST或PUT请求，全部获取完毕
                            hasMore = false;
                        }
                    }
                    else
                    {
                        ok = false; // 请求头格式错误
                        hasMore = false;
                    }
                    buf->retrieveUntil(crlf + 2); // 移动缓冲区读指针，跳过请求头行
                }
                else
                {
                    hasMore = false; // 没有找到完整的请求头行，等待更多数据
                }
            }
            else if(state_ == kExpectBody)
            {
                //检查是否有足够的数据读取请求体
                if(buf->readableBytes() < request_.getContentLength())
                {
                    hasMore =  false; // 数据不足，等待更多数据
                    return true;
                }

                //只读取请求体
                std::string body(buf->peek() , buf->peek() + request_.getContentLength());
                request_.setBody(body);
                buf->retrieve(request_.getContentLength()); // 移动缓冲区读指
                state_ = kGotAll; // 全部获取完毕
                hasMore = false;
            }
        }
        return ok; // 返回解析是否成功
    }

    bool HttpContext::processRequestLine(const char * begin, const char *end)
    {
        bool succeed = false;
        const char *start = begin;
        const char *space = std::find(start, end, ' ');
        if(space != end && request_.setMethod(start, space)) // 解析HTTP方法
        {
            start = space + 1;
            space = std::find(start, end, ' ');
            if(space != end) // 解析路径
            {
                const char *argumentStart = std::find(start, space, '?');
                if(argumentStart != space) //请求带参数
                {
                    request_.setPath(start, argumentStart); // 设置路径
                    request_.setQueryParameters(argumentStart + 1, space); // 设置查询参数
                }
                else //请求不带参数
                {
                    request_.setPath(start, space); // 设置路径
                }
                start = space + 1;
                succeed = ((end - start) == 8 && std::equal(start, end - 1, "HTTP/1.")); // 解析HTTP版本
                if(succeed)
                {
                    if(*(end - 1) == '1')
                    {
                        request_.setVersion("HTTP/1.1");
                    }
                    else if(*(end - 1) == '0')
                    {
                        request_.setVersion("HTTP/1.0");
                    }
                    else
                    {
                        succeed = false;
                    }
                }
            }
        }
        return succeed;
    }
}