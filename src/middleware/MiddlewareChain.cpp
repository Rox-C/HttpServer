#include "../../include/middleware/MiddlewareChain.h"
#include <muduo/base/Logging.h>

namespace http
{
namespace middleware
{
void MiddlewareChain::addMiddleware(std::shared_ptr<Middleware> middleware)
{
    middlewares_.push_back(middleware);
}

void MiddlewareChain::processBefore(HttpRequest& request)
{
    for(auto& middleware : middlewares_)
    {
        if(middleware)
        {
            middleware->before(request);
        }
    }
}

void MiddlewareChain::processAfter(HttpResponse& response)
{
    try
    {
        //反向遍历中间件列表，确保响应后处理的顺序与请求前处理的顺序相反
        for(auto it = middlewares_.rbegin(); it != middlewares_.rend(); ++it)
        {
            if(*it)
            {
                (*it)->after(response);
            }
        }
    }catch(const std::exception& e)
    {
        LOG_ERROR << "Exception in processAfter: " << e.what();
    }
}

}
}