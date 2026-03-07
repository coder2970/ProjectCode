#include <iostream>
#include "../Common/httplib.h"

using namespace httplib;
int main()
{
    // 用户请求的服务路由功能 - 根据用户不同需求, 为用户提供不同的内容
    Server svr;
    // 1.获取所有题目列表
    svr.Get("/all_questions", [](const Request &req, Response &resp)
            { resp.set_content("题目列表", "text/plain; charset=utf-8"); });
    // 2.根据题目编号获取题目内容
    // 正则表达式
    svr.Get(R"(/question/(\d+))", [](const Request &req, Response &resp)
            { std::string num = req.matches[1]; 
                resp.set_content("这是指定一道题 " + num, "text/plain; charset=utf-8"); });

    // 3.用户提交代码, 使用判题功能(每道题的测试用例, compile_and_run)
    svr.Get(R"(/judge/(\d+))", [](const Request &req, Response &resp)
            { std::string num = req.matches[1]; 
                resp.set_content("指定题目的判题" + num,  "text/plain; charset=utf-8"); });
    svr.set_base_dir("./wwwroot");
    svr.listen("0.0.0.0", 8080);
    return 0;
}