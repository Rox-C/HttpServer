#pragma once
#include <openssl/ssl.h>
#include "SslConfig.h"
#include <memory>
#include <muduo/base/noncopyable.h>

namespace ssl
{
class SslContext : muduo::noncopyable
{
public:
    explicit SslContext(const SslConfig& config);
    ~SslContext();

    bool inilitize();
    SSL_CTX* getNativeHandle(){return ctx_;}
private:
    bool loadCertificates();
    bool setupProtocol();
    void setupSessionCache();
    static void handleSslError(const char* msg );
private:
    SSL_CTX* ctx_;
    SslConfig config_;
};
}