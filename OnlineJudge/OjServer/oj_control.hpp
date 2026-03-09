#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <mutex>
#include <cassert>
#include "oj_model.hpp"
#include "oj_view.hpp"
#include "../Common/util.hpp"
#include "../Common/log.hpp"

// 逻辑控制 被路由功能直接使用

namespace ns_control
{
    using namespace ns_log;
    using namespace ns_util;
    using namespace ns_model;
    using namespace ns_view;

    // 负载均衡模块
    // 提供服务的主机
    class Machine
    {
    public:
        Machine()
            : _ip(""), _port(0), _load(0), _mtx(nullptr)
        {
        }
        ~Machine() {}

    public:
        std::string _ip;  // 编译服务的ip
        int _port;        // 编译服务的port
        uint64_t _load;   // 编译服务的负载
        std::mutex *_mtx; // 锁 保护负载, c++mutex禁止拷贝, 所以定义指针
    };

    const std::string service_machine = "./conf/service_machine.conf";
    // 负载均衡模块
    class LoadBalance
    {
        public:
        LoadBalance()
        {
            assert(LoadConf(service_machine));
        }
        ~LoadBalance(){}
        // 加载配置文件
        bool LoadConf(const std::string& machine_list)
        {

        }
        // 智能选择 - 从在线主机里选择负载最少的主机来提供服务
        bool SmartChoice()
        {}
        // 离线主机
        void OfflineMachine(){}  
        // 上线
        void OnlineMachine()
        {}

    private:
        std::vector<Machine> _machines; // 可以提供编译服务的所有的主机, 下标表示主机id
        std::vector<int> _online; // 所有在线的主机
        std::vector<int> _offline; // 所有离线的主机
    };

    class Control
    {
    public:
        Control() {}
        ~Control() {}
        // 根据题目数据构建网页
        bool AllQuestions(std::string *html)
        {
            bool ret = true;
            std::vector<Question> all;
            if (_model.GetAllQuestions(&all))
            {
                // 获取题目信息成功, 将题目数据构建成网页
                _view.AllExpandHtml(all, html);
            }
            else
            {
                *html = "获取题目失败, 形成题目列表失败";
                ret = false;
            }
            return ret;
        }
        bool OneQuestion(const std::string &number, std::string *html)
        {
            bool ret = true;
            Question q;
            if (_model.GetOneQuestion(number, &q))
            {
                _view.OneExpandHtml(q, html);
            }
            else
            {
                *html = "指定题目" + number + "不存在";
            }
            return ret;
        }

        // 判题功能
        // 提交内容放入json串, 同样以json串返回
        void Judge(const std::string in_json, std::string *out_json)
        {
            // in_json: id, code, input
            // 1.对in_json进行反序列化

            // 2.重新拼接用户代码和测试代码, 形成新的代码

            // 3.选择负载最低的主机

            // 4.发起http请求, 得到结果

            // 5.将结果赋值给out_json
        }

    private:
        Model _model; // 提供后台数据
        View _view;   // 提供网页渲染功能
    };
}