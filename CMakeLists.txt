# 最低 CMake 版本
cmake_minimum_required(VERSION 3.10)
# 项目信息
project(HttpServer)

# 1 . 指定 C++11 标准
# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 编译选项
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -O2")

# 链接选项：支持 pthread
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -pthread")

# 2 . 添加目录
# 头文件目录

include_directories(
    #项目自身头文件目录（include/ 和 app/include）
    ${PROJECT_SOURCE_DIR}/include
    ${PROJECT_SOURCE_DIR}/app/include
    #系统头文件目录
    /usr/include
    /usr/include/mysql
    /usr/include/nlohmann
)

find_package(OpenSSL REQUIRED)
#3. cpp源文件收集

file(GLOB_RECURSE FRAMEWORK_SRC
    ${PROJECT_SOURCE_DIR}/src/*.cpp
)

file(GLOB_RECURSE APP_SRC
    ${PROJECT_SOURCE_DIR}/app/src/*.cpp
)

set(MAIN_SRC
    ${PROJECT_SOURCE_DIR}/main.cpp
)

# 4. 添加可执行文件
add_executable(http_server
    ${MAIN_SRC}
    ${FRAMEWORK_SRC}
    ${APP_SRC}
)

# 5. 链接库
target_link_libraries(http_server
    muduo_net
    muduo_base
    mariadb
    mysqlclient
    OpenSSL::SSL
    OpenSSL::Crypto
)

# 6.打印信息
message(STATUS "=== HttpServer 编译配置 ===")
message(STATUS "C++ 标准: C++${CMAKE_CXX_STANDARD}")
message(STATUS "头文件路径:")
get_property(INCLUDE_DIRS DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY INCLUDE_DIRECTORIES)
foreach(dir ${INCLUDE_DIRS})
    message(STATUS "  - ${dir}")
endforeach()
message(STATUS "可执行文件: ${PROJECT_BINARY_DIR}/http_server")
message(STATUS "===========================")