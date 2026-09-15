#pragma once
#include "Compiler.hpp"
#include "Runner.hpp"
#include "../Comm/Logger.hpp"
#include <csignal>
#include <jsoncpp/json/reader.h>
#include <jsoncpp/json/value.h>
#include <jsoncpp/json/writer.h>
#include <string>
#include <jsoncpp/json/json.h>
#include <unistd.h>

namespace NS_COMPILE_AND_RUN_MODULE
{
    using namespace NS_UTIL_MODUL;
    using namespace NS_COMPILER_MODULE;
    using namespace NS_RUNNER__MODULE;

    class CompileAndRun
    {
    private:
        static void RemoveTempFile(const std::string& file_name)
        {
            if(FileUtil::IsExists(PathUtil::Src(file_name)))
                unlink(PathUtil::Src(file_name).c_str());

            if(FileUtil::IsExists(PathUtil::Exe(file_name)))
                unlink(PathUtil::Exe(file_name).c_str());

            if(FileUtil::IsExists(PathUtil::Compiler_Error(file_name)))
                unlink(PathUtil::Compiler_Error(file_name).c_str());

            if(FileUtil::IsExists(PathUtil::Stdin(file_name)))
                unlink(PathUtil::Stdin(file_name).c_str());

            if(FileUtil::IsExists(PathUtil::Stdout(file_name)))
                unlink(PathUtil::Stdout(file_name).c_str());

            if(FileUtil::IsExists(PathUtil::Stderr(file_name)))
                unlink(PathUtil::Stderr(file_name).c_str());
        }

        static std::string CodeToDesc(int code, const std::string& file_name)
        {
            std::string desc;
            switch(code)
            {
                case 0:
                    desc = "编译运行成功";
                    break;
                case -1:
                    desc = "未知错误";
                    break;
                case -2:
                    desc = "用户提交代码为空";
                    break;
                case -3:
                    FileUtil::ReadFile(PathUtil::Compiler_Error(file_name), &desc);
                    break;
                case -4:
                    FileUtil::ReadFile(PathUtil::Stderr(file_name), &desc);
                    break;
                case SIGABRT:
                    desc = "内存超过范围";
                    break;
                case SIGXCPU:
                    desc = "CPU运行错误";
                    break;
                case SIGFPE:
                    desc = "浮点数异常";
                    break;
                default:
                    desc = "unkown error, code: " + std::to_string(code);
                    break;
            }
            return desc;
        }
    public:
        /*****************************************
         * 输入
         * code：用户提交的代码
         * input：用户给的自定义输入，不做处理
         *
         * 输出
         * status：状态码
         * reason：应答结果
         *
         * 选填
         * stdout：用户代码运行输出
         * stderr：用户代码标准错误
         ****************************************/
        static void Start(const std::string& in_json, std::string* out_json)
        {
            Json::Value in_value;
            Json::Reader reader;
            Json::Value out_value;

            reader.parse(in_json, in_value);

            std::string code  = in_value["code"].asString();
            std::string input = in_value["input"].asString();
            int cpu_limit = in_value["cpu_limit"].asInt();
            int mem_limit = in_value["mem_limit"].asInt();
            std::string file_name = FileUtil::UniqueFileName();
            int run_result = 0;
            int status = 0;

            if(code.empty())
            {
                status = -2;
                LOG(WARNING) << "提交代码为空";
                goto END;
            }

            // 将源码写入文件
            if(!FileUtil::WriteFile(PathUtil::Src(file_name), code))
            {
                status = -1;
                LOG(ERROR) << "写入源码到文件失败";
                goto END;
            }

            if(!Compiler::Compile(file_name))
            {
                status = -3;
                LOG(ERROR) << "编译失败";
                goto END;
            }

            run_result = Runner::Run(file_name, cpu_limit, mem_limit);
            if(run_result < 0)
            {
                status = -1;
                LOG(ERROR) << "运行出现内部错误";
                goto END;
            }
            else if(run_result > 0)
            {
                status = run_result;
                goto END;
            }
            else
            {
                status = 0;
            }
            goto END;

END:
            out_value["status"] = status;
            out_value["reason"] = CodeToDesc(status, file_name);
            if(status == 0)
            {
                std::string _stdout;
                std::string _stderr;
                FileUtil::ReadFile(PathUtil::Stdout(file_name), &_stdout);
                FileUtil::ReadFile(PathUtil::Stderr(file_name), &_stderr);
                out_value["stdout"] = _stdout;
                out_value["stderr"] = _stderr;
            }
            Json::StyledWriter writer;
            *out_json = writer.write(out_value);
            RemoveTempFile(file_name);
        }
    };
}
