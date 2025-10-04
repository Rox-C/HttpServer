#pragma once 
#include "SslTypes.h"
#include <string>
#include <vector>

namespace ssl
{
class SslConfig
{
public:
    SslConfig();
    ~SslConfig() = default;

    //证书配置
    void setCertificateFile(const std::string& certFile){certFile_ = certFile;}
    void setPrivateKeyFile(const std::string& keyFile){keyFile_ = keyFile;}
    void setCertificateChainFile(const std::string& chainFile){chainFile_ = chainFile;}

    //协议版本和加密套件配置
    void setProtocolVersion(SSLVersion version){version_ = version;}
    void setCipherList(const std::string& cipherList){cipherList_ = cipherList;}

    //客户端验证配置
    void setVerifyClient(bool verifyClient){verifyClient_ = verifyClient;}
    void setVerifyDepth(int depth){verifyDepth_ = depth;}

    //会话配置
    void setSessionTimeout(int timeout){sessionTimeout_ = timeout;}
    void setSessionCacheSize(long size){sessionCacheSize_ = size;}

    //Getters
    const std::string& getCertificateFile() const { return certFile_; }
    const std::string& getPrivateKeyFile() const { return keyFile_; }
    const std::string& getCertificateChainFile() const { return chainFile_; }
    SSLVersion getProtocolVersion() const { return version_; }
    const std::string& getCipherList() const { return cipherList_; }
    bool getVerifyClient() const { return verifyClient_; }
    int getVerifyDepth() const { return verifyDepth_; }
    int getSessionTimeout() const { return sessionTimeout_; }
    long getSessionCacheSize() const { return sessionCacheSize_; }

private:
    std::string certFile_;//证书文件
    std::string keyFile_;//密钥文件
    std::string chainFile_;//链文件
    SSLVersion  version_;//ssl版本
    std::string cipherList_;//密码列表
    bool        verifyClient_;
    int         verifyDepth_;
    int         sessionTimeout_;
    long        sessionCacheSize_;
};
}