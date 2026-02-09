#include "util.hpp"
#include "level.hpp"
#include "message.hpp"
#include "format.hpp"

int main()
{
    ns_log::LogMessage message(ns_log::LogLevel::Value::INFO, 53, "main.c", "root", "格式化功能测试");
    ns_log::Formatter fmt("abcfa%%dw[ %d{%H:%M:%S} ][ %t ][ %c ][ :%l ][ %p ]%T%m");
    std::string str = fmt.format(message);
    std::cout << str << std::endl;
    return 0;
}