// 日志落地模块
// 将格式化完成后的日志消息字符串, 输出到指定位置
// 支持同时将日志落地到不同的位置
// 输出分类 : 标准输出, 指定文件, 滚动文件(文件按照时间或大小进行滚动切换)

//  1. 抽象落地模块基类
//  2. 不同落地方向从基类进行派生
//  3. 使用工厂模式进行创建与表示的分离
#pragma once
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
    // 标准输出
    class StdoutSink : public LogSink
    {
    public:
        // 将日志消息写入到标准输出
        void Log(const char *data, size_t len) override
        {
            std::cout.write(data, len); // 从data中写入len长度的数据
            // std::cout.flush();
        }
    };
    // 指定文件
    class FileSink : public LogSink
    {
    public:
        // 接收 + 打开文件
        FileSink(const std::string &pathname) : _pathname(pathname)
        {
            // 1. 创建日志文件所在目录
            util::File::CreateDirectory(util::File::Path(_pathname));
            // 2. 创建并打开日志文件
            _ofs.open(_pathname, std::ios::binary | std::ios::app);
            assert(_ofs.is_open());
        }

        // 将日志写到指定文件
        void Log(const char *data, size_t len) override
        {
            _ofs.write(data, len);
            // _ofs.flush();
            assert(_ofs.good());
        }

    private:
        std::string _pathname;
        std::ofstream _ofs; // 输出文件的操作句柄
    };

    // 滚动文件(以大小进行滚动)
    class RollBySizeSink : public LogSink
    {
    private:
        // 进行大小判断,创建新文件
        std::string CreateNewFile()
        {
            // 获取系统时间,构造扩展名
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

            filename << ".log"; // 后缀
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
            // _ofs.flush();
            assert(_ofs.good());
            _cur_filesize += len;
        }

    private:
        // 通过基础文件名 + 扩展文件名(以时间生成)组成一个实际的输出文件名
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
