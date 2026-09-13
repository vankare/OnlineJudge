#pragma once

#include <string>
#include <sys/stat.h>

const std::string temp = "./temp/";

namespace NS_UTIL_MODUL
{
    class PathUtil
    {
    private:
        static std::string AddSuf(const std::string& file, const std::string& suf)
        {
            std::string path = temp;
            path += file;
            path += suf;
            return path;
        }
    public:
        static std::string Src(const std::string& file)
        {
            return AddSuf(file, ".cpp");
        }
        static std::string Exe(const std::string& file)
        {
            return AddSuf(file, ".exe");
        }
        static std::string Stderr(const std::string& file)
        {
            return AddSuf(file, ".stderr");
        }
    };
    class FileUtil
    {
    public:
        static bool IsExists(const std::string& file)
        {
            struct stat st;
            int n = stat(file.c_str(), &st);
            if(n == -1)
                return false;
            else
                return true;
        }
    };
}
