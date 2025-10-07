#include "../include/handlers/LoginHandler.h"

void LoginHandler::handle(const http::HttpRequest& req, http::HttpResponse* resp)
{
    // 处理登录逻辑
    // 验证 Content-Type
    auto contentType = req.getHeader("Content-Type");
    if(contentType.empty() || contentType != "application/json" || req.getBody().empty())
    {
        LOG_INFO << "content:" << req.getBody();
        resp->setStatusLine(req.getVersion(), http::HttpResponse::k400BadRequest, "Bad Request");
        resp->setCloseConnection(true);
        resp->setContentType("application/json");
        resp->setContentLength(0);
        resp->setBody("");
        return;
    }
    //JSON 解析使用 try catch
    try
    {
        json paresed = json::parse(req.getBody());
        std::string username = paresed["username"];
        std::string password = paresed["password"];
        int userId = queryUserId(username, password);
        if(userId != -1)
        {
            // 获取会话
            auto session = server_->getSessionManager()->getSession(req,resp);
            // 设置会话
            session->setValue("userId", std::to_string(userId));
            session->setValue("username", username);
            session->setValue("isLoggedIn", "true");
            //登录成功
            //封装响应报文
            json successResp;
            successResp["success"] = true;
            successResp["userId"] = userId;
            std::string successRespStr = successResp.dump(4);
            resp->setStatusLine(req.getVersion(), http::HttpResponse::k200Ok, "OK");
            resp->setCloseConnection(false);
            resp->setContentType("application/json");
            resp->setContentLength(successRespStr.size());
            resp->setBody(successRespStr);
            return;
        }
    }catch(const json::exception& e)
    {
        json failResp;
        failResp["status"] = "error";
        failResp["message"] = e.what();
        std::string failRespStr = failResp.dump(4);
        resp->setStatusLine(req.getVersion(), http::HttpResponse::k400BadRequest, "Bad Request");
        resp->setCloseConnection(true);
        resp->setContentType("application/json");
        resp->setContentLength(failRespStr.size());
        resp->setBody(failRespStr);
        return;
    }
}

int LoginHandler::queryUserId(const std::string& username, const std::string& password)
{
    std::string sql = "SELECT id FROM users WHERE username = ? AND password = ?";
    MYSQL_RES* res = mysqlUtil_.executeQuery(sql, username, password);
    MYSQL_ROW row = mysql_fetch_row(res);
    if(row)
    {
        mysql_free_result(res);  // 释放结果集
        return atoi(row[0]);
    }
    mysql_free_result(res);  // 释放结果集
    return -1;
}
