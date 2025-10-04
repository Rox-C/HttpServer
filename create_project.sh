#!/bin/bash

# 定义目标目录结构（目录部分）
DIRS=(
    "include/http"
    "include/router"
    "include/middleware"
    "include/middleware/cors"
    "include/session"
    "include/ssl"
    "include/utils"
    "include/utils/db"
    "src/http"
    "src/router"
    "src/middleware"
    "src/middleware/cors"
    "src/session"
    "src/ssl"
    "src/utils"
    "src/utils/db"
    "app/include/handlers"
    "app/src/handlers"
    "tests"
)

# 批量创建目录（-p 确保父目录不存在时也能创建）
for dir in "${DIRS[@]}"; do
    mkdir -p "$dir"
    echo "创建目录: $dir"
done

# 定义目标空文件（文件部分）
FILES=(
    # include 下的头文件
    "include/http/HttpContext.h"
    "include/http/HttpRequest.h"
    "include/http/HttpResponse.h"
    "include/http/HttpServer.h"
    "include/router/Router.h"
    "include/router/RouterHandler.h"
    "include/middleware/Middleware.h"
    "include/middleware/MiddlewareChain.h"
    "include/middleware/cors/CorsConfig.h"
    "include/middleware/cors/CorsMiddleware.h"
    "include/session/Session.h"
    "include/session/SessionManager.h"
    "include/session/SessionStorage.h"
    "include/ssl/SslContext.h"
    "include/ssl/SslConfig.h"
    "include/ssl/SslConnection.h"
    "include/utils/FileUtil.h"
    "include/utils/JsonUtil.h"
    "include/utils/db/DbConnection.h"
    "include/utils/db/DbConnectionPool.h"
    "include/utils/db/DbException.h"
    
    # src 下的源文件
    "src/http/HttpContext.cpp"
    "src/http/HttpRequest.cpp"
    "src/http/HttpResponse.cpp"
    "src/http/HttpServer.cpp"
    "src/router/Router.cpp"
    "src/middleware/MiddlewareChain.cpp"
    "src/middleware/cors/CorsMiddleware.cpp"
    "src/session/Session.cpp"
    "src/session/SessionManager.cpp"
    "src/session/SessionStorage.cpp"
    "src/ssl/SslContext.cpp"
    "src/ssl/SslConnection.cpp"
    "src/utils/FileUtil.cpp"
    "src/utils/db/DbConnection.cpp"
    "src/utils/db/DbConnectionPool.cpp"
    
    # app 下的文件
    "app/include/handlers/LoginHandler.h"
    "app/include/handlers/RegisterHandler.h"
    "app/include/handlers/BusinessHandler.h"
    "app/include/AppServer.h"
    "app/src/handlers/LoginHandler.cpp"
    "app/src/handlers/RegisterHandler.cpp"
    "app/src/handlers/BusinessHandler.cpp"
    "app/src/AppServer.cpp"
    
    # 测试文件和入口文件
    "tests/HttpServerTest.cpp"
    "tests/AppTest.cpp"
    "main.cpp"
    "CMakeLists.txt"
)

# 批量创建空文件（touch 无则创建，有则更新时间戳）
for file in "${FILES[@]}"; do
    touch "$file"
    echo "创建文件: $file"
done

echo -e "\n项目结构创建完成！"
