// DbConnection.cpp
#include "../../../include/utils/db/DbConnection.h"
#include "../../../include/utils/db/DbException.h"
#include <muduo/base/Logging.h>
#include <stdexcept>

namespace http 
{
namespace db 
{

DbConnection::DbConnection(const std::string& host,
                         const std::string& user,
                         const std::string& password,
                         const std::string& database)
    : host_(host)
    , user_(user)
    , password_(password)
    , database_(database)
{
    try 
    {
        // 初始化MySQL连接
        connection_ = mysql_init(nullptr);
        if (!connection_) 
        {
            const char* error = mysql_error(connection_);
            LOG_ERROR << "Failed to initialize MySQL connection: " << error;
            throw DbException(error);
        }

        // 设置连接选项
        my_bool reconnect = 1;
        mysql_options(connection_, MYSQL_OPT_RECONNECT, &reconnect);
        
        unsigned int timeout = 10;
        mysql_options(connection_, MYSQL_OPT_CONNECT_TIMEOUT, &timeout);
        
        // my_bool multi_statements = 0;
        // mysql_options(connection_, MYSQL_OPT_MULTI_STATEMENTS, &multi_statements);

        // 建立连接
        if (!mysql_real_connect(connection_, host_.c_str(), user_.c_str(), 
                               password_.c_str(), database_.c_str(), 0, nullptr, 0)) 
        {
            const char* error = mysql_error(connection_);
            LOG_ERROR << "Failed to connect to database: " << error;
            throw DbException(error);
        }

        // 设置字符集
        if (mysql_set_character_set(connection_, "utf8mb4")) 
        {
            const char* error = mysql_error(connection_);
            LOG_WARN << "Failed to set character set: " << error;
        }

        LOG_INFO << "Database connection established";
    } 
    catch (const DbException& e) 
    {
        // 清理已分配的资源
        if (connection_) 
        {
            mysql_close(connection_);
            connection_ = nullptr;
        }
        throw;
    }
    catch (const std::exception& e) 
    {
        // 清理已分配的资源
        if (connection_) 
        {
            mysql_close(connection_);
            connection_ = nullptr;
        }
        LOG_ERROR << "Failed to create database connection: " << e.what();
        throw DbException(e.what());
    }
}

DbConnection::~DbConnection() 
{
    try 
    {
        cleanup();
    } 
    catch (...) 
    {
        // 析构函数中不抛出异常
    }
    
    if (connection_) 
    {
        mysql_close(connection_);
        connection_ = nullptr;
        LOG_INFO << "Database connection closed";
    }
}

bool DbConnection::ping() 
{
    if (!connection_) 
    {
        return false;
    }
    
    try 
    {
        return mysql_ping(connection_) == 0;
    } 
    catch (...) 
    {
        return false;
    }
}

bool DbConnection::isValid() 
{
    return connection_ != nullptr && mysql_ping(connection_) == 0;
}

void DbConnection::reconnect() 
{
    std::lock_guard<std::mutex> lock(mutex_);
    
    try 
    {
        if (connection_) 
        {
            mysql_close(connection_);
            connection_ = nullptr;
        }

        // 重新初始化连接
        connection_ = mysql_init(nullptr);
        if (!connection_) 
        {
            const char* error = mysql_error(connection_);
            LOG_ERROR << "Failed to initialize MySQL connection: " << error;
            throw DbException(error);
        }

        // 设置连接选项
        my_bool reconnect = 1;
        mysql_options(connection_, MYSQL_OPT_RECONNECT, &reconnect);
        
        unsigned int timeout = 10;
        mysql_options(connection_, MYSQL_OPT_CONNECT_TIMEOUT, &timeout);
        
        // my_bool multi_statements = 0;
        // mysql_options(connection_, MYSQL_OPT_MULTI_STATEMENTS, &multi_statements);

        // 建立连接
        if (!mysql_real_connect(connection_, host_.c_str(), user_.c_str(), 
                               password_.c_str(), database_.c_str(), 0, nullptr, 0)) 
        {
            const char* error = mysql_error(connection_);
            LOG_ERROR << "Failed to reconnect to database: " << error;
            throw DbException(error);
        }

        // 设置字符集
        if (mysql_set_character_set(connection_, "utf8mb4")) 
        {
            const char* error = mysql_error(connection_);
            LOG_WARN << "Failed to set character set: " << error;
        }

        LOG_INFO << "Database reconnected successfully";
    } 
    catch (const std::exception& e) 
    {
        LOG_ERROR << "Reconnect failed: " << e.what();
        throw DbException(e.what());
    }
}

void DbConnection::cleanup() 
{
    std::lock_guard<std::mutex> lock(mutex_);
    // MariaDB的清理工作相对简单，主要是检查连接状态
    try 
    {
        if (connection_) 
        {
            // 执行一个简单的查询来验证连接
            if (mysql_query(connection_, "SELECT 1")) 
            {
                // 查询失败，连接可能已经断开
                const char* error = mysql_error(connection_);
                LOG_WARN << "Connection check failed: " << error;
            }
        }
    } 
    catch (const std::exception& e) 
    {
        LOG_WARN << "Error during connection cleanup: " << e.what();
    }
}

} // namespace db
} // namespace http