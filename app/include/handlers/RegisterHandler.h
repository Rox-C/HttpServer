#pragma once
#include "../../include/router/RouterHandler.h"
#include "../../include/utils/MysqlUtil.h"
#include "../AppServer.h"
#include "../../include/utils/JsonUtil.h"

class RegisterHandler : public http::router::RouterHandler 
{
public:
    explicit RegisterHandler(AppServer* server) : server_(server) {}

    void handle(const http::HttpRequest& req, http::HttpResponse* resp) override;
private:
    int insertUser(const std::string& username, const std::string& password);
    bool isUserExist(const std::string& username);
private:
    AppServer* server_;
    http::MysqlUtil     mysqlUtil_;
};