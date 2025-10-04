#include "../../../include/middleware/cors/CorsMiddleware.h"
#include <muduo/base/Logging.h>
#include <algorithm>
#include <sstream>
#include <iostream>

namespace http
{
namespace middleware
{
    CorsMiddleware::CorsMiddleware(const CorsConfig& config): config_(config) {}

    void CorsMiddleware::before(HttpRequest& request)
    {
        LOG_DEBUG << "CorsMiddleware before called.";
        if(request.method() == HttpRequest::kOptions)
        {
            //处理预检请求
            HttpResponse response;
            handlePreflightRequest(request, response);
            LOG_DEBUG << "Handled preflight OPTIONS request.";
            throw response; // 使用异常机制中断请求处理流程，直接返回响应
        }
    }

    void CorsMiddleware::after(HttpResponse& response)
    {
        LOG_DEBUG << "CorsMiddleware after called.";
        if(!config_.allowedOrigins.empty())
        {
            //如果允许所有源
            if(std::find(config_.allowedOrigins.begin(),config_.allowedOrigins.end(),"*") != config_.allowedOrigins.end())
            {
                addCorsHeaders(response, "*");
            }
            else
            {
                addCorsHeaders(response,config_.allowedOrigins[0]);
            }
        }
    }

    bool CorsMiddleware::isOriginAllowed(const std::string& origin) const
    {
        return config_.allowedOrigins.empty() || 
            std::find(config_.allowedOrigins.begin(), 
                        config_.allowedOrigins.end(), "*") != config_.allowedOrigins.end() ||
            std::find(config_.allowedOrigins.begin(), 
                        config_.allowedOrigins.end(), origin) != config_.allowedOrigins.end();
    }

    void CorsMiddleware::handlePreflightRequest(const HttpRequest& request, HttpResponse& response)
    {
        const std::string& origin = request.getHeader("Origin");
        if(!isOriginAllowed(origin))
        {
            LOG_WARN << "Origin not allowed: " << origin;
            response.setStatusCode(HttpResponse::k403Forbidden);
            response.setStatusMessage("Forbidden");
            return;
        }

        addCorsHeaders(response, origin);
        response.setStatusCode(HttpResponse::k204NoContent);
        response.setStatusMessage("No Content");
        LOG_INFO << "Preflight request from origin " << origin << " allowed.";
    }

    void CorsMiddleware::addCorsHeaders(HttpResponse& response, const std::string& origin)
    {
        try
        {
            response.addHeader("Access-Control-Allow-Origin", origin);
            if(config_.allowCredentials)
            {
                response.addHeader("Access-Control-Allow-Origin", "true");
            }
            if(!config_.allowedMethods.empty())
            {
                response.addHeader("Access-Control-Allow-Methods",join(config_.allowedMethods,","));
            }
            if(!config_.allowedHeaders.empty())
            {
                response.addHeader("Access-Control-Allow-Headers",
                                    join(config_.allowedHeaders,","));
            }

            response.addHeader("Access-Control-Max-Age",
                                std::to_string(config_.maxAge));

            LOG_DEBUG << "CORS headers added to response.";

        }
        catch(const std::exception& e)
        {
            LOG_ERROR << "Exception in addCorsHeaders: " << e.what();
        }
    }

    std::string CorsMiddleware::join(const std::vector<std::string>& strings, const std::string& delimiter)
    {
        std::ostringstream result;
        for(size_t i = 0;i < strings.size();++i)
        {
            if(i > 0)
            {
                result << delimiter;
            }
            result << strings[i];
        }
        return result.str();
    }
}
}