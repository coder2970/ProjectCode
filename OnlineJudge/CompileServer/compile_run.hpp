#pragma once
#include "../Common/log.hpp"
#include "../Common/util.hpp"
#include "compiler.hpp"
#include "runner.hpp"
#include <signal.h>
#include <unistd.h>
#include <jsoncpp/json/json.h>
// 整合功能,对外提供接口
// 适配用户请求, 定制通信协议
// 正确调用compile和run
// 形成唯一文件名

namespace ns_compile_and_run
{
    using namespace ns_log;
    using namespace ns_util;
    using namespace ns_compiler;
    using namespace ns_runner;

    class CompileAndRun
    {
    public:
        CompileAndRun()
        {
        }
        ~CompileAndRun()
        {
        }
        // 将信号报错转化为相应的原因
        // < 0 : 非运行报错
        // > 0 : 运行时错误
        // = 0 : 程序运行完
        static std::string CodeToDesc(int code, const std::string &file_name)
        {
            std::string desc;
            switch (code)
            {
            case 0:
                desc = "编译运行成功";
                break;
            case 1:
                desc = "用户提交代码为空";
                break;
            case -2:
                desc = "未知错误";
                break;
            case -3:
                // desc = "代码编译错误";
                FileUtil::ReadFile(PathUtil::CompileError(file_name), &desc, true);
                break;
            case SIGABRT:
                desc = "内存超过限制";
                break;
            case SIGXCPU:
                desc = "CPU使用超时";
                break;
            case SIGFPE:
                desc = "除零错误";
            default:
                desc = "debug code: " + std::to_string(code);
                break;
            }
            return desc;
        }

        static void RemoveTempFile(const std::string& file_name)
        {
            // 清理文件的数目是不确定的
            std::string _src = PathUtil::Src(file_name);
            if(FileUtil::IsFileExist(_src))
            {
                unlink(_src.c_str());
            }
            std::string _compile_error = PathUtil::CompileError(file_name);
            if(FileUtil::IsFileExist(_compile_error))
            {
                unlink(_compile_error.c_str());
            }
            std::string _exectue = PathUtil::Exe(file_name);
            if(FileUtil::IsFileExist(_exectue))
            {
                unlink(_exectue.c_str());
            }
            std::string _stdin = PathUtil::Stdin(file_name);
            if(FileUtil::IsFileExist(_stdin))
            {
                unlink(_stdin.c_str());
            }
            std::string _stdout = PathUtil::Stdout(file_name);
            if(FileUtil::IsFileExist(_stdout))
            {
                unlink(_stdout.c_str());
            }
            std::string _stderr = PathUtil::Stderr(file_name);
            if(FileUtil::IsFileExist(_stderr))
            {
                unlink(_stderr.c_str());
            }
        }
        // 输入:1. code: 用户提交的代码 2. input: 用户代码中的输入(cin内容)(暂时不做处理) 3. cpu_limit: cpu运行时间要求 4. mem_limit: 内存运行要求
        // 输出:1. status: 状态码(0为成功, 非0失败) 2. reason: 请求结果 3. stdout: 程序运行结果(选填) 4.stderr: 程序运行完的错误结果(选填)
        static void Start(const std::string &in_json, std::string *out_json)
        {
            // 反序列化
            Json::Value in_value;
            Json::Reader reader;
            reader.parse(in_json, in_value); // todo出错

            // 创建原文件,并将代码写入
            std::string code = in_value["code"].asString();
            std::string input = in_value["input"].asString();
            int cpu_limit = in_value["cpu_limit"].asInt();
            int mem_limit = in_value["mem_limit"].asInt();

            Json::Value out_value;
            int status_code = 0;
            int run_result;
            std::string file_name;

            if (code.size() == 0)
            {
                // 代码为空
                // out_value["status"] = -1; // 代码为空
                // out_value["reason"] = "用户提交代码为空";
                status_code = -1;
                goto END;
            }
            // 只形成一个唯一的文件名file_name 如test, 不包含后缀和目录, 采用毫秒级时间戳+原子性递增唯一值方法
            file_name = FileUtil::UniqueFileName();
            if (!FileUtil::WriteFile(PathUtil::Src(file_name), code)) // 把代码写入文件, 即形成临时src原文件, 如test.cpp
            {
                // out_value["status"] = -2; // 未知错误
                // out_value["reason"] = "提交代码发生未知错误";
                status_code = -2;
                goto END;
            }

            // 编译 & 运行
            if (!Compiler::Compile(file_name))
            {
                // 编译失败
                // out_value["status"] = -3;
                // // 编译失败的原因写入了compile_error文件,将其内容写入reason中
                // out_value["reason"] = FileUtil::ReadFile(PathUtil::CompileError(file_name));
                status_code = -3;
                goto END;
            }

            run_result = Runner::Run(file_name, cpu_limit, mem_limit);
            if (run_result < 0)
            {
                // 内部错误
                // out_value["status"] = -2; // 未知错误
                // out_value["reason"] = "提交代码发生未知错误";
                status_code = -2;
            }
            else if (run_result > 0)
            {
                // out_value["status"] = run_result;               // 运行时报错
                // out_value["reason"] = SignalToDesc(run_result); // 信号->报错原因
                status_code = run_result;
            }
            else
            {
                // 运行成功
                status_code = 0;
            }
        END:
            // 根据status_code填充reason
            out_value["status"] = status_code;
            out_value["reason"] = CodeToDesc(status_code, file_name);
            if (status_code == 0)
            {
                // 整个过程全部成功,读取运行结果至stdout或stderr(程序运行成功,运行结果可能和预期不对)
                std::string _stdout;
                FileUtil::ReadFile(PathUtil::Stdout(file_name), &_stdout, true);
                out_value["stdout"] = _stdout;
                
                std::string _stderr;
                FileUtil::ReadFile(PathUtil::Stderr(file_name), &_stderr, true);
                out_value["stderr"] = _stderr;
            }

            // 序列化返回
            Json::StyledWriter writer;
            *out_json = writer.write(out_value);

            // 清理临时文件
            RemoveTempFile(file_name);
        }

    private:
    };
}