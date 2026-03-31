#pragma once

namespace ns_log
{
    class LogLevel
    {
    public:
        enum class Value
        {
            UNKNOW = 0,
            DEBUG,
            INFO,
            WARN,
            ERROR,
            FATAL,
            OFF
        };
        static const char *ToString(Value level)
        {
            switch (level)
            {
            case Value::DEBUG:
                return "DEBUG";
            case Value::INFO:
                return "INFO";
            case Value::WARN:
                return "WARN";
            case Value::ERROR:
                return "ERROR";
            case Value::FATAL:
                return "FATAL";
            case Value::OFF:
                return "OFF";
            default:
                return "UNKNOW";
            }
            return "UNKNOW";
        }
    };
}