#pragma once

#include <string>
#include <sys/stat.h>
#include <ctime>

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
        static std::string Compiler_Err(const std::string& file)
        {
            return AddSuf(file, ".compiler_err");
        }
        static std::string Stdin(const std::string& file)
        {
            return AddSuf(file, ".stdin");
        }
        static std::string Stdout(const std::string& file)
        {
            return AddSuf(file, ".stdout");
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
    
    class TimeUtil
    {
    public:
        static std::string Get_DateTime()
        {
            std::time_t now = std::time(nullptr);
            std::tm tm_buf{};
            localtime_r(&now, &tm_buf);

            char buf[64];
            std::snprintf(buf, sizeof(buf),
                    "%04d-%02d-%02d %02d:%02d:%02d",
                    tm_buf.tm_year + 1900,
                    tm_buf.tm_mon + 1,
                    tm_buf.tm_mday,
                    tm_buf.tm_hour,
                    tm_buf.tm_min,
                    tm_buf.tm_sec
                    );
            return std::string(buf);
        }
    };
}
