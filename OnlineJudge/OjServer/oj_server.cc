#include <iostream>
#include "../Common/httplib.h"
#include "oj_control.hpp"

using namespace httplib;
using namespace ns_control;
int main()
{
    // 用户请求的服务路由功能 - 根据用户不同需求, 为 用户提供不同的内容
    Server svr;
    Control ctrl;
    // 1.获取所有题目列表
    // 返回一张有所有题目列表的网页
    svr.Get("/all_questions", [&ctrl](const Request &req, Response &resp)
            {
                std::string html;
                ctrl.AllQuestions(&html);
                resp.set_content(html, "text/html; charset=utf-8"); });

    // 2.根据题目编号获取题目内容
    // 正则表达式
    svr.Get(R"(/question/(\d+))", [&ctrl](const Request &req, Response &resp)
            { 
                std::string number = req.matches[1];
                std::string html;
                ctrl.OneQuestion(number, &html);
                resp.set_content(html, "text/html; charset=utf-8"); });

    // 3.用户提交代码, 使用判题功能(每道题的测试用例, compile_and_run)
    svr.Get(R"(/judge/(\d+))", [](const Request &req, Response &resp)
            { std::string num = req.matches[1]; 
                resp.set_content("指定题目的判题" + num, "text/plain; charset=utf-8"); });
    svr.set_base_dir("./wwwroot");
    svr.listen("0.0.0.0", 8080);
    return 0;
}