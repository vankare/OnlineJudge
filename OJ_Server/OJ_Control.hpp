#pragma once
#include "../Comm/httplib.h"
#include "OJ_Model.hpp"
#include "OJ_View.hpp"
#include <cstdint>
#include <cstdio>
#include <jsoncpp/json/reader.h>
#include <jsoncpp/json/value.h>
#include <jsoncpp/json/writer.h>
#include <string>
#include <utility>
#include <vector>
#include <unordered_map>
#include <mutex>
#include <fstream>
#include <jsoncpp/json/json.h>
#include "../Comm/Logger.hpp"
#include "../Comm/Util.hpp"

namespace NS_CONTROL_MODULE
{
    using namespace NS_MODEL_MODULE;
    using namespace NS_LOGGER_MODULE;
    using namespace NS_VIEW_MODULE;
    using namespace NS_UTIL_MODULE;
    using namespace httplib;

    const std::string machine_config_file = "./config/machine_config";

    class Machine
    {
    public:
        Machine(const std::string& ip = "127.0.0.1", uint16_t port = 9999)
            :_ip(ip)
            ,_port(port)
            ,_isActive(false)
            ,_load(0)
            ,_mtx(nullptr)
        {  }

        // 增加负载
        void IncLoad()
        {
            if(_mtx) _mtx->lock();
            _load++;
            if(_mtx) _mtx->unlock();
        }

        // 减少负载
        void DecLoad()
        {
            if(_mtx) _mtx->lock();
            _load--;
            if(_mtx) _mtx->unlock();
        }

        // 获取负载
        uint64_t Load()
        {
            return _load;
        }

    public:
        std::string DebugStr()
        {
            return "[" + _ip + ":" + std::to_string(_port) + "]";
        }

    public:
        std::string _ip;           // ip
        uint16_t _port;            // 端口
        bool _isActive;            // 是否可用
        uint64_t _load;            // 负载
        std::mutex* _mtx;          // 锁
    };

    class LoadBalance
    {
    public:
        LoadBalance()
        {
            std::ifstream in(machine_config_file);
            if(!in.is_open())
            {
                LOG(FATAL) << "加载负载均衡机器配置失败";
                exit(1);
            }

            std::string line;
            while(std::getline(in, line))
            {
                std::vector<std::string> v;
                StringUtil::Split(line, &v, " ");
                if(v.size() != 2 || v[0][0] == '#')
                {
                    continue;
                }
                
                Machine machine(v[0], std::stoi(v[1]));
                std::mutex * mtx = new std::mutex;
                machine._mtx = mtx;    // mutex 不能拷贝
                _onlineMachne.push_back(std::move(machine));
            }

            in.close();

            if(_onlineMachne.empty())
            {
                LOG(FATAL) << "没有编译机器可以使用";
            }
        }

        Machine* SelectMachine()
        {
            if(_onlineMachne.empty())
            {
                Online();
            }
            uint64_t minPos = 0;
            for(uint32_t i = 0; i < _onlineMachne.size(); ++i)
            {
                if(_onlineMachne[i].Load() == 0)
                {
                   return &_onlineMachne[i];
                }
                minPos = _onlineMachne[i].Load() < _onlineMachne[minPos].Load() ? i : minPos;
            }

            return &_onlineMachne[minPos];
        }

        void Offline(const Machine *pm)
        {
            mtx.lock();
            for(auto iter = _onlineMachne.begin(); iter != _onlineMachne.end(); ++iter)
            {
                if(&(*iter) == pm)
                {
                    auto m = *iter;
                    _onlineMachne.erase(iter);
                    _offlineMachne.push_back(std::move(m));
                }
            }
            mtx.unlock();
        }

        void Online()
        {

        }

    private:
        std::vector<Machine> _onlineMachne;
        std::vector<Machine> _offlineMachne;
        std::unordered_map<int, std::pair<int, int>> _cpu_mem_limit;
        std::mutex mtx;
    };

    class Control
    {
    public:
        bool AllQuestions(std::string *html)
        {
            std::vector<struct Question> questions;
            if(_model.GetAllQuestions(&questions))
            {
                _View.AllExpandHtml(&questions, html);
            }
            else
            {
                *html = "获取题目列表失败";
                return false;
            }
            return true;
        }

        bool OneQuestion(int number, std::string* html)
        {
            struct Question q;
            if(_model.GetOneQuestion(number, &q))
            {
                _View.OneExpandHtml(q, html);
            }
            else
            {
                *html = "获取题目" + std::to_string(number) + "失败";
                return false;
            }
            return true;
        }

        /*
         * number : 题号
         * in_josn : 传入josn, 包含: id, code, input
         */
        void Judge(int number, const std::string& in_json, std::string* out_json)
        {
            // 0. 拿到题目细节
            Question q;
            _model.GetOneQuestion(number, &q);

            // 1. 解析 in_json
            Json::Value root;
            Json::Reader reader;
            bool n = reader.parse(in_json, root);
            if(!n)
            {
                LOG(ERROR) << "Judge模块, 解析josn失败";
                return;
            }
            std::string code = root["code"].asString() + q._tail;
            std::string input = root["input"].asString();

            // 2. 将用户代码与测试代码进行拼接
            Json::Value compile_value;
            compile_value["code"] = code;
            compile_value["input"] = root["input"].asString();
            compile_value["cpu_limit"] = q._cpu_limit;
            compile_value["mem_limit"] = q._mem_limit;

            Json::StyledWriter writer;
            std::string json = writer.write(compile_value);

            // 3. 选择负载最低的机器
            Machine* pm = nullptr;
            while(true)
            {
                pm = _loadbalance.SelectMachine();
                if(pm == nullptr)
                {
                    LOG(FATAL) << "没有任何可用机器";
                    break;
                }
                pm->IncLoad();

                Client clnt(pm->_ip, pm->_port);
                auto ret = clnt.Post("/compile_and_run", json, "application/json;charset=utf8");
                if(ret)
                {
                    // 发送请求成功
                    if(ret->status == 200)
                    {
                        // 成功获取返回
                        *out_json = ret->body;
                        pm->DecLoad();
                        break;
                    }
                    else
                    {
                        // 获取返回失败, 继续尝试
                        pm->DebugStr();
                        continue;
                    }
                }
                else
                {
                    // 发送请求失败, 离线主机
                    LOG(ERROR) << "当前主机" << pm->DebugStr() << "出现异常";
                    _loadbalance.Offline(pm);
                }
            }
        }

    private:
        Model _model;
        View _View;
        LoadBalance _loadbalance;
    };
}
