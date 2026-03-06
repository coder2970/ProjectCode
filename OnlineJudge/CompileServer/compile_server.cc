// 基于网络请求式的编译运行服务
#include <iostream>
#include <string>
#include "compile_run.hpp"
#include "../Common/httplib.h"

using namespace ns_compile_and_run;
using namespace httplib;

void Usage(std::string message)
{
    std::cerr << "usage: ./compile_server " << message << std::endl;
}
// ./compile_server port
int main(int argc, char* argv[])
{
    if(argc != 2)
    {
        Usage(argv[0]);
        exit(1);
    }
    Server svr;
    // svr.Get("/hello", [](const Request &req, Response &resp)
    //         { resp.set_content("hello httplib, 你好", "text/plain; charset=utf-8"); });
    // svr.set_base_dir("./wwwroot"); // 仅作测试, 实际使用时,只需要对代码进行编译, 把结果返回就行, 不用给用户提供网页服务

    // 运行逻辑: 客户端请求compile_and_run服务, 请求成功后得到响应(lambda表达式完成)
    svr.Post("/compile_and_run", [](const Request &req, Response &resp){
        // 用户请求 - json串
        std::string in_json = req.body;
        std::string out_json;
        if (!in_json.empty())
        {
            CompileAndRun::Start(in_json, &out_json);
            resp.set_content(out_json, "application/json;charset=utf-8");
        } 
    });
    svr.listen("0.0.0.0", std::atoi(argv[1]));

    // std::string in_json;
    // Json::Value in_value;
    // in_value["code"] = R"(
    //     #include <iostream>
    //     int main()
    //     {
    //     afas
    //         std::cout << "hello" << std::endl;
    //         // int *p = new int[1024 * 1024 * 60];
    //         return 0;
    //     }
    // )";
    // in_value["input"] = "";
    // in_value["cpu_limit"] = 1;
    // in_value["mem_limit"] = 10240 * 3; // 30M
    // Json::FastWriter writer;
    // in_json = writer.write(in_value);
    // std::cout << in_json << std::endl;
    // std::string out_json;
    // CompileAndRun::Start(in_json, &out_json);
    // // std::cout << out_json << std::endl;

    // // 解决中文打印问题
    // Json::Value out_value;
    // Json::Reader reader;
    // reader.parse(out_json, out_value);
    // std::cout << out_value["reason"].asString() << std::endl;

    return 0;
}