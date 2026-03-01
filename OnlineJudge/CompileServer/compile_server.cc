// 基于网络请求式的编译运行服务
#include <iostream>
#include <string>
#include "compiler.hpp"
using namespace ns_compiler;
int main()
{
    const std::string code = "code";
    Compiler::Compile(code);
    return 0;
}