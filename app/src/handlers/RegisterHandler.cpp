#include "../include/handlers/RegisterHandler.h"

void RegisterHandler::handle(const http::HttpRequest& req, http::HttpResponse* resp)
{
    //解析body
    json parsed = json::parse(req.getBody());
    std::string username = parsed["username"];
    std::string password = parsed["password"];

    //判断用户是否已经存在，如果存在则注册失败
    int userId = insertUser(username, password);
    if(userId != -1)
    {
        //注册成功
        //封装成功响应
        json successResp;
        successResp["status"] = "success";
        successResp["message"] = "Register successfully";
        successResp["userId"] = userId;
        std::string successRespStr = successResp.dump(4);
        resp->setStatusLine(req.getVersion(), http::HttpResponse::HttpStatusCode::k200Ok, "OK");
        resp->setCloseConnection(false);
        resp->setContentType("application/json");
        resp->setContentLength(successRespStr.size());
        resp->setBody(successRespStr);
    }
    else
    {
        //注册失败
        json failResp;
        failResp["status"] = "error";
        failResp["message"] = "Register failed : username already exists";
        std::string failRespStr = failResp.dump(4);
        resp->setStatusLine(req.getVersion(), http::HttpResponse::HttpStatusCode::k409Conflict, "Conflict");
        resp->setCloseConnection(false);
        resp->setContentType("application/json");
        resp->setContentLength(failRespStr.size());
        resp->setBody(failRespStr);
    }
}

int RegisterHandler::insertUser(const std::string& username, const std::string& password)
{
    if(!isUserExist(username))
    {
        //用户不存在
        std::string sql1 = "INSERT INTO users (username, password) VALUES ('" + username + "', '" + password + "')";
        mysqlUtil_.executeUpdate(sql1);
        std::string sql2 = "SELECT id FROM users WHERE username = '" + username + "'";
        MYSQL_RES* res = mysqlUtil_.executeQuery(sql2);
        MYSQL_ROW row;
        if ((row = mysql_fetch_row(res)) != NULL)
        {
            // 假设id是第一列，根据实际情况调整索引
            int id = atoi(row[0]); 
            mysql_free_result(res);  // 释放结果集内存
            return id;
        }
        mysql_free_result(res);  // 释放结果集内存
    }
    return -1;
}

bool RegisterHandler::isUserExist(const std::string& username)
{
    std::string sql = "SELECT * FROM users WHERE username = '" + username + "'";
    MYSQL_RES* res = mysqlUtil_.executeQuery(sql);
    MYSQL_ROW row;
    if((row = mysql_fetch_row(res)) != NULL)
    {
        mysql_free_result(res);  // 释放结果集内存
        return true;
    }
    mysql_free_result(res);  // 释放结果集内存
    return false;
}