#include "../include/AppServer.h"
#include "../include/handlers/LoginHandler.h"
#include "../include/handlers/RegisterHandler.h"
#include "../../include/http/HttpServer.h"
#include "../../include/http/HttpRequest.h"
#include "../../include/http/HttpResponse.h"

using namespace http;

AppServer::AppServer(int port,
                    const std::string& name,
                    muduo::net::TcpServer::Option option)
            : httpServer_(port, name, option)
            {
                initialize();
            }

void AppServer::setThreadNum(int numThreads)
{
    httpServer_.setThreadNum(numThreads);
}

void AppServer::start()
{
    httpServer_.start();
}

void AppServer::initialize()
{
    try {
        http::MysqlUtil::init("127.0.0.1", "HttpServer", "zyH7PcKm6m3t82pH", "HttpServer", 10);
        LOG_WARN << "Database initialization completed";
        initializeSession();
        initializeMiddleware();
        initializeRouter();
        LOG_WARN << "AppServer initialization completed";
    } catch (const std::exception& e) {
        LOG_ERROR << "Failed to initialize AppServer: " << e.what();
        throw; // 重新抛出异常，确保程序正确处理初始化失败
    }
}

void AppServer::initializeSession()
{
    auto sessionStorage = std::make_unique<session::MemorySessionStorage>();
    auto sessionManager = std::make_unique<session::SessionManager>(std::move(sessionStorage));
    setSessionManager(std::move(sessionManager));
}

void AppServer::initializeMiddleware()
{
    auto corsMiddleware = std::make_shared<middleware::CorsMiddleware>();
    httpServer_.addMiddleware(corsMiddleware);
}

void AppServer::initializeRouter()
{
    httpServer_.Get("/", std::make_shared<BusinessHandler>(this));
    httpServer_.Post("/login",std::make_shared<LoginHandler>(this));
    httpServer_.Post("/register",std::make_shared<RegisterHandler>(this));
    httpServer_.Get("/menu",std::make_shared<MenuHandler>(this));
}