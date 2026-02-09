#pragma once
/*
1. 定义枚举类,枚举出日志等级
2. 提供转化接口, 将枚举转换为对应字符串
*/

namespace ns_log
{
    class LogLevel
    {
    public:
        enum class Value
        {
            UNKNOW,
            DEBUG,
            INFO,
            WARN,
            ERROR,
            FATAL,
            OFF
        };
        static const char *ToString(LogLevel::Value level)
        {
            switch (level)
            {
            case LogLevel::Value::DEBUG:
                return "DEBUG";
            case LogLevel::Value::INFO:
                return "INFO";
            case LogLevel::Value::WARN:
                return "WARN";
            case LogLevel::Value::ERROR:
                return "ERROR";
            case LogLevel::Value::FATAL:
                return "FATAL";
            case LogLevel::Value::OFF:
                return "OFF";
            default:
                return "UNKONW";
            }
            return "UNKONW";
        }
    };
}