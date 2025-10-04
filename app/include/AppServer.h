#pragma once

#include <memory>
#include <string>
#include "http/HttpServer.h"
#include "utils/db/DbConnectionPool.h" // 假设您有一个连接池实现

namespace app {

class AppServer {
public:
    AppServer(int port, const std::string& name);
    ~AppServer() = default;

    void start();
    void initializeDatabase();
    void initializeRoutes();
    void initializeMiddlewares();

    // 获取数据库连接池
    http::db::DbConnectionPool* getDbConnectionPool() const {
        return dbConnectionPool_.get();
    }

private:
    std::unique_ptr<http::HttpServer> httpServer_;
    std::unique_ptr<http::db::DbConnectionPool> dbConnectionPool_; // 改为连接池
    
    // 数据库配置
    std::string dbHost_;
    std::string dbUser_;
    std::string dbPassword_;
    std::string dbName_;
    int dbPoolSize_; // 连接池大小
};

} // namespace app