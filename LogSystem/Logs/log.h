#pragma once
#include "Logger.hpp"
// 1.提供获取指定日志器的全局接口
// 2.使用宏函数对日志器的接口进行代理
// 3. 提供宏函数,直接通过默认日志器进行日志的标准输出打印
namespace ns_log
{
    Logger::ptr GetLogger(const std::string &name)
    {
        return ns_log::LoggerManager::getInstanence().GetLogger(name);
    }
    Logger::ptr RootLogger()
    {
        return ns_log::LoggerManager::getInstanence().RootLogger();
    }
#define Debug(fmt, ...) Debug(__FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define Info(fmt, ...) Info(__FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define Warning(fmt, ...) Warning(__FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define Error(fmt, ...) Error(__FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define Fatal(fmt, ...) Fatal(__FILE__, __LINE__, fmt, ##__VA_ARGS__)

#define DEBUG(fmt, ...) ns_log::RootLogger()->Debug(fmt, ##__VA_ARGS__)
#define INFO(fmt, ...) ns_log::RootLogger()->Info(fmt, ##__VA_ARGS__)
#define WARN(fmt, ...) ns_log::RootLogger()->Warning(fmt, ##__VA_ARGS__)
#define ERROR(fmt, ...) ns_log::RootLogger()->Error(fmt, ##__VA_ARGS__)
#define FATAL(fmt, ...) ns_log::RootLogger()->Fatal(fmt, ##__VA_ARGS__)
}
