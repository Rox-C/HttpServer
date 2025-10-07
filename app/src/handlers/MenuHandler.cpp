#include "../include/handlers/MenuHandler.h"

void MenuHandler::handle(const http::HttpRequest& req, http::HttpResponse* resp)
{
    try
    {
        auto session = server_->getSessionManager()->getSession(req,resp);
        LOG_INFO << "session->getValue(\"isLoggedIn\") = " << session->getValue("isLoggedIn");
        if(session->getValue("isLoggedIn") != "true")
        {
            json errorResp;
            errorResp["status"] = "error";
            errorResp["message"] = "Unauthorized";
            std::string errorBody = errorResp.dump(4);

            resp->setStatusLine(req.getVersion(), http::HttpResponse::HttpStatusCode::k401Unauthorized, "Unauthorized");
            resp->setCloseConnection(true);
            resp->setContentType("application/json");
            resp->setContentLength(errorBody.size());
            resp->setBody(errorBody);
            return;
        }
        int userId = std::stoi(session->getValue("userId"));
        std::string username = session->getValue("username");
        std::string reqFile("../../../resource/menu.html");
        FileUtil fileOperator(reqFile);
        if(!fileOperator.isValid())
        {
            LOG_WARN << "file not found: " << reqFile;
            fileOperator.resetDefaultFile();
        }
        std::vector<char> buffer(fileOperator.size());
        fileOperator.readFile(buffer);
        std::string htmlContent(buffer.data(),buffer.size());
        size_t headEnd = htmlContent.find("</head>");
        if(headEnd != std::string::npos)
        {
            std::string script = "<script>const userId = '" + std::to_string(userId) + "';</script>";
            htmlContent.insert(headEnd, script);
        }
        resp->setStatusLine(req.getVersion(), http::HttpResponse::HttpStatusCode::k200Ok, "OK");
        resp->setCloseConnection(false);
        resp->setContentType("text/html");
        resp->setContentLength(htmlContent.size());
        resp->setBody(htmlContent);
    }
    catch(const std::exception& e)
    {
                json failureResp;
        failureResp["status"] = "error";
        failureResp["message"] = e.what();
        std::string failureBody = failureResp.dump(4);
        resp->setStatusLine(req.getVersion(), http::HttpResponse::HttpStatusCode::k400BadRequest, "Bad Request");
        resp->setCloseConnection(true);
        resp->setContentType("application/json");
        resp->setContentLength(failureBody.size());
        resp->setBody(failureBody);
    }
}