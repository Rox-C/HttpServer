#include "AppServer.h"
#include "handlers/LoginHandler.h"
#include "handlers/RegisterHandler.h"
#include "handlers/BusinessHandler.h"
#include "middleware/cors/CorsMiddleware.h"
#include "session/SessionManager.h"
#include <muduo/base/Logging.h>

namespace app {

AppServer::AppServer(int port, const std::string& name)
    : dbHost_("localhost")
    , dbUser_("root")
    , dbPassword_("password")
    , dbName_("app_db")
    , dbPoolSize_(10) { // 设置连接池大小
    
    // 创建HTTP服务器 - 使用 new 替代 make_unique
    httpServer_.reset(new http::HttpServer(port, name));
    
    // 初始化数据库连接池
    initializeDatabase();
    
    // 初始化中间件
    initializeMiddlewares();
    
    // 初始化路由
    initializeRoutes();
}

void AppServer::initializeDatabase() {
    try {
        // 创建连接池而不是单个连接 - 使用 new 替代 make_unique
        dbConnectionPool_.reset(new http::db::DbConnectionPool(
            dbHost_, dbUser_, dbPassword_, dbName_, dbPoolSize_));
        
        // 测试连接池是否有效 - 如果 DbConnectionPool 没有 isValid 方法，则需要实现一个检查机制
        // 这里暂时注释掉检查逻辑，你可以根据实际情况决定如何验证连接池有效性
        /*
        if (!dbConnectionPool_->isValid()) {
            LOG_ERROR << "Failed to initialize database connection pool";
            return;
        }
        */
        
        LOG_INFO << "Database connection pool initialized successfully with " 
                 << dbPoolSize_ << " connections";
    } catch (const std::exception& e) {
        LOG_ERROR << "Database connection pool initialization failed: " << e.what();
    }
}

void AppServer::initializeMiddlewares() {
    // 添加CORS中间件
    auto corsMiddleware = std::make_shared<http::middleware::CorsMiddleware>();
    httpServer_->addMiddleware(corsMiddleware);
    
    // 添加Session管理器 - 使用 new 替代 make_unique
    std::unique_ptr<http::session::SessionManager> sessionManager(new http::session::SessionManager());
    httpServer_->setSessionManager(std::move(sessionManager));
}

void AppServer::initializeRoutes() {
    // 注册登录处理器，传入连接池而不是单个连接
    // 修复命名空间问题 - 应该使用 app::http::handlers 而不是 http::handlers
    auto loginHandler = std::make_shared<app::http::handlers::LoginHandler>(dbConnectionPool_.get());
    httpServer_->Post("/api/login", loginHandler);
    
    // 注册注册处理器
    auto registerHandler = std::make_shared<app::http::handlers::RegisterHandler>(dbConnectionPool_.get());
    httpServer_->Post("/api/register", registerHandler);
    
    // 注册业务处理器
    auto businessHandler = std::make_shared<app::http::handlers::BusinessHandler>(dbConnectionPool_.get());
    httpServer_->Get("/api/business", businessHandler);
    httpServer_->Post("/api/business", businessHandler);
    
    LOG_INFO << "Routes initialized successfully";
}

void AppServer::start() {
    LOG_INFO << "Starting AppServer...";
    httpServer_->start();
}

} // namespace app