#pragma once

#include <string>
#include <vector>

namespace http
{
namespace middleware
{
struct CorsConfig
{
    std::vector<std::string> allowedOrigins; // 允许的源
    std::vector<std::string> allowedMethods; // 允许的HTTP方法
    std::vector<std::string> allowedHeaders; // 允许的HTTP头
    bool allowCredentials = false;           // 是否允许携带凭证（如Cookies）
    int maxAge = 3600;                       // 预检请求的缓存时间，单位为秒

    static CorsConfig defaultConfig()
    {
        CorsConfig config;
        config.allowedOrigins = {"*"}; // 允许所有源
        config.allowedMethods = {"GET", "POST", "PUT", "DELETE", "OPTIONS"};
        config.allowedHeaders = {"Content-Type", "Authorization"};
        return config;
    }
};
}
}