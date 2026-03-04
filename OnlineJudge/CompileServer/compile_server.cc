// 基于网络请求式的编译运行服务
#include <iostream>
#include <string>
#include "compile_run.hpp"

using namespace ns_compile_and_run;

int main()
{
    std::string in_json;
    Json::Value in_value;
    in_value["code"] = R"(
        #include <iostream>
        int main()
        {
            std::cout << "hello" << std::endl;
            iiijfafafa;
            return 0;
        }
    )";
    in_value["input"] = "";
    in_value["cpu_limit"] = 1;
    in_value["mem_limit"] = 10240 * 3; // 30M
    Json::FastWriter writer;
    in_json = writer.write(in_value);
    std::cout << in_json << std::endl;
    std::string out_json;
    CompileAndRun::Start(in_json, &out_json);
    std::cout << out_json << std::endl;
    
    return 0;
}