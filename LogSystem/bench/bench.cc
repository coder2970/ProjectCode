#include "../Logs/log.h"
#include <chrono>
void bench(const std::string &logger_name, size_t thr_count, size_t msg_count, size_t msg_len)
{
    // 1.获取日志器
    ns_log::Logger::ptr logger = ns_log::GetLogger(logger_name);
    if (logger.get() == nullptr)
    {
        return;
    }
    std::cout << "测试日志: " << msg_count << "条, 总大小: " << (msg_count * msg_len) / 1024 << " KB" << std::endl;
    // 2.组织指定长度的日志消息
    std::string msg(msg_len - 1, 'A'); // 留一个空行
    // 3.创建指定数量线程
    std::vector<std::thread> threads;
    std::vector<double> cost_array(thr_count);
    size_t msg_per_thr = msg_count / thr_count;
    for (int i = 0; i < thr_count; i++)
    {
        threads.emplace_back([&, i]()
                             {
        // 4.线程函数内部开始计时
        auto start = std::chrono::high_resolution_clock::now();
        // 5.开始循环写日志
        for (int j = 0; j < msg_per_thr; j++)
        {
            logger->Fatal("%s", msg.c_str());
        }
        // 6.线程函数内部结束计时
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> cost = end - start;
        cost_array[i] = cost.count();
        std::cout << "\t线程" << i << ": " << "\t输出日志数量:" << msg_per_thr << "\t耗时: " << cost.count() << std::endl;
     });
    }
    for (int i = 0; i < thr_count; i++)
    {
        threads[i].join();
    }
    // 7.计算总耗时
    double max_cost = cost_array[0];
    for (int i = 0; i < thr_count; i++)
    {
        max_cost = max_cost < cost_array[i] ? cost_array[i] : max_cost;
    }        
    size_t msg_per_sec = msg_count / max_cost;
    size_t size_per_sec = (msg_count * msg_len) / (max_cost * 1024);
    // 8.进行输出打印
    std::cout << "\t总耗时: " << max_cost << "\n";
    std::cout << "\t每秒输出日志数量: " << msg_per_sec << "条\n";
    std::cout << "\t每秒输出日志大小: " << size_per_sec << "kb\n";
}
void sync_bench()
{
    std::unique_ptr<ns_log::LoggerBuilder> builder(new ns_log::GlobalLoggerBuilder());
    builder->BuildLoggerName("sync_logger");
    builder->BuildFormatter("%m%n");
    builder->BuildLoggerType(ns_log::LoggerType::LOGGER_SYNC);
    builder->BuildSink<ns_log::FileSink>("./logfile/sync.log");
    builder->Build();
    bench("sync_logger", 3, 1000000, 100);

}
void async_bench()
{
    std::unique_ptr<ns_log::LoggerBuilder> builder(new ns_log::GlobalLoggerBuilder());
    builder->BuildLoggerName("async_logger");
    builder->BuildFormatter("%m%n");
    builder->BuildLoggerType(ns_log::LoggerType::LOGGER_ASYNC);
    builder->BuildEnableUnsafeAsync();
    builder->BuildSink<ns_log::FileSink>("./logfile/async.log");
    builder->Build();
    bench("async_logger", 3, 3000000, 100);
}
int main()
{
    async_bench();
    return 0;
}