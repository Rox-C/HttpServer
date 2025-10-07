#pragma once
#include "../../include/router/RouterHandler.h"
#include "../../include/utils/MysqlUtil.h"
#include "../AppServer.h"
#include "../../include/utils/JsonUtil.h"

class MenuHandler : public http::router::RouterHandler
{
public:
    explicit MenuHandler(AppServer* server) : server_(server) {}

    void handle(const http::HttpRequest& req, http::HttpResponse* resp) override;
private:
    AppServer* server_;
};