#pragma once

#include <atomic>
#include <memory>
#include <tuple>
#include <unordered_map>
#include <mutex>

#include "../../include/http/HttpServer.h"
#include "../../include/utils/MysqlUtil.h"
#include "../../include/utils/JsonUtil.h"
#include "../../include/utils/FileUtil.h"

class LoginHandler;
class RegisterHandler;
class BusinessHandler;
class MenuHandler;
class AppServer
{
public:
    AppServer(int port,
                const std::string& name,
                muduo::net::TcpServer::Option option = muduo::net::TcpServer::kNoReusePort);

    void setThreadNum(int numThreads);
    void start();
private:
    void initialize();
    void initializeSession();
    void initializeRouter();
    void initializeMiddleware();

    void setSessionManager(std::unique_ptr<http::session::SessionManager> manager)
    {
        httpServer_.setSessionManager(std::move(manager));
    }

    http::session::SessionManager* getSessionManager() const
    {
        return httpServer_.getSessionManager();
    }
private:
    friend class LoginHandler;
    friend class RegisterHandler;
    friend class BusinessHandler;
    friend class MenuHandler;
private:
    http::HttpServer                      httpServer_;
    http::MysqlUtil                       mysqlUtil_;
};