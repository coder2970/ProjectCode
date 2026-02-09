#pragma once

#include <iostream>
#include <ctime>
#include <string>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

/*
实用工具类的实现
功能:
1.获取系统时间
2.判断文件是否存在
3. 获取文件所在路径
4. 创建目录

*/
namespace ns_log
{
    namespace util
    {
        class Date
        {
        public:
            static size_t Now()
            {
                return (size_t)time(nullptr);
            }
        };

        class File
        {
        public:
            /* 判断文件是否存在 */
            static bool Exists(const std::string &pathname)
            {
                // return (access(pathname.c_str(), F_OK) == 0); // 仅Linux平台
                struct stat st;
                if (stat(pathname.c_str(), &st) < 0)
                {
                    return false;
                }
                return true;
            }

            /* 获取文件所在路径 */
            static std::string Path(const std::string &pathname)
            {
                size_t pos = pathname.find_last_of("/\\");
                if (pos == std::string::npos)
                {
                    return ".";
                }
                return pathname.substr(0, pos + 1);
            }

            /* 创建目录 */
            static void CreateDirectory(const std::string &pathname)
            {
                size_t pos = 0, idx = 0; /*标定查找的起始位置*/
                while (idx < pathname.size())
                {
                    pos = pathname.find_first_of("/\\", idx);
                    if (pos == std::string::npos)
                    {
                        mkdir(pathname.c_str(), 0777);
                    }

                    std::string parent_dir = pathname.substr(0, pos + 1);

                    if (Exists(parent_dir) == true)
                    {
                        idx = pos + 1;
                        continue;
                    }
                    mkdir(parent_dir.c_str(), 0777);
                    idx = pos + 1;
                }
            }
        };
    }
}