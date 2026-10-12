#pragma once

#include <unistd.h>
#include <string>
#include <sys/wait.h>
#include <fcntl.h>
#include "../Comm/Util.hpp"
#include "../Comm/Logger.hpp"

namespace NS_RUNNER__MODULE
{
    using namespace NS_LOGGER_MODULE;
    using namespace NS_UTIL_MODULE;
    class Runner
    {
    public:
        static int Run(const std::string& _file, int _cpu_limit, int _mem_limit)
        {
           /*****************************************
            * 只关系是否正常运行，对错与否交给上层
            * 1. 返回值小于0，内部错误
            * 2. 返回值等于0，正常运行
            * 3. 返回值大于0，信号退出，返回信号
            *
            * 标准输入，不处理，但预留接口
            * 标准输出，定向到临时文件
            * 标注错误，定向到临时文件
            *****************************************/
            std::string exe     = PathUtil::Exe(_file);
            std::string _stdin  = PathUtil::Stdin( _file);
            std::string _stdout = PathUtil::Stdout(_file);
            std::string _stderr = PathUtil::Stderr(_file);

            pid_t pid = fork();
            if(pid < 0)
            {
                LOG(ERROR) << "创建子进程失败，无法运行";
                return -1; // 表示fork失败
            }
            else if(pid == 0)
            {
                // child
                
                umask(0);
                int _in  = open(_stdin.c_str(), O_CREAT | O_WRONLY | O_TRUNC, 0644);
                int _out = open(_stdout.c_str(), O_CREAT | O_WRONLY | O_TRUNC, 0644);
                int _err = open(_stderr.c_str(), O_CREAT | O_WRONLY | O_TRUNC, 0644);
                
                if(_in < 0 || _out < 0 || _err < 0)
                {
                    LOG(ERROR) << "标准输入/标准输出/标准错误，打开失败";
                    exit(-2);
                }

                dup2(_in,  0);
                dup2(_out, 1);
                dup2(_err, 2);

                ResourceLimit::SetProcLimit(_cpu_limit, _mem_limit * 1024 * 1024);

                execl(exe.c_str(), exe.c_str(), nullptr);

                LOG(ERROR) << "程序替换失败，无法执行可执行程序";

                exit(-3);
            }
            else 
            {
                int status = 0;
                waitpid(pid, &status, 0);

                LOG(INFO) << "运行完毕, info:" << (status & 0x7f);
                return status & 0x7f;
            }
        }
    };
}
