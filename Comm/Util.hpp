#pragma once

#include <fstream>
#include <string>
#include <sys/stat.h>
#include <ctime>
#include <sys/resource.h>
#include <sys/time.h>
#include <sys/types.h>
#include <atomic>


namespace NS_UTIL_MODUL
{
    const std::string temp = "./temp/";

    class TimeUtil
    {
    public:
        static std::string GetDateTime()
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
        static std::string GetTimeMs()
        {
            struct timeval tv;
            gettimeofday(&tv, nullptr);
            time_t sec = tv.tv_sec;
            suseconds_t sus = tv.tv_usec;
            long long ms = sec * 1000 + sus / 1000;
            return std::to_string(ms);
        }
    };

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
        static std::string Compiler_Error(const std::string& file)
        {
            return AddSuf(file, ".compiler_error");
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
        static std::string UniqueFileName()
        {
            static std::atomic_int id(0);
            id++;
            std::string time = TimeUtil::GetTimeMs();
            std::string uniuq_id = std::to_string(id);
            return time + "_" + uniuq_id;
        }

        static bool WriteFile(const std::string& filename, const std::string& content)
        {
            std::ofstream out(filename, std::ios::out);
            if(!out.is_open())
            {
                return false;
            }
            out << content;

            out.close();
            return true;
        }
        static bool ReadFile(const std::string& filename, std::string * content)
        {
            std::ifstream in(filename, std::ios::in);
            if(!in.is_open())
            {
                return false;
            }
            std::string line;
            while(std::getline(in, line))
            {
                *content += line;
                *content += "\n";
            }
            
            in.close();
            return true;
        }
    };
    
    class ResourceLimit
    {
    public:
        static void SetProcLimit(int _cpu_limit, int _mem_limit)
        {
            struct rlimit rcpu;
            rcpu.rlim_cur = _cpu_limit;
            rcpu.rlim_max = RLIM_INFINITY;
            setrlimit(RLIMIT_CPU, &rcpu);

            struct rlimit rmem;
            rcpu.rlim_cur = _mem_limit;
            rcpu.rlim_max = RLIM_INFINITY;
            setrlimit(RLIMIT_AS, &rmem);
        }
    };
}
