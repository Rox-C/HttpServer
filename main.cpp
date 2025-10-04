#include "AppServer.h"
#include <muduo/base/Logging.h>
#include <iostream>
#include <signal.h>

// 全局服务器指针，用于信号处理
static app::AppServer* g_server = nullptr;

// 信号处理函数
void signalHandler(int signal) {
    LOG_INFO << "Received signal " << signal << ", shutting down server...";
    if (g_server) {
        // 这里可以添加优雅关闭逻辑
        exit(0);
    }
}

int main(int argc, char* argv[]) {
    // 设置日志级别
    muduo::Logger::setLogLevel(muduo::Logger::INFO);
    
    LOG_INFO << "Starting HTTP Server Application";
    
    // 注册信号处理器
    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);
    
    try {
        // 创建应用服务器实例
        app::AppServer server(8080, "AppServer");
        g_server = &server;
        
        // 启动服务器
        server.start();
        
    } catch (const std::exception& e) {
        LOG_ERROR << "Server error: " << e.what();
        return -1;
    }
    
    return 0;
}