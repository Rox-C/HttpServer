#pragma once
#include <iostream>
#include <unordered_map>
#include <string>
#include <memory>
#include <functional>
#include <regex>
#include <vector>

#include "RouterHandler.h"
#include "../http/HttpRequest.h"
#include "../http/HttpResponse.h"

namespace http
{
namespace router
{
//选择是否注册对象式路由处理器还是回调函数式路由处理器取决于处理器执行的复杂程度
//如果是简单的处理逻辑，可以使用回调函数式路由处理器
//如果处理逻辑较为复杂，涉及多个步骤或状态管理，建议使用对象
class Router
{
public :
    //对象式路由处理器注册
    using HandlerPtr = std::shared_ptr<RouterHandler>; 
    //回调函数式路由处理器注册
    using HandlerCallback = std::function<void(const HttpRequest &, HttpResponse*)>; 

    //路由键值（HTTP方法 + 路径）
    struct RouteKey
    {
        HttpRequest::Method method; // HTTP方法
        std::string path; // 路径

        bool operator==(const RouteKey& other) const
        {
            return method == other.method && path == other.path;
        }
    };

    //哈希函数，用于RouteKey在unordered_map中的存储
    struct RouteKeyHash
    {
        size_t operator()(const RouteKey& key) const
        {
            size_t methodHash = std::hash<int>{}(static_cast<int>(key.method));
            size_t pathHash = std::hash<std::string>{}(key.path);
            return methodHash * 31 + pathHash;
        }
    };

    //注册对象式路由处理器
    void registerHandler(HttpRequest::Method method, const std::string &path, HandlerPtr handler);

    //注册回调函数式路由处理器
    void registerCallback(HttpRequest::Method method, const std::string &path, HandlerCallback callback);

    //注册正则表达式对象式路由处理器
    void addRegexHandler(HttpRequest::Method method, const std::string &path, HandlerPtr handler)
    {
        std::regex pathRegex = convertPathToRegex(path);
        regexHandlers_.emplace_back(method,pathRegex,handler);
    }
    //注册正则表达式回调函数式路由处理器
    void addRegexCallback(HttpRequest::Method method, const std::string &path, HandlerCallback callback)
    {
        std::regex pathRegex = convertPathToRegex(path);
        regexCallbacks_.emplace_back(method,pathRegex,callback);
    }

    //处理请求
    bool route(const HttpRequest& req, HttpResponse* resp);
private:
    std::regex convertPathToRegex(const std::string& path)
    {
        std::string regexPath = "^" + std::regex_replace(path,std::regex(R"(/:([^/]+))"),R"(/([^/]+))") + "$";
        return std::regex(regexPath);
    }

    void extractPathParameters(const std::smatch& match, HttpRequest &request)
    {
        for(size_t i = 1; i < match.size(); ++i)
        {
            request.setPathParameters("param" + std::to_string(i), match[i]);
        }
    }
private:
    struct RouteCallbackObj
    {
        HttpRequest::Method method_;
        std::regex pathRegex_; // 路径正则表达式
        HandlerCallback callback_; // 回调函数
        RouteCallbackObj(HttpRequest::Method method, std::regex pathRegex, HandlerCallback callback)
            : method_(method), pathRegex_(pathRegex), callback_(callback) {}
    };

    struct RouteHandlerObj
    {
        HttpRequest::Method method_;
        std::regex pathRegex_; // 路径正则表达式
        HandlerPtr handler_; // 路由处理器对象
        RouteHandlerObj(HttpRequest::Method method, std::regex pathRegex, HandlerPtr handler)
            : method_(method), pathRegex_(pathRegex), handler_(handler) {}
    };

    std::unordered_map<RouteKey,HandlerPtr,RouteKeyHash> handlers_; //精确匹配的对象式路由处理器
    std::unordered_map<RouteKey,HandlerCallback,RouteKeyHash> callbacks_; //精确匹配的回调函数式路由处理器
    std::vector<RouteHandlerObj> regexHandlers_; //正则匹配的对象式路由处理器
    std::vector<RouteCallbackObj> regexCallbacks_; //正则匹配的回调函数式路由
};
}
}
