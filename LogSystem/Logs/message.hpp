// 日志消息类:中间存储一条日志所需的各项要素
// 1. 输出时间 -> 过滤时间
// 2. 日志等级 -> 过滤分析
// 3. 源文件名称
// 4. 源代码行号 -> 定位错误位置
// 5. 线程id -> 定位错误线程
// 6. 主题消息 -> 具体失败消息
// 7. 日志器名称 -> 支持多日志器的同时使用
#pragma once
#include <iostream>
#include <string>
#include <thread>
#include "level.hpp"
#include "util.hpp"

namespace ns_log
{
    struct LogMessage
    {
        time_t _ctime;          // 日志产生时间戳
        LogLevel::Value _level; // 日志等级
        size_t _line;           // 行号
        std::thread::id _tid;   // 线程id
        std::string _file;      // 文件名称
        std::string _logger;    // 日志器名称
        std::string _payload;   // 有效载荷 (日志主体)

        LogMessage(LogLevel::Value level, size_t line, const std::string file, const std::string logger, const std::string message)
            : _ctime(util::Date::Now()), _level(level), _line(line), _file(file), _logger(logger), _payload(message), _tid(std::this_thread::get_id())
        {
        }
    };
}