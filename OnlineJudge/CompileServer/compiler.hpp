#pragma once

#include <iostream>
#include <string>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include "../Common/util.hpp"
// 只负责提供代码编译功能

namespace ns_compiler
{
    // 引入路劲拼接功能
    using namespace ns_util;

    class Compiler
    {
    public:
        Compiler()
        {
        }

        ~Compiler()
        {
        }

        // 返回值:编程是否成功
        // 参数: 编译文件的文件名test
        // test -> temp/test.cpp 源文件
        // test -> temp/test.exe 成功可执行文件
        // test -> temp/test.error 执行出错文件
        static bool Compile(const std::string &file_name)
        {
            pid_t pid = fork();
            if (pid < 0)
            {
                /* 创建失败 */
                return false;
            }
            else if (pid == 0)
            {
                int _stderr = open(PathUtil::Stderr(file_name).c_str(), O_CREAT | O_WRONLY, 0644);
                if(_stderr < 0)
                {
                    exit(1);
                }
                // 重定向标准错误到stderr文件中
                dup2(_stderr, 2);
                
                /* 子进程 */
                /* 调用编译器完成对代码的编译工作 */
                execlp("g++", "-o", PathUtil::Exe(file_name).c_str(), PathUtil::Src(file_name).c_str(), "-std=c++11", nullptr);

                exit(2);
            }
            else
            {
                /* 父进程 */
                waitpid(pid, nullptr, 0);
                // 编译是否成功:只关心有没有形成对应的可执行程序
                if (FileUtil::IsFileExist(PathUtil::Exe(file_name)))
                {
                    return true;
                }
            }
            return false;
        }

    private:
    };
}