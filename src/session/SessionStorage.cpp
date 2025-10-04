#include "../include/session/SessionStorage.h"
#include <iostream>

namespace http
{
namespace session
{

    void MemorySessionStorage::save(std::shared_ptr<Session> session)
    {
        sessions_[session->getSessionId()] = session;
    }
    //通过会话ID从存储中加载数据
    std::shared_ptr<Session> MemorySessionStorage::load(const std::string& sessionId)
    {
        auto it = sessions_.find(sessionId);
        if(it != sessions_.end())
        {
            if(!it->second->isExpired())
            {
                return it->second;
            }
            else
            {
                sessions_.erase(it);
            }
        }
        return nullptr;
    }

    void MemorySessionStorage::remove(const std::string& sessionId)
    {
        sessions_.erase(sessionId);
    }

    void MemorySessionStorage::cleanExpired()
    {
        for (auto it = sessions_.begin(); it != sessions_.end(); ) {
            if (it->second->isExpired()) {
                it = sessions_.erase(it);
            } else {
                ++it;
            }
        }
    }
}
}