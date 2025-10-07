#pragma once
#include "../../include/router/RouterHandler.h"
#include "../../include/utils/MysqlUtil.h"
#include "../AppServer.h"
#include "../../include/utils/JsonUtil.h"

class LoginHandler : public http::router::RouterHandler
{
public:
    explicit LoginHandler(AppServer* server): server_(server){}

    void handle(const http::HttpRequest& req, http::HttpResponse* resp) override;
private:
    int queryUserId(const std::string& username, const std::string& password);
private:
    AppServer* server_;
    http::MysqlUtil mysqlUtil_;
};