#include "../include/session/Session.h"
#include "../include/session/SessionManager.h"

namespace http
{
namespace session
{

    Session::Session(const std::string& sessionId, SessionManager* sessionManager, int maxAge)
        : sessionId_(sessionId)
        , maxAge_(maxAge)
        , sessionManager_(sessionManager)
    {
        refresh();
    }

    //检查会话是否过期
    bool Session::isExpired() const
    {
        return std::chrono::system_clock::now() > expireTime_;
    }

    //刷新会话的过期时间
    void Session::refresh()
    {
        expireTime_ = std::chrono::system_clock::now() + std::chrono::seconds(maxAge_);
    }

    //设置会话数据
    void Session::setValue(const std::string& key, const std::string& value)
    {
        data_[key] = value;
        if(sessionManager_)
        {
            sessionManager_->updateSession(shared_from_this());
        }
    }

    //获取会话数据
    std::string Session::getValue(const std::string& key) const
    {
        auto iter = data_.find(key);
        return iter != data_.end() ? iter->second : "";
    }

    void Session::remove(const std::string& key)
    {
        data_.erase(key);
    }

    void Session::clear()
    {
        data_.clear();
    }

}
}