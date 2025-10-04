#include "../../include/http/HttpServer.h"

#include <any>
#include <functional>
#include <memory>

namespace http
{

    //默认Http回应函数
    void defaultHttpCallback(const HttpRequest& req, HttpResponse* resp)
    {
        resp->setStatusCode(HttpResponse::k404NotFound);
        resp->setStatusMessage("Not Found");
        resp->setCloseConnection(true);
    }

    HttpServer::HttpServer(int port,
                           const std::string& name,
                           bool useSSL,
                           muduo::net::TcpServer::Option option)
            : listenAddr_(port)
            , server_(&mainLoop_, listenAddr_, name, option)
            , useSSL_(useSSL)
            , httpCallback_(std::bind(&HttpServer::handleRequest, this, std::placeholders::_1, std::placeholders::_2))
            {
                initialize();
            }
    
    //启动
    void HttpServer::start()
    {
        LOG_WARN << "HttpServer[" << server_.name() << "] starts listenning on " << listenAddr_.toIpPort();
        server_.start();
        mainLoop_.loop();
    }

    void HttpServer::initialize()
    {
        //设置回调函数
        server_.setConnectionCallback(std::bind(&HttpServer::onConnection, this, std::placeholders::_1));
        server_.setMessageCallback(std::bind(&HttpServer::onMessage, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
    }

    void HttpServer::setSslConfig(const ssl::SslConfig& config)
    {
        if(useSSL_)
        {
            sslCtx_ = std::unique_ptr<ssl::SslContext>(new ssl::SslContext(config));
            if(!sslCtx_->inilitize())
            {
                LOG_ERROR << "SSL initalize failed";
                abort();
            }
        }
    }

    void HttpServer::onConnection(const muduo::net::TcpConnectionPtr& conn)
    {
        if(conn->connected())
        {
            if(useSSL_)
            {
                auto sslConn = std::unique_ptr<ssl::SslConnection>(new ssl::SslConnection(conn, sslCtx_.get()));
                sslConn->setMessageCallback(std::bind(&HttpServer::onMessage, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
                sslConns_[conn] = std::move(sslConn);
                sslConns_[conn]->startHandshake();
            }
            conn->setContext(HttpContext());
        }
        else
        {
            if(useSSL_)
            {
                sslConns_.erase(conn);
            }
        }
    }

    void HttpServer::onMessage(const muduo::net::TcpConnectionPtr& conn, muduo::net::Buffer* buf, muduo::Timestamp receiveTime)
    {
        try
        {
            //判断是否SSL
            if(useSSL_)
            {
                LOG_INFO << "onMessage use SSL";
                //1.查找对应的SSL连接
                auto it = sslConns_.find(conn);
                if(it != sslConns_.end())
                {
                    LOG_INFO << "onMessage find SSL connection";
                    //2.SSL连接处理数据
                    it->second->onRead(conn, buf, receiveTime);
                    //3.判断SSL连接是否完成,未完成直接返回
                    if(!it->second->isHandshakeCompleted())
                    {
                        LOG_INFO << "onMessage SSL handshake not completed";
                        return;
                    }
                    //4.SSL连接处理完成，从解密缓冲区获取数据
                    muduo::net::Buffer* decryptedBuf = it->second->getDecryptedBuffer();
                    if(decryptedBuf->readableBytes() == 0)
                    {
                        return;
                    }
                    //5.处理数据
                    buf = decryptedBuf;
                    LOG_INFO << "onMessage SSL handshake completed";
                }
            }
            //HttpContext对象用于解析出buf的请求报文并把信息封装到HttpRequest对象中
            HttpContext *context = boost::any_cast<HttpContext>(conn->getMutableContext());
            if(!context->parseRequest(buf, receiveTime))
            {
                conn->send("HTTP/1.1 400 Bad Request\r\n\r\n");
                conn->shutdown();
            }
            if(context->gotAll())
            {
                onRequest(conn, context->request());
                context->reset();
            }
        }
        catch(std::exception& e)
        {
            LOG_ERROR << "Exception in Message: " << e.what();
            conn->send("HTTP/1.1 400 Bad Request\r\n\r\n");
            conn->shutdown();
        }
    }

    void HttpServer::onRequest(const muduo::net::TcpConnectionPtr& conn, const HttpRequest& req)
    {
        const std::string& connection = req.getHeader("Connection");
        bool close = connection == "close" || (req.getVersion() == "HTTP/1.0" && connection != "Keep-Alive");
        HttpResponse response(close);

        //根据请求报文信息来封装HttpResponse对象
        httpCallback_(req, &response);
        muduo::net::Buffer buf;
        response.appendToBuffer(&buf);
        LOG_INFO << "Sending response: \n" << buf.toStringPiece().as_string() << "\n";

        conn->send(&buf);
        if(response.getCloseConnection())
        {
            conn->shutdown();
        }
    }

    void HttpServer::handleRequest(const HttpRequest& req, HttpResponse* resp)
    {
        try
        {
            //处理请求前的中间件
            HttpRequest mutableReq = req;
            middlewareChain_.processBefore(mutableReq);
            if(!router_.route(mutableReq, resp))
            {
                LOG_INFO << "请求的啥，url：" << req.method() << " " << req.path();
                LOG_INFO << "未找到路由，返回404";
                resp->setStatusCode(HttpResponse::k404NotFound);
                resp->setStatusMessage("Not Found");
                resp->setCloseConnection(true);
            }
            middlewareChain_.processAfter(*resp);
        }
        catch(const HttpResponse& res)
        {
            //中间件抛出的异常
            *resp = res;
        }
        catch(const std::exception& e)
        {
            resp->setStatusCode(HttpResponse::k500InternalServerError);
            resp->setStatusMessage(e.what());
        }
    }
}