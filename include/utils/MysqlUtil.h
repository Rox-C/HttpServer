#pragma once
#include "db/DbConnectionPool.h"
#include <mysql/mysql.h>  // 引入MariaDB头文件
#include <string>

namespace http
{

class MysqlUtil
{
public:
    // 初始化数据库连接池
    static void init(const std::string& host, const std::string& user,
                    const std::string& password, const std::string& database,
                    size_t poolSize = 10)
    {
        http::db::DbConnectionPool::getInstance().init(
            host, user, password, database, poolSize);
    }

    // 执行查询操作，返回MariaDB原生结果集指针
    template<typename... Args>
    MYSQL_RES* executeQuery(const std::string& sql, Args&&... args)
    {
        auto conn = http::db::DbConnectionPool::getInstance().getConnection();
        return conn->executeQuery(sql, std::forward<Args>(args)...);
    }

    // 执行更新操作（INSERT/UPDATE/DELETE），返回受影响的行数
    template<typename... Args>
    int executeUpdate(const std::string& sql, Args&&... args)
    {
        auto conn = http::db::DbConnectionPool::getInstance().getConnection();
        return conn->executeUpdate(sql, std::forward<Args>(args)...);
    }
};

} // namespace http