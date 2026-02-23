#include "log.h"
void test()
{
    // ns_log::Logger::ptr logger = ns_log::LoggerManager::getInstanence().GetLogger("async_logger");
    // logger->Debug("%s", "测试日志");
    // logger->Info("%s", "测试日志");
    // logger->Warning("%s", "测试日志");
    // logger->Error("%s", "测试日志");
    // logger->Fatal("%s", "测试日志");
    // size_t count = 0;
    // while (count < 500000)
    // {
    //     logger->Fatal("测试日志-%d", count++);
    // }
    DEBUG("%s", "测试日志");
    INFO("%s", "测试日志");
    WARN("%s", "测试日志");
    ERROR("%s", "测试日志");
    FATAL("%s", "测试日志");
    size_t count = 0;
    while (count < 5000)
    {
        FATAL("测试日志-%d", count++);
    }
}
int main()
{
    // 读取文件数据,一点一点写入缓冲区,最终将缓冲区数据写入文件,判断新文件与源文件是否一致
    // std::ifstream ifs("./logfile/test.log", std::ios::binary);
    // if (ifs.is_open() == false)
    //     return -1;
    // ifs.seekg(0, std::ios::end);
    // size_t fsize = ifs.tellg();
    // ifs.seekg(0, std::ios::beg);
    // std::string body;
    // body.resize(fsize);
    // ifs.read(&body[0], fsize);
    // if (ifs.good() == false)
    // {
    //     std::cout << "read error" << std::endl;
    //     return -1;
    // }
    // ifs.close();

    // ns_log::Buffer buffer;
    // for (int i = 0; i < body.size(); i++)
    // {
    //     buffer.Push(&body[i], 1);
    // }
    // std::ofstream ofs("./logfile/tmp.log", std::ios::binary);
    // // ofs.write(buffer.Begin(), buffer.ReadAbleSize());
    // size_t rsize = buffer.ReadAbleSize();
    // for (int i = 0; i < rsize; i++)
    // {
    //     ofs.write(buffer.Begin(), 1);
    //     buffer.MoveReader(1);
    // }
    // ofs.close();

    // std::string loggername = "synclogger";
    // ns_log::LogLevel::Value limit = ns_log::LogLevel::Value::WARN;
    // ns_log::Formatter::ptr fmt(new ns_log::Formatter("[%d{%H:%M:%S}][%c][%f:%l][%p]%T%m%n"));
    // ns_log::LogSink::ptr stdout_lsp = ns_log::SinkFactory::create<ns_log::StdoutSink>();
    // ns_log::LogSink::ptr file_lsp = ns_log::SinkFactory::create<ns_log::FileSink>("./logfile/test.log");
    // ns_log::LogSink::ptr roll_lsp = ns_log::SinkFactory::create<ns_log::RollBySizeSink>("./logfile/roll-", 1024 * 1024);
    // std::vector<ns_log::LogSink::ptr> sinks = {stdout_lsp, file_lsp, roll_lsp};
    // ns_log::Logger::ptr logger(new ns_log::SyncLogger(loggername, limit, fmt, sinks));

    std::unique_ptr<ns_log::LoggerBuilder> builder(new ns_log::GlobalLoggerBuilder());
    builder->BuildLoggerName("async_logger");
    builder->BuildLoggerLevel(ns_log::LogLevel::Value::WARN);
    builder->BuildFormatter("[%c][%f:%l]%m%n");
    builder->BuildEnableUnsafeAsync();
    builder->BuildLoggerType(ns_log::LoggerType::LOGGER_ASYNC);
    builder->BuildSink<ns_log::StdoutSink>();
    builder->BuildSink<ns_log::FileSink>("./logfile/async.log");
    // builder->BuildSink<ns_log::RollBySizeSink>("./logfile/roll-", 1024 * 1024);
    // ns_log::Logger::ptr logger = builder->Build();
    builder->Build();

    test();
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