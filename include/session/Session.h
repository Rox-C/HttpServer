#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <chrono>

namespace http
{
namespace session
{

class SessionManager;

class Session : public std::enable_shared_from_this<Session>
{
public:
    Session(const std::string& sessionId, SessionManager* sessionManager, int maxAge = 3600);

    const std::string& getSessionId() const
    {
        return sessionId_;
    }

    bool isExpired() const;
    void refresh();

    void setManager(SessionManager* sessionManager)
    {
        sessionManager_ = sessionManager;
    }

    SessionManager* getManager() const
    {
        return sessionManager_;
    }
    //数据存取
    void setValue(const std::string& key, const std::string& value);
    std::string getValue(const std::string& key) const;   
    void remove(const std::string& key);
    void clear(); 
private:
    std::string                                 sessionId_; //会话ID
    std::unordered_map<std::string,std::string> data_; //会话数据
    std::chrono::system_clock::time_point       expireTime_; //过期时间(相对时间)
    int                                         maxAge_;//会话超时时间(秒)
    SessionManager*                             sessionManager_;
};

}
}