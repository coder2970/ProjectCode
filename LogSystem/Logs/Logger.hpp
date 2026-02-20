// 日志器模块:
// 对之前所有的模块进行整合, 向外提供接口,完成不同等级日志的输出
// 管理成员:
// 格式化模块对象, 落地模块对象数组(一个日志器可能向多个位置进行输出), 默认输出限制等级(大于等于限制等级的才能输出), 互斥锁(多线程的安全性), 日志器名称
// 提供操作:
// debug等级的日志输出,info, waring...等的输出操作,不同等级封装不同的日志消息
// 实现:
// 抽象Logger基类(同步日志器和异步日志器)
// 两种不同的日志器只有落地方式不同,因此将落地操作抽象出来
// 不同日志器调用各自的落地操作进行日志落地
// 使用基类指针对子类日志器对象进行管理和操作
#pragma once

// #define _GNU_SOURCE

#include "util.hpp"
#include "level.hpp"
#include "format.hpp"
#include "sink.hpp"
#include <atomic>
#include <thread>
#include <mutex>
#include <cstdarg>

namespace ns_log
{
    class Logger
    {
    protected:
        // 格式化消息
        void Serialize(LogLevel::Value level, const std::string &filename, size_t line, char *str)
        {
            // 3.
            LogMessage message(level, line, filename, _logger_name, str);
            // 4.
            std::stringstream ss;
            _formatter->format(ss, message);
            // 5.
            Log(ss.str().c_str(), ss.str().size());
        }
        // 抽象接口完成实际的落地输出,不同日志器对应不同的落地方式
        virtual void Log(const char *data, size_t len) = 0;

    public:
        Logger(const std::string &logger_name, LogLevel::Value level, Formatter::ptr &formatter, std::vector<LogSink::ptr> &sinks)
            : _logger_name(logger_name), _limit_level(level), _formatter(formatter), _sinks(sinks.begin(), sinks.end())
        {
        }
        using ptr = std::shared_ptr<Logger>;
        // 完成日志消息对象的构造,并进行格式化,得到格式后的日志消息字符串
        void Debug(const std::string &filename, size_t line, const std::string &fmt, ...)
        {
            // 通过传入参数构造日志消息对象,进行日志格式化
            // 1.判断当前日志等级是否达到输出等级
            // 2.对fmt格式化字符串和不定参进行字符串组织,得到日志消息字符串
            // 3.构造LogMessage对象
            // 4.通过格式化工具,对LogMessage进行格式化,得到格式化后的字符串
            // 5.进行日志落地

            // 1.
            if (LogLevel::Value::DEBUG < _limit_level)
            {
                return;
            }
            // 2.
            va_list ap;
            va_start(ap, fmt);
            char *res;
            int ret = vasprintf(&res, fmt.c_str(), ap);
            if (ret == -1)
            {
                std::cout << "vasprintf failed!" << std::endl;
                return;
            }
            // 此时res为格式化之后的字符串
            va_end(ap);
            // 3. 4. 5.
            Serialize(LogLevel::Value::DEBUG, filename, line, res);
            free(res);
        }
        void Info(const std::string &filename, size_t line, const std::string &fmt, ...)
        {
            // 1.
            if (LogLevel::Value::INFO < _limit_level)
            {
                return;
            }
            // 2.
            va_list ap;
            va_start(ap, fmt);
            char *res;
            int ret = vasprintf(&res, fmt.c_str(), ap);
            if (ret == -1)
            {
                std::cout << "vasprintf failed!" << std::endl;
                return;
            }
            // 此时res为格式化之后的字符串
            va_end(ap);
            // 3. 4. 5.
            Serialize(LogLevel::Value::INFO, filename, line, res);
            free(res);
        }
        void Waring(const std::string &filename, size_t line, const std::string &fmt, ...)
        {
            // 1.
            if (LogLevel::Value::WARN < _limit_level)
            {
                return;
            }
            // 2.
            va_list ap;
            va_start(ap, fmt);
            char *res;
            int ret = vasprintf(&res, fmt.c_str(), ap);
            if (ret == -1)
            {
                std::cout << "vasprintf failed!" << std::endl;
                return;
            }
            // 此时res为格式化之后的字符串
            va_end(ap);
            // 3. 4. 5.
            Serialize(LogLevel::Value::WARN, filename, line, res);
            free(res);
        }
        void Error(const std::string &filename, size_t line, const std::string &fmt, ...)
        {
            // 1.
            if (LogLevel::Value::ERROR < _limit_level)
            {
                return;
            }
            // 2.
            va_list ap;
            va_start(ap, fmt);
            char *res;
            int ret = vasprintf(&res, fmt.c_str(), ap);
            if (ret == -1)
            {
                std::cout << "vasprintf failed!" << std::endl;
                return;
            }
            // 此时res为格式化之后的字符串
            va_end(ap);
            // 3. 4. 5.
            Serialize(LogLevel::Value::ERROR, filename, line, res);
            free(res);
        }
        void Fatal(const std::string &filename, size_t line, const std::string &fmt, ...)
        {
            // 1.
            if (LogLevel::Value::FATAL < _limit_level)
            {
                return;
            }
            // 2.
            va_list ap;
            va_start(ap, fmt);
            char *res;
            int ret = vasprintf(&res, fmt.c_str(), ap);
            if (ret == -1)
            {
                std::cout << "vasprintf failed!" << std::endl;
                return;
            }
            // 此时res为格式化之后的字符串
            va_end(ap);
            // 3. 4. 5.
            Serialize(LogLevel::Value::FATAL, filename, line, res);
            free(res);
        }

    protected:
        std::mutex _mutex;                         // 锁
        std::string _logger_name;                  // 日志器名称
        std::atomic<LogLevel::Value> _limit_level; // 限制等级
        Formatter::ptr _formatter;                 // 格式化器
        std::vector<LogSink::ptr> _sinks;          // 落地方向
    };

    // 同步日志器
    class SyncLogger : public Logger
    {
    public:
        SyncLogger(const std::string &logger_name, LogLevel::Value level, Formatter::ptr &formatter, std::vector<LogSink::ptr> &sinks)
            : Logger(logger_name, level, formatter, sinks)
        {
        }

    protected:
        // 将日志直接通过落地模块句柄进行日志落地
        void Log(const char *data, size_t len)
        {
            std::unique_lock<std::mutex> lock(_mutex);
            if (_sinks.empty())
            {
                return;
            }
            for (auto &sink : _sinks)
            {
                sink->Log(data, len);
            }
        }
    };

    // 建造者模式 构造 日志器, 简化用户使用的复杂度
    // 1.抽象日志器建造者类
    // 设置日志器类型, 将不同类型日志器放到同一个日志器建造者类中完成
    // 2.派生出具体的建造者类 -- 局部日志器建造者和全局日志器建造者
    enum class LoggerType
    {
        LOGGER_SYNC,
        LOGGER_ASYNC
    };
    // 1.
    class LoggerBuilder
    {
    public:
        LoggerBuilder() : _logger_type(LoggerType::LOGGER_SYNC), _limit_level(LogLevel::Value::DEBUG)
        {
        }
        void BuildLoggerType(LoggerType type)
        {
            _logger_type = type;
        }

        void BuildLoggerName(const std::string &name)
        {
            _logger_name = name;
        }

        void BuildLoggerLevel(LogLevel::Value level)
        {
            _limit_level = level;
        }

        void BuildFormatter(const std::string &pattern)
        {
            _formatter = std::make_shared<Formatter>(pattern);
        }
        template <typename SinkType, typename... Args>
        void BuildSink(Args &&...args)
        {
            LogSink::ptr psink = SinkFactory::create<SinkType>(std::forward<Args>(args)...);
            _sinks.push_back(psink);
        }
        virtual Logger::ptr Build() = 0;

    protected:
        LoggerType _logger_type;
        std::string _logger_name;
        LogLevel::Value _limit_level;
        Formatter::ptr _formatter;
        std::vector<LogSink::ptr> _sinks;
    };

    // 2.
    class LocalLoggerBuilder : public LoggerBuilder
    {
    public:
        Logger::ptr Build() override
        {
            assert(_logger_name.empty() == false); // 必须要有日志器名称
            if (_formatter.get() == nullptr)
            {
                // 没有日志器
                _formatter = std::make_shared<Formatter>();
            }
            if (_sinks.empty())
            {
                // 默认添加标准输出的落地器
                BuildSink<StdoutSink>();
            }
            if (_logger_type == LoggerType::LOGGER_ASYNC)
            {

            }
            // 同步日志器
            return std::make_shared<SyncLogger>(_logger_name, _limit_level, _formatter, _sinks); 
        }
    };
}