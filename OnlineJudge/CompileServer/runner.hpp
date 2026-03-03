// 提供运行服务
#pragma once
#include <iostream>
#include <string>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <fcntl.h>
#include "../Common/log.hpp"
#include "../Common/util.hpp"

namespace ns_runner
{
    using namespace ns_log;
    using namespace ns_util;

    class Runner
    {
    public:
        Runner()
        {
        }
        ~Runner()
        {
        }
        
        // 设置进程占用的cpu和内存资源上限
        static void SetProcLimit(int cpu_limit, int mem_limit)
        {
            // 设置cpu时长
            struct rlimit cpu_rlimit;
            cpu_rlimit.rlim_cur = cpu_limit;
            cpu_rlimit.rlim_max = RLIM_INFINITY;
            setrlimit(RLIMIT_CPU, &cpu_rlimit);

            // 设置内存大小
            struct rlimit mem_rlimit;
            mem_rlimit.rlim_cur = mem_limit * 1024; // 单位kb
            mem_rlimit.rlim_max = RLIM_INFINITY;
            setrlimit(RLIMIT_AS, &mem_rlimit);
        }
        // 指定文件名称编译, 无需后缀
        // 程序运行三种情况: 1.代码跑完结果对, 2.代码跑完结果不对, 3.代码没跑完,异常
        // Run运行模块, 只考虑代码是否运行完毕即可. 不需要考虑代码运行完结果的正确与否, 代码的运行结果由测试用例决定

        // 运行时,形成三个文件: 1.标准输入(不做处理) 2.标准输出 3. 标准错误

        // 返回值 > 0: 程序异常,返回值代表信号值  ==0: 正常运行完毕,结果保存至临时文件  <0: 内部错误

        // cpu_limit: 程序运行时最大的cpu资源上限,  mem_limit: 程序运行时最大的内存上限(kb)
        static int Run(const std::string &file_name, int cpu_limit, int mem_limit)
        {
            std::string _exectue = PathUtil::Exe(file_name); // 可执行程序名
            std::string _stdin = PathUtil::Stdin(file_name);
            std::string _stdout = PathUtil::Stdout(file_name);
            std::string _stderr = PathUtil::Stderr(file_name);

            // 创建 + 打开文件
            umask(0);
            int _stdin_fd = open(_stdin.c_str(), O_CREAT | O_RDONLY, 0644);
            int _stdout_fd = open(_stdout.c_str(), O_CREAT | O_WRONLY, 0644);
            int _stderr_fd = open(_stderr.c_str(), O_CREAT | O_WRONLY, 0644);
            if (_stdin_fd < 0 || _stdout_fd < 0 || _stderr_fd < 0)
            {
                LOG(ERROR) << "运行时标准文件打开失败\n";
                return -1; // -1 文件打开失败
            }
            // 在子进程中运行
            pid_t pid = fork();
            if (pid < 0)
            {
                LOG(ERROR) << "子进程创建失败\n";
                close(_stdin_fd);
                close(_stdout_fd);
                close(_stderr_fd);
                return -2; // -2 子进程创建失败
            }
            else if (pid == 0)
            {
                SetProcLimit(cpu_limit, mem_limit);
                dup2(_stdin_fd, 0);
                dup2(_stdout_fd, 1);
                dup2(_stderr_fd, 2);

                execl(_exectue.c_str(), _exectue.c_str(), nullptr);
                exit(1);
            }
            else
            {
                close(_stdin_fd);
                close(_stdout_fd);
                close(_stderr_fd);
                int status = 0;
                waitpid(pid, &status, 0);
                // 程序运行异常,一定是收到了信号
                LOG(INFO) << "运行完毕, 退出信号: " << (status & 0x7F) << "\n";
                return status & 0x7F;
            }
        }
    };
}