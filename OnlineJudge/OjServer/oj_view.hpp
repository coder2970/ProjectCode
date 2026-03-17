#pragma once
#include <iostream>
#include <string>
#include <ctemplate/template.h>
#include "oj_model2.hpp"

namespace ns_view
{
    using namespace ns_model;

    const std::string template_path = "./template_html/"; //要渲染的网页模板的路径

    class View
    {

    public:
        View() {}
        ~View() {}
        void AllExpandHtml(const std::vector<Question>& questions, std::string* html)
        {
            // 编号 标题 难度 -- 表格显示
            // 1.形成路径
            std::string src_html = template_path + "all_questions.html";
            // 2.形成数据字典
            ctemplate::TemplateDictionary root("all_questions");
            for(auto &q : questions)
            {
                // 形成root数据字典的子字典
                ctemplate::TemplateDictionary *sub = root.AddSectionDictionary("question_list");
                sub->SetValue("number", q.number);
                sub->SetValue("title", q.title);
                sub->SetValue("star", q.star);
            }
            // 3.获取被渲染的网页
            ctemplate::Template *tpl = ctemplate::Template::GetTemplate(src_html, ctemplate::DO_NOT_STRIP);
            //4. 开始完成渲染功能
            tpl->Expand(html, &root);
        }
        void OneExpandHtml(const Question& q, std::string* html)
        {
            std::string src_html = template_path + "one_question.html";
            ctemplate::TemplateDictionary root("one_question");
            root.SetValue("number", q.number);
            root.SetValue("title", q.title);
            root.SetValue("star", q.star);
            root.SetValue("desc", q.desc);
            root.SetValue("pre_code", q.header);
            ctemplate::Template *tpl = ctemplate::Template::GetTemplate(src_html, ctemplate::DO_NOT_STRIP);
            tpl->Expand(html, &root);
        }

    private:
    };
}