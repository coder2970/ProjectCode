#pragma once
// 日志落地模块
#include "util.hpp"
#include <fstream>
#include <memory>
#include <cassert>
#include <sstream>

namespace ns_log
{
    // 抽象类
    class LogSink
    {
    public:
        using ptr = std::shared_ptr<LogSink>;
        virtual ~LogSink() {}
        virtual void Log(const char *data, size_t len) = 0;
    };

    // 落地方向:标准输出 指定文件 滚动文件
    class StdoutSink : public LogSink
    {
    public:
        // 将日志消息写入到标准输出
        void Log(const char *data, size_t len) override
        {
            std::cout.write(data, len);
        }
    };
    class FileSink : public LogSink
    {
    public:
        // 接收 + 打开文件
        FileSink(const std::string &pathname) : _pathname(pathname)
        {
            util::File::CreateDirectory(util::File::Path(_pathname));
            _ofs.open(_pathname, std::ios::binary | std::ios::app);
            assert(_ofs.is_open());
        }

        // 将日志写到指定文件
        void Log(const char *data, size_t len) override
        {
            _ofs.write(data, len);
            assert(_ofs.good());
        }

    private:
        std::string _pathname;
        std::ofstream _ofs; // 输出文件的操作句柄
    };

    class RollBySizeSink : public LogSink
    {
    private:
        std::string CreateNewFile()
        {
            time_t t = util::Date::Now();
            struct tm lt;
            localtime_r(&t, &lt);
            std::stringstream filename;
            filename << _basename;
            filename << lt.tm_year + 1900;
            filename << lt.tm_mon + 1;
            filename << lt.tm_mday;
            filename << lt.tm_hour;
            filename << lt.tm_min;
            filename << lt.tm_sec;
            filename << "-";
            filename << _name_count++;

            filename << ".log";
            return filename.str();
        }

    public:
        RollBySizeSink(const std::string &basename, size_t max_size)
            : _basename(basename), _max_filesize(max_size), _cur_filesize(0), _name_count(0)
        {
            std::string pathname = CreateNewFile();
            util::File::CreateDirectory(util::File::Path(pathname));
            _ofs.open(pathname, std::ios::binary | std::ios::app);
            assert(_ofs.is_open());
        }
        // 将日志写到指定文件, 写入前判断文件大小,超过则切换
        void Log(const char *data, size_t len) override
        {
            if(_cur_filesize >= _max_filesize)
            {
                _ofs.close();// 关闭原文件
                std::string pathname = CreateNewFile();
                _ofs.open(pathname, std::ios::binary | std::ios::app);
                assert(_ofs.is_open());
                _cur_filesize = 0;
            }
            _ofs.write(data, len);
            assert(_ofs.good());
            _cur_filesize += len;
        }

    private:
        std::string _basename; // 基础路径 ./logs/bas-    ->  ./logs/bas-20000101010101.log
        std::ofstream _ofs;    // 输出文件的操作句柄
        size_t _max_filesize;  // 记录最大大小,超过则切换
        size_t _cur_filesize;  // 记录当前文件大小

        size_t _name_count; // 名称计数器
    };

    // 日志落地工厂
    class SinkFactory
    {
        public:
            template <class SinkType, class... Args>
            static LogSink::ptr create(Args &&...args)
            {
                return std::make_shared<SinkType>(std::forward<Args>(args)...);
            }
    };
}
