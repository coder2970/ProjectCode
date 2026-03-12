#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <mutex>
#include <fstream>
#include <cassert>
#include <algorithm>
#include <jsoncpp/json/json.h>
#include "oj_model.hpp"
#include "oj_view.hpp"
#include "../Common/util.hpp"
#include "../Common/log.hpp"
#include "../Common/httplib.h"
// 逻辑控制 被路由功能直接使用

namespace ns_control
{
    using namespace httplib;
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
        ~Machine()
        {
        }
        void ResetLoad()
        {
            if (_mtx)
            {
                _mtx->lock();
            }
            _load = 0;
            if (_mtx)
            {
                _mtx->unlock();
            } 
        }
        // 对负载进行递增
        void IncLoad()
        {
            if (_mtx)
            {
                _mtx->lock();
            }
            _load++;
            if (_mtx)
            {
                _mtx->unlock();
            }
        }
        // 减少主机负载
        void DecLoad()
        {
            if (_mtx)
            {
                _mtx->lock();
            }
            _load--;
            if (_mtx)
            {
                _mtx->unlock();
            }
        }

        // 获取主机负载
        uint64_t Load()
        {
            uint64_t load = 0;
            if (_mtx)
            {
                _mtx->lock();
            }
            load = _load;
            if (_mtx)
            {
                _mtx->unlock();
            }
            return load;
        }

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
            LOG(INFO) << "加载配置文件" << service_machine << "成功" << "\n";
        }
        ~LoadBalance() {}
        // 加载配置文件
        bool LoadConf(const std::string &machine_conf)
        {
            std::ifstream in(machine_conf);
            if (!in.is_open())
            {
                LOG(FATAL) << "加载: " << machine_conf << "失败" << "\n";
                return false;
            }
            std::string line;
            while (std::getline(in, line))
            {
                std::vector<std::string> tokens; // 切割成两部分, ip 和 端口
                StringUtil::SplitString(line, &tokens, ":");
                if (tokens.size() != 2)
                {
                    LOG(WARING) << "切分" << line << "失败" << "\n";
                    continue;
                }
                Machine m;
                m._ip = tokens[0];
                m._port = std::atoi(tokens[1].c_str());
                m._load = 0;
                m._mtx = new std::mutex();
                _online.push_back(_machines.size()); // 未插入数据, size为0
                _machines.push_back(m);
            }
            return true;
        }
        // 智能选择 - 从在线主机里选择负载最少的主机来提供服务
        // id 和 m均为输出型参数, id表示主机id, m为该主机的更详细信息
        bool SmartChoice(int *id, Machine **m)
        {
            // 1. 使用选择好的主(更新该主机的负载)
            // 2. 后续可能离线该主机
            _mtx.lock();
            // 均衡算法: 轮询 + hash
            int online_num = _online.size(); // 当前在线主机的数量
            if (online_num == 0)             // 全离线了
            {
                _mtx.unlock();
                LOG(FATAL) << "所有主机均已离线" << "\n";
                return false;
            }
            // 通过遍历的方式, 找到负载最小的机器
            *id = _online[0];
            *m = &_machines[_online[0]];
            uint64_t min_load = _machines[_online[0]].Load();
            for (int i = 0; i < online_num; i++)
            {
                if (min_load > _machines[_online[i]].Load())
                {
                    min_load = _machines[_online[i]].Load();
                    *id = _online[i];
                    *m = &_machines[_online[i]];
                }
            }
            _mtx.unlock();
            return true;
        }
        // 离线主机
        void OfflineMachine(int which) 
        {
            _mtx.lock();
            for(auto iter = _online.begin(); iter != _online.end(); iter++)
            {
                if(*iter == which)
                {
                    _machines[which].ResetLoad();
                    _online.erase(iter);
                    _offline.push_back(which);
                    break;
                }
            }

            _mtx.unlock();
        }
        // 上线
        void OnlineMachine()
        {
            // 统一上线 offline -> online;
            _mtx.lock();
            _online.insert(_online.end(), _offline.begin(), _offline.end());
            _offline.erase(_offline.begin(), _offline.end());
            _mtx.unlock();
            LOG(INFO) << "所有主机均已上线" << "\n";
        }
        // for test
        void ShowMechines()
        {
            _mtx.lock();
            std::cout << "当前在线主机列表: ";
            for (auto &id : _online)
            {
                std::cout << id << " ";
            }
            std::cout << std::endl;
            std::cout << "当前离线主机列表: ";
            for (auto &id : _offline)
            {
                std::cout << id << " ";
            }
            std::cout << std::endl;
            _mtx.unlock();
        }

    private:
        std::vector<Machine> _machines; // 可以提供编译服务的所有的主机, 下标表示主机id
        std::vector<int> _online;       // 所有在线的主机的id
        std::vector<int> _offline;      // 所有离线的主机的id
        std::mutex _mtx;                // 保证在选择主机时的数据安全
    };

    class Control
    {
    public:
        Control() {}
        ~Control() {}
        // 恢复主机
        void RecoveryMachine()
        {
            _load_balance.OnlineMachine();
        }
        // 根据题目数据构建网页
        bool AllQuestions(std::string *html)
        {
            bool ret = true;
            std::vector<Question> all;
            if (_model.GetAllQuestions(&all))
            {
                std::sort(all.begin(), all.end(), [](const Question &q1, const Question &q2)
                          { return std::atoi(q1.number.c_str()) < std::atoi(q2.number.c_str()); });
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
        void Judge(const std::string &number /*题号*/, const std::string in_json, std::string *out_json)
        {
            // in_json: id, code, input
            // 0.根据题目编号,直接拿到对应的题目细节
            struct Question q;
            _model.GetOneQuestion(number, &q);
            // 1.对in_json进行反序列化
            Json::Reader reader;
            Json::Value in_value;
            reader.parse(in_json, in_value);
            // 2.重新拼接用户代码和测试代码, 形成新的代码
            Json::Value compile_value;
            compile_value["input"] = in_value["input"].asString();
            std::string code = in_value["code"].asString();
            compile_value["code"] = code +"\n"+ q.tail;
            compile_value["cpu_limit"] = q.cpu_limit;
            compile_value["mem_limit"] = q.mem_limit;
            Json::FastWriter writer;
            std::string compile_string = writer.write(compile_value);
            // 3.选择负载最低的主机
            while (true)
            {
                int id = 0;
                Machine *m = nullptr;
                if (!_load_balance.SmartChoice(&id, &m))
                {
                    break;
                }

                // 4.发起http请求, 得到结果
                Client cli(m->_ip, m->_port);
                m->IncLoad();
                LOG(INFO) << "选择主机成功, id: " << id << " 详情: " << m->_ip << ":" << m->_port << " 负载: " << m->Load() << "\n";
                if (auto res = cli.Post("/compile_and_run", compile_string, "application/json;charset=utf-8"))
                {
                    // 5.将结果赋值给out_json
                    if (res->status == 200)
                    {
                        *out_json = res->body;
                        m->DecLoad();
                        LOG(INFO) << "请求编译运行服务成功" << "\n";
                        break;
                    }
                    m->DecLoad();
                    //  _load_balance.OfflineMachine(id);
                    // _load_balance.ShowMechines();
                }
                else
                {
                    // 请求失败
                    LOG(ERROR) << "当前主机请求失败, id: " << id << " 详情: " << m->_ip << ":" << m->_port << "\n";
                    _load_balance.OfflineMachine(id);
                    _load_balance.ShowMechines();
                }
            }
        }

    private:
        Model _model;              // 提供后台数据
        View _view;                // 提供网页渲染功能
        LoadBalance _load_balance; // 核心: 负载均衡器
    };
}