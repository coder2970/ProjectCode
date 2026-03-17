#pragma once
#include "../Common/util.hpp"
#include "../Common/log.hpp"
#include <iostream>
#include <string>
#include <cassert>
#include <vector>
#include <unordered_map>
#include <fstream>
#include <mysql/mysql.h>
// mysql 版本
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
        std::string desc;   // 题目描述
        std::string header; // 题目预设给用户的代码
        std::string tail;   // 题目的测试用例, 和header拼接形成完整代码
        int cpu_limit;      // 时间要求
        int mem_limit;      // 空间要求 kb
    };
    const std::string oj_questions = "oj_questions";
    const std::string host = "127.0.0.1";
    const std::string user = "oj_client";
    const std::string passwd = "123456";
    const std::string db = "oj";
    const int port = 3306;
    class Model
    {
    public:
        Model()
        {
        }
        ~Model()
        {
        }
        bool QueryMySql(const std::string &sql, std::vector<Question> *out)
        {
            // 创建mysql句柄
            MYSQL *my = mysql_init(nullptr);
            if (nullptr == mysql_real_connect(my, host.c_str(), user.c_str(), passwd.c_str(), db.c_str(), port, nullptr, 0))
            {
                LOG(FATAL) << "链接数据库失败" << "\n";
                mysql_close(my);
                return false;
            }
            LOG(INFO) << "连接数据库成功" << "\n";
            mysql_set_character_set(my, "utf8mb4");

            // 执行sql语句
            if (mysql_query(my, sql.c_str()) != 0)
            {
                LOG(WARING) << sql << "excute error" << "\n";
                return false;
            }
            // 提取结果
            MYSQL_RES *res = mysql_store_result(my);
            // 分析结果
            int rows = mysql_num_rows(res);
            int cols = mysql_num_fields(res);
            for (int i = 0; i < rows; i++)
            {
                MYSQL_ROW row = mysql_fetch_row(res);
                struct Question q;
                q.number = row[0];
                q.title = row[1];
                q.star = row[2];
                q.desc = row[3];
                q.header = row[4];
                q.tail = row[5];
                q.cpu_limit = std::atoi(row[6]);
                q.mem_limit = std::atoi(row[7]);

                out->push_back(q);
            }

            // 释放空间
            mysql_free_result(res);
            // 关闭连接
            mysql_close(my);
            return true;
        }
        // 获取全部题目
        bool GetAllQuestions(std::vector<Question> *out)
        {
            std::string sql = "select * from ";
            sql += oj_questions;
            return QueryMySql(sql, out);
        }
        // 获取一个题目
        bool GetOneQuestion(const std::string &number, Question *q)
        {
            bool res = false;
            std::string sql = "select * from ";
            sql += oj_questions;
            sql += " where number = ";
            sql += number;

            std::vector<Question> result;
            if (QueryMySql(sql, &result))
            {
                if (result.size() == 1)
                {
                    *q = result[0];
                    res = true;
                }
            }
            return res;
        }

    private:
        // 建立题号:题目细节的映射关系
        std::unordered_map<std::string, Question> _questions;
    };
}