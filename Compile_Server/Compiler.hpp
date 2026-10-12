#pragma once
#include <csignal>
#include <string>
#include "../Comm/Util.hpp"
#include "../Comm/Logger.hpp"
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>


namespace NS_COMPILER_MODULE
{
    using namespace NS_UTIL_MODULE;
    using namespace NS_LOGGER_MODULE;
    class Compiler
    {
    public:
        static bool Compile(const std::string& _file)
        {
            pid_t pid = fork();
            if(pid < 0)
            {
                LOG(ERROR) << "创建子进程失败，无法编译";
                return false;
            }
            else if(pid == 0)
            {
                // child
                int _compile_err = open(PathUtil::Compiler_Error(_file).c_str(), O_CREAT | O_WRONLY | O_TRUNC, 0644);
                if(_compile_err < 0)
                {
                    LOG(ERROR) << "没有成功形成compile_error文件";
                    exit(1);
                }
                dup2(_compile_err, 2);


                execlp("g++",  "g++", "-O2", PathUtil::Src(_file).c_str(),
                        "-o", PathUtil::Exe(_file).c_str(), nullptr);

                LOG(ERROR) << "子进程替换失败";
                exit(2);
            }
            // parent
            waitpid(pid, nullptr, 0);

            if(!FileUtil::IsExists(PathUtil::Exe(_file)))
            {
                LOG(ERROR) << "构建可执行文件失败";
                return false;
            }

            LOG(INFO) << "成功编译可执行文件";
            return true;
        }
    };
}
