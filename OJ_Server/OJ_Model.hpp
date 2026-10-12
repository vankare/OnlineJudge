#pragma once

#include "../Comm/Logger.hpp"
#include "../Comm/Util.hpp"
#include "string"
#include <fstream>
#include <string>
#include <utility>
#include <vector>
#include <unordered_map>
#include <assert.h>

namespace NS_MODEL_MODULE
{
    using namespace NS_UTIL_MODULE;
    using namespace NS_LOGGER_MODULE;
    const std::string gQuestionsPath = "./questions/";
    const std::string gQuestionsList = "./questions/questions.list";

    struct Question
    {
        int _id;                       // 题目编号，唯一
        std::string _title;            // 题目标题
        std::string _desc;             // 题目描述
        std::string _diff;             // 难度
        int _cpu_limit;                // 时间限制
        int _mem_limit;                // 内存限制
        std::string _header;            // 预设代码
        std::string _tail;              // 测试代码
    };
    class Model
    {
    public:
        Model(const std::string listfile = gQuestionsList, const std::string path = gQuestionsPath)
            : _questionsList(listfile)
            , _questionsPath(path)
        {
            bool isLoad = LoadQuestions();
            assert(isLoad);
            LOG(INFO) << "成功加载题目";
        }

        bool LoadQuestions()
        {
            std::ifstream in(_questionsList);
            if(!in.is_open())
            {
                LOG(FATAL) << "加载题库失败";
                return false;
            }

            std::string line;
            for(;;)
            {
                if(!std::getline(in, line))
                {
                    break;
                }
                 
                // 分割字符串line
                Question q;
                std::vector<std::string> tokens;
                StringUtil::Split(line, &tokens, " ");

                if(tokens[0].front() == '#')
                {
                    // 读到注释了
                    continue;
                }
                
                if(tokens.size() != 5)
                {
                    LOG(WARNING) << "加载部分题目失败";
                    continue;
                }
                q._id = std::stoi(tokens[0]);
                q._title = tokens[1];
                q._diff = tokens[2];
                q._cpu_limit = std::stoi(tokens[3]);
                q._mem_limit = std::stoi(tokens[4]);

                std::string path = _questionsPath + std::to_string(q._id) + "/";
                q._desc = FileUtil::ReadFile(path + "desc.txt");
                q._header = FileUtil::ReadFile(path + "header.hpp");
                q._tail = FileUtil::ReadFile(path + "tail.cpp");

                _questions.insert({q._id, q});
            }
            in.close();

            return true;
        }

        bool GetAllQuestions(std::vector<Question>* out)
        {
            if(_questions.empty())
            {
                return false;
            }

            for(auto& [_, q] : _questions)
            {
                out->push_back(q);
            }

            return true;
        }

        bool GetOneQuestion(int id, Question* out)
        {
            auto iter = _questions.find(id);
            if(iter == _questions.end())
            {
                return false;
            }

            *out = iter->second;
            return true;
        }
    private:
        std::unordered_map<int, Question> _questions;
        std::string _questionsList;
        std::string _questionsPath;
    };
}
