#pragma once
#include "../Common/util.hpp"
#include "../Common/log.hpp"
#include <iostream>
#include <string>
#include <cassert>
#include <vector>
#include <unordered_map>
#include <fstream>

// 根据list文件, 加载所有的题目到内存中
// model: 主要来和数据交互,对外提供访问数据的接口

namespace ns_model
{
    using namespace ns_log;
    using namespace ns_util;
    // 题目细节
    struct Question
    {
        std::string number; // 题目编号
        std::string title;  // 题目标题
        std::string star;   // 难度: 简单中等困难
        int cpu_limit;      // 时间要求
        int mem_limit;      // 空间要求 kb
        std::string desc;   // 题目描述
        std::string header; // 题目预设给用户的代码
        std::string tail;   // 题目的测试用例, 和header拼接形成完整代码
    };
    const std::string question_list = "./questions/questions.list";
    const std::string question_path = "./questions/";
    class Model
    {
    public:
        Model() 
        { 
            assert(LoadQuestionList(question_list)); 
        }
        ~Model() 
        {
        }
        // 加载题目的配置文件 questions/question.list
        bool LoadQuestionList(const std::string &question_list)
        {
            // 打开文件
            std::ifstream in(question_list);
            if (!in.is_open())
            {
                LOG(FATAL) << "加载题库配置文件失败, 检查配置文件" << "\n";
                return false;
            }
            // 1.按行读取
            std::string line;
            while(getline(in, line))
            {
                // 2.对读到的每一行进行切分, 切分成五部分
                std::vector<std::string> tokens;
                StringUtil::SplitString(line, &tokens, " ");
                if(tokens.size() != 5)
                {
                    LOG(WARING) << "加载部分题目描述出错, 请检查文件格式" << "\n";
                    continue;
                }
                // 切分成功, 填充question
                Question q;
                q.number = tokens[0];
                q.title = tokens[1];
                q.star = tokens[2];
                q.cpu_limit = std::atoi(tokens[3].c_str());
                q.mem_limit = std::atoi(tokens[4].c_str());

                // 每道题对应的路径
                std::string path = question_path;
                path += q.number;
                path += "/";

                FileUtil::ReadFile(path + "desc.txt", &(q.desc), true);
                FileUtil::ReadFile(path + "header.cpp", &(q.header), true);
                FileUtil::ReadFile(path + "tail.cpp", &(q.tail), true);

                // 至此读到了一个question, 将其填充到_questions中
                _questions.insert({q.number, q});
            }
            LOG(INFO) << "加载题库成功" << "\n";
            in.close();

            return true;
        }

        // 获取全部题目
        bool GetAllQuestions(std::vector<Question> *out)
        {
            if (_questions.size() == 0)
            {
                LOG(ERROR) << "用户获取题库失败" << "\n";
                return false;
            }
            for (const auto &q : _questions)
            {
                out->push_back(q.second);
            }
            return true;
        }
        // 获取一个题目
        bool GetOneQuestion(const std::string &number, Question *q)
        {
            const auto &iter = _questions.find(number);
            if (iter == _questions.end())
            {
                LOG(ERROR) << "用户获取题目失败, 题号: " << number << "\n";
                return false;
            }
            (*q) = iter->second;
            return true;
        }

    private:
        // 建立题号:题目细节的映射关系
        std::unordered_map<std::string, Question> _questions;
    };
}