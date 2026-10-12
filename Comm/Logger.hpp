#pragma once
#include <fstream>
#include <sstream>
#include <string>
#include <mutex>
#include <filesystem>
#include <iostream>
#include <memory>
#include "Util.hpp"

namespace NS_LOGGER_MODULE
{
    namespace fs = std::filesystem;
    enum LogLevel
    {
        INFO,
        DEBUG,
        WARNING,
        ERROR,
        FATAL
    };
    class LogStrategy
    {
    public:
        virtual void SyncLog(const std::string& message) = 0;
    protected:
        std::mutex _mtx;
    };

    class FileStrategy : public LogStrategy
    {
    public:
        FileStrategy(const std::string& path = "./log", const std::string& file = "log.txt")
            :_path(path)
            ,_file(file)
        {
            if(!path.empty() && path.back() != '/')
            {
                _path += "/";
            }
            try
            {
                fs::create_directory(_path);
            }
            catch(const fs::filesystem_error& e)
            {
                std::cerr << e.what() << "\n";
            }
        }

        void SyncLog(const std::string& message) override
        {
            std::lock_guard<std::mutex> guard(_mtx);
            std::ofstream out(_path + _file, std::ios::app);
            if(!out.is_open())
            {
                std::cerr << "ofstream:" << _path << _file << "\n";
            }

            out << message << "\n";

            out.close();
        }
    private:
        std::string _path;
        std::string _file;
    };
    class ConsoleStrategy : public LogStrategy
    {
    public:
        void SyncLog(const std::string& message) override
        {
            std::lock_guard<std::mutex> guard(_mtx);
            std::cerr << message << "\n";
        }
    };

    class Logger
    {
    private:
        class LogMessage
        {
        public:
            LogMessage(const std::string& level, const std::string& file, const int line, Logger& logger)
                :_level(level)
                ,_file(file)
                ,_line(std::to_string(line))
                ,_logger(logger)
            {
                _time = NS_UTIL_MODULE::TimeUtil::GetDateTime();
                std::stringstream ss;
                ss << "[" + _level + "]"
                   << "[" + _file  + "]"
                   << "[" + _line  + "]"
                   << "[" + _time  + "]"
                   << " ";
                _loginfo = ss.str();
            }

            template<class T>
            LogMessage& operator<<(const T& message)
            {
                std::stringstream ss;
                ss << message;
                _loginfo += ss.str();

                return *this;
            }

            ~LogMessage()
            {
                _logger._strategy->SyncLog(_loginfo);
            }
        private:
            std::string _level;
            std::string _file;
            std::string _line;
            std::string _time;
            Logger& _logger;
            std::string _loginfo;
        };
    public:
        Logger()
        {
            UseCosoleStrategy();
        }

        LogMessage operator()(std::string level, const std::string& file, const int line)
        {
            return LogMessage(level, file, line, *this);
        }

        void UseFileStrategy()
        {
            _strategy = std::make_unique<FileStrategy>();
        }

        void UseCosoleStrategy()
        {
            _strategy = std::make_unique<ConsoleStrategy>();
        }
    private:
        std::unique_ptr<LogStrategy> _strategy;
    };


    static NS_LOGGER_MODULE::Logger logger;
#define ENABLE_FILE_STRATEGY logger.UseFileStrategy()
#define ENABLE_CONSOLE_STRATEGY logger.UseCosoleStrategy()
#define LOG(level) logger(#level, __FILE__, __LINE__)
}

