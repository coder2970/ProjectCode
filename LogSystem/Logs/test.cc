#include "util.hpp"
#include "level.hpp"
#include "message.hpp"
#include "format.hpp"
#include "sink.hpp"
#include "Logger.hpp"

int main()
{
    // std::string loggername = "synclogger";
    // ns_log::LogLevel::Value limit = ns_log::LogLevel::Value::WARN;
    // ns_log::Formatter::ptr fmt(new ns_log::Formatter("[%d{%H:%M:%S}][%c][%f:%l][%p]%T%m%n"));
    // ns_log::LogSink::ptr stdout_lsp = ns_log::SinkFactory::create<ns_log::StdoutSink>();
    // ns_log::LogSink::ptr file_lsp = ns_log::SinkFactory::create<ns_log::FileSink>("./logfile/test.log");
    // ns_log::LogSink::ptr roll_lsp = ns_log::SinkFactory::create<ns_log::RollBySizeSink>("./logfile/roll-", 1024 * 1024);
    // std::vector<ns_log::LogSink::ptr> sinks = {stdout_lsp, file_lsp, roll_lsp};
    // ns_log::Logger::ptr logger(new ns_log::SyncLogger(loggername, limit, fmt, sinks));

    std::unique_ptr<ns_log::LoggerBuilder> builder(new ns_log::LocalLoggerBuilder);
    builder->BuildLoggerName("sync_logger");
    builder->BuildLoggerLevel(ns_log::LogLevel::Value::WARN);
    builder->BuildFormatter("%m%n");
    builder->BuildLoggerType(ns_log::LoggerType::LOGGER_SYNC);
    builder->BuildSink<ns_log::StdoutSink>();
    builder->BuildSink<ns_log::FileSink>("./logfile/test.log");
    builder->BuildSink<ns_log::RollBySizeSink>("./logfile/roll-", 1024 * 1024);
    ns_log::Logger::ptr logger = builder->Build();

    logger->Debug(__FILE__, __LINE__, "%s", "测试日志");
    logger->Info(__FILE__, __LINE__, "%s", "测试日志");
    logger->Waring(__FILE__, __LINE__, "%s", "测试日志");
    logger->Error(__FILE__, __LINE__, "%s", "测试日志");
    logger->Fatal(__FILE__, __LINE__, "%s", "测试日志");

    size_t cursize = 0;
    size_t count = 0;
    while (cursize < 1024 * 1024 * 10)
    {
        logger->Fatal(__FILE__, __LINE__, "测试日志-%d", count++);
        cursize += 20;
    }
     
    // ns_log::LogMessage message(ns_log::LogLevel::Value::INFO, 53, "main.c", "root", "格式化功能测试");
    // ns_log::Formatter fmt;
    // std::string str = fmt.format(message);
    // ns_log::LogSink::ptr stdout_lsp = ns_log::SinkFactory::create<ns_log::StdoutSink>();
    // ns_log::LogSink::ptr file_lsp = ns_log::SinkFactory::create<ns_log::FileSink>("./logfile/test.log");
    // ns_log::LogSink::ptr roll_lsp = ns_log::SinkFactory::create<ns_log::RollBySizeSink>("./logfile/roll-", 1024 * 1024);

    // stdout_lsp->Log(str.c_str(), str.size());
    // file_lsp->Log(str.c_str(), str.size());
    // size_t cursize = 0;
    // size_t count = 0;
    // while (cursize < 1024 * 1024 * 10)
    // {
    //     std::string tmp = str + std::to_string(count++);
    //     roll_lsp->Log(tmp.c_str(), tmp.size());
    //     cursize += tmp.size();
    // }

    // std::cout << str << std::endl;
    return 0;
}