#include <iostream>
#include <vector>
#include <sys/time.h>
#include <sys/resource.h>
#include <ctemplate/template.h>
#include <boost/algorithm/string.hpp>

int main()
{
    // struct rlimit r;
    // r.rlim_cur = 1;
    // r.rlim_max = RLIM_INFINITY;
    // setrlimit(RLIMIT_CPU, &r);

    // struct rlimit r;
    // r.rlim_cur = 1024 * 1024 * 20;
    // r.rlim_max = RLIM_INFINITY;
    // setrlimit(RLIMIT_AS, &r);
    // int count = 1;
    // while (1)
    // {
    //     int *p = new int[1024 * 1024];
    //     std::cout << count++ << std::endl;
    // }

    // ------------------------------------------------------------
    // 使用boost库进行字符串切分
    // const std::string str = "1:判断回文:简单:1::::30000";
    // std::vector<std::string> tokens;
    // boost::split(tokens, str, boost::is_any_of(":"), boost::algorithm::token_compress_on);
    // for(auto iter : tokens)
    //     std::cout << iter << std::endl;

    // 使用ctemplate进行网页渲染
    // ------------------------------------------------------------
    std::string in_html = "./test.html";
    std::string value = "tangjunyao";

    // 形成数据字典
    ctemplate::TemplateDictionary root("test"); // 类似于unordered_map<> test
    root.SetValue("key", value);                // test.insert({});

    // 获取被渲染网页对象
    ctemplate::Template *tpl = ctemplate::Template::GetTemplate(in_html, ctemplate::DO_NOT_STRIP); // 保持网页原貌

    // 添加字典数据到网页
    std::string out_html;
    tpl->Expand(&out_html, &root);
    // 完成渲染
    std::cout << out_html << std::endl;
    return 0;
}
