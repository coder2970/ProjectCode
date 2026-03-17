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

    // 运行逻辑: 客户端请求compile_and_run服务, 请求成功后得到响应
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

    return 0;
}