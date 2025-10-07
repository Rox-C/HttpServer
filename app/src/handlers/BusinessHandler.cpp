#include "../include/handlers/BusinessHandler.h"

void BusinessHandler::handle(const http::HttpRequest& req, http::HttpResponse* resp)
{
    std::string reqFile;
    reqFile.append("../../../resource/index.html");
    FileUtil fileOperator(reqFile);
    if(!fileOperator.isValid())
    {
        LOG_ERROR << "file not found : " << reqFile;
        fileOperator.resetDefaultFile();
    }
    std::vector<char> buffer(fileOperator.size());
    fileOperator.readFile(buffer);
    std::string bufstr = std::string(buffer.data(), buffer.size());

    resp->setStatusLine(req.getVersion(), http::HttpResponse::HttpStatusCode::k200Ok, "OK");
    resp->setCloseConnection(false);
    resp->setContentType("text/html");
    resp->setContentLength(bufstr.size());
    resp->setBody(bufstr);
}