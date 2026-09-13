#pragma once
#include <csignal>
#include <string>
#include "../Comm/Util.hpp"
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>


namespace NS_COMPILER_MODULE
{
    using namespace NS_UTIL_MODUL;
    class Compiler
    {
    public:
        Compiler(const std::string& code_file)
            :_code_file(code_file)
        {  }
        bool Compile()
        {
            pid_t pid = fork();
            if(pid < 0)
            {
                return false;
            }
            else if(pid == 0)
            {
                // child
                int fd = open(PathUtil::Stderr(_code_file).c_str(), O_CREAT | O_WRONLY | O_TRUNC, 644);
                if(fd < 0)
                {
                    exit(1);
                }
                dup2(fd, 2);


                execlp("g++", "-O2", PathUtil::Src(_code_file).c_str(),
                        "-o", PathUtil::Exe(_code_file).c_str());
            }
            // parent
            waitpid(pid, nullptr, 0);

            if(FileUtil::IsExists(PathUtil::Exe(_code_file)))
            {
                return false;
            }

            return true;
        }
    private:
        std::string _code_file;
    };
}
