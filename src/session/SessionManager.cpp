#include "../include/session/SessionManager.h"
#include <iomanip>
#include <iostream>
#include <sstream>

namespace http
{
namespace session
{

    SessionManager::SessionManager(std::unique_ptr<SessionStorage> storage)
        : storage_(std::move(storage))
        , rng_(std::random_device{}())
        {}
    //从请求中获取和创建回话，如果请求中有回话ID则加载会话否则创建会话
    std::shared_ptr<Session> SessionManager::getSession(const HttpRequest& req, HttpResponse* resp)
    {
        std::string sessionId = getSessionIdFromCookie(req);
        std::shared_ptr<Session> session;
        if(!sessionId.empty())
        {
            session = storage_->load(sessionId);
        }
        if(!session || session->isExpired())
        {
            sessionId = generateSessionId();
            session = std::make_shared<Session>(sessionId,this);
            setSessionCookie(sessionId,resp);
        }
        else
        {
            session->setManager(this);
        }
        session->refresh();
        storage_->save(session);
        return session;
    }

    //生成唯一的会话管理标识符
    std::string SessionManager::generateSessionId()
    {
        std::stringstream ss;
        std::uniform_int_distribution<int> dist(0, 15);
        for(int i = 0; i < 32; ++i)
        {
            ss << std::hex << dist(rng_);
        }
        return ss.str();
    }

    void SessionManager::destroySession(const std::string& sessionId)
    {
        storage_->remove(sessionId);
    }

    void SessionManager::cleanExpiredSessions()
    {
        // TODO: 删除过期的会话，我是基于内存的存储，在加载时检查是否过期
        storage_->cleanExpired();
    }

    std::string SessionManager::getSessionIdFromCookie(const HttpRequest& req)
    {
        std::string sessionId;
        std::string cookie = req.getHeader("Cookie");
        if(!cookie.empty())
        {
            size_t pos = cookie.find("sessionId=");
            if(pos != std::string::npos)
            {
                pos += 10;
                size_t end = cookie.find(";", pos);
                if(end != std::string::npos)
                {
                    sessionId = cookie.substr(pos,end - pos);
                }
                else
                {
                    sessionId = cookie.substr(pos,end - pos);
                }
            }
        }
        return sessionId;
    }

    void SessionManager::setSessionCookie(const std::string& sessionId, HttpResponse* resp)
    {
        std::string cookie = "sessionId=" + sessionId + "; path=/; HttpOnly";
        resp->addHeader("Set-Cookie",cookie);
    }
}
}