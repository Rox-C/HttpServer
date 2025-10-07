#include "../../include/router/Router.h"
#include <muduo/base/Logging.h>

namespace http
{
namespace router
{
void Router::registerHandler(HttpRequest::Method method, const std::string &path, HandlerPtr handler)
{
    RouteKey key{method, path};
    handlers_[key] = std::move(handler);
}

void Router::registerCallback(HttpRequest::Method method, const std::string &path, HandlerCallback callback)
{
    RouteKey key{method, path};
    callbacks_[key] = std::move(callback);
}

bool Router::route(const HttpRequest& req, HttpResponse* resp)
{
    RouteKey key{req.method(), req.path()};
    //1.尝试精确匹配对象式路由处理器
    auto it = handlers_.find(key);
    if(it != handlers_.end())
    {
        it->second->handle(req, resp);
        return true;
    }

    //2.尝试精确匹配回调函数式路由处理器
    auto cbIt = callbacks_.find(key);
    if(cbIt != callbacks_.end())
    {
        cbIt->second(req, resp);
        return true;
    }

    //3.尝试正则匹配对象式路由处理器
    for(const auto&[method, pathRegex, handler] : regexHandlers_)
    {
        std::smatch match;
        std::string reqPath = req.path();
        //如果HTTP方法匹配且路径正则表达式匹配
        if(method == req.method() && std::regex_match(reqPath, match, pathRegex))
        {
            HttpRequest reqCopy = req; // 复制请求对象以添加路径参数
            extractPathParameters(match, reqCopy);
            handler->handle(reqCopy, resp);
            return true;
        }
    }

    //4.尝试正则匹配回调函数式路由处理器
    for(const auto&[method, pathRegex, callback] : regexCallbacks_)
    {
        std::smatch match;
        std::string reqPath = req.path();
        //如果HTTP方法匹配且路径正则表达式匹配
        if(method == req.method() && std::regex_match(reqPath, match, pathRegex))
        {
            HttpRequest reqCopy = req; // 复制请求对象以添加路径参数
            extractPathParameters(match, reqCopy);
            callback(reqCopy, resp);
            return true;
        }
    }

    //未找到匹配的路由处理器
    return false;
}
}
}