#pragma once
#include <memory>
#include <string>
#include <mutex>
// MariaDB Connector/C 头文件
#include <mysql/mysql.h>
#include <muduo/base/Logging.h>
#include "DbException.h"

namespace http 
{
namespace db 
{

class DbConnection 
{
public:
    DbConnection(const std::string& host, 
                const std::string& user,
                const std::string& password,
                const std::string& database);
    ~DbConnection();

    // 禁止拷贝
    DbConnection(const DbConnection&) = delete;
    DbConnection& operator=(const DbConnection&) = delete;

    bool isValid();
    void reconnect();
    void cleanup();

    template<typename... Args>
    MYSQL_RES* executeQuery(const std::string& sql, Args&&... args)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        try 
        {
            // 准备SQL语句并绑定参数
            std::string preparedSql = prepareSqlWithParams(sql, std::forward<Args>(args)...);
            
            if (mysql_query(connection_, preparedSql.c_str())) 
            {
                const char* error = mysql_error(connection_);
                LOG_ERROR << "Query failed: " << error << ", SQL: " << preparedSql;
                throw DbException(error);
            }
            
            MYSQL_RES* result = mysql_store_result(connection_);
            if (!result) 
            {
                const char* error = mysql_error(connection_);
                LOG_ERROR << "Store result failed: " << error;
                throw DbException(error);
            }
            
            return result;
        } 
        catch (const DbException& e) 
        {
            throw; // 重新抛出DbException
        }
        catch (const std::exception& e) 
        {
            LOG_ERROR << "Query failed: " << e.what() << ", SQL: " << sql;
            throw DbException(e.what());
        }
    }
    
    template<typename... Args>
    int executeUpdate(const std::string& sql, Args&&... args)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        try 
        {
            // 准备SQL语句并绑定参数
            std::string preparedSql = prepareSqlWithParams(sql, std::forward<Args>(args)...);
            
            if (mysql_query(connection_, preparedSql.c_str())) 
            {
                const char* error = mysql_error(connection_);
                LOG_ERROR << "Update failed: " << error << ", SQL: " << preparedSql;
                throw DbException(error);
            }
            
            return mysql_affected_rows(connection_);
        } 
        catch (const DbException& e) 
        {
            throw; // 重新抛出DbException
        }
        catch (const std::exception& e) 
        {
            LOG_ERROR << "Update failed: " << e.what() << ", SQL: " << sql;
            throw DbException(e.what());
        }
    }

    bool ping();  // 添加检测连接是否有效的方法
    
private:
    // 辅助函数：准备带参数的SQL语句
    template<typename... Args>
    std::string prepareSqlWithParams(const std::string& sql, Args&&... args)
    {
        // 这里简化处理，实际项目中可能需要更复杂的参数绑定逻辑
        // 对于生产环境，建议使用真正的预处理语句
        return sql;
    }

private:
    MYSQL*      connection_;  // MariaDB连接
    std::string host_;
    std::string user_;
    std::string password_;
    std::string database_;
    std::mutex  mutex_;
};
} // namespace db
} // namespace http