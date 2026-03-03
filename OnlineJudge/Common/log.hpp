#pragma once

#include <string>
#include <iostream>
#include "util.hpp"

// 日志输出模块
namespace ns_log
{
    using namespace ns_util;
    // 日志等级
    enum
    {
        INFO,
        DEBUG,
        WARING,
        ERROR,
        FATAL
    };
    // inline 作用
    inline std::ostream &Log(const std::string &level, const std::string &file_name, int line)
    {
        // 添加日志等级
        std::string message = "[";
        message += level;
        message += "]";
        // 添加报错文件名
        message += "[";
        message += file_name;
        message += "]";
        // 添加报错行号
        message += "[";
        message += std::to_string(line);
        message += "]";
        // 添加时间戳
        message += "[";
        message += TimeUtil::GetTimeStamp();
        message += "]";

        // 写入cout缓冲区, 不刷新(不写endl), 提供开放式输出
        std::cout << message;
        return std::cout;
    }
// #作用?
#define LOG(level) Log(#level, __FILE__, __LINE__)
}