#pragma once
#include <iostream>
#include <string>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <unistd.h>

// 工具类
namespace ns_util
{
    const std::string temp_path = "./temp/";
    // 路径拼接类:对路径进行操作
    class PathUtil
    {
    public:
        static std::string AddSuffix(const std::string &file_name, const std::string &suffix)
        {
            std::string path_name = temp_path + file_name + suffix;
            return path_name;
        }
        // Src, Exe, CompileError - 程序编译时形成的文件
        static std::string Src(const std::string &file_name)
        {
            /* 构建源文件路径+后缀的完整文件名 */
            return AddSuffix(file_name, ".cpp");
        }
        static std::string Exe(const std::string &file_name)
        {
            /* 构建可执行程序的完整路径+后缀*/
            return AddSuffix(file_name, ".exe");
        }
        // 编译时报错
        static std::string CompileError(const std::string &file_name)
        {
            return AddSuffix(file_name, ".compile_error");
        }

        // Stdin, Stdout, Stderr - 程序运行时形成的文件
        static std::string Stdin(const std::string &file_name)
        {
            return AddSuffix(file_name, ".stdin");
        }
        static std::string Stdout(const std::string &file_name)
        {
            return AddSuffix(file_name, ".stdout");
        }
        static std::string Stderr(const std::string &file_name)
        {
            /* 构建标准错误的完整路径+后缀*/
            return AddSuffix(file_name, ".stderr");
        }
    };

    // 对文件进行操作
    class FileUtil
    {
    public:
        static bool IsFileExist(const std::string &path_name)
        {
            /* 判断文件是否存在*/
            // 思路1:stat系统调用,获取文件属性成功,说明文件存在
            // 思路2:直接打开文件, 打开成功就存在
            struct stat st;
            if (stat(path_name.c_str(), &st) == 0)
            {
                // 获取属性成功,文件已经存在
                return true;
            }
            return false;
        }

        // 形成唯一文件名
        static std::string UniqueFileName()
        {
            return "";
        }

        // 向文件写入数据
        static bool WriteFile(const std::string& target, const std::string& code)
        {

        }

        // 读取文件
        static std::string& ReadFile(const std::string& target)
        {}
    };

    // 时间功能
    class TimeUtil
    {
    public:
        // 获取系统时间戳 - 使用gettimeofday接口
        static std::string GetTimeStamp()
        {
            timeval _time;
            gettimeofday(&_time, nullptr);
            return std::to_string(_time.tv_sec); // 获得秒数
        }
    };
}
