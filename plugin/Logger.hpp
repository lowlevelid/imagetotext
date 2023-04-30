#ifndef HEADER_ITT_LOGGER_H
#define HEADER_ITT_LOGGER_H

#include <chrono>
#include <iostream>
#include <sstream>
#include <cstring>

#define C_COLOR_RED "\033[31m"
#define C_COLOR_GREEN "\033[32m"
#define C_COLOR_YELLOW "\033[33m"
#define C_COLOR_RESET "\033[0m"

#define INFO_LEVEL "INFO"
#define ERROR_LEVEL "ERROR"

namespace itt 
{

enum LogLevel 
{
    INFO = 1,
    ERROR = 2
};

class Logger
{
public:
    Logger(LogLevel level);
    ~Logger();

    template<typename T>
    Logger& operator<<(const T& t);
private:
    std::stringstream m_stream;
    std::ostream& m_out;
    LogLevel m_level;
};

Logger::Logger(LogLevel level = LogLevel::INFO) :
    m_out(std::cout), m_level(level)
{

    auto now = std::chrono::system_clock::now();
    std::time_t end_time = std::chrono::system_clock::to_time_t(now);

    m_stream << C_COLOR_GREEN;
    m_stream << std::strtok(std::ctime(&end_time), "\n");
    m_stream << " :: ";
}

template<typename T>
Logger& Logger::operator<<(const T& t)
{

    switch (m_level)
    {
    case LogLevel::INFO:
        m_stream << C_COLOR_YELLOW;
        m_stream << "LEVEL :: ";
        m_stream << INFO_LEVEL;
        m_stream << " :: ";
        break;
    
    case LogLevel::ERROR:
        m_stream << C_COLOR_RED;
        m_stream << "LEVEL :: ";
        m_stream << ERROR_LEVEL;
        m_stream << " :: ";
        break;

    default:
        m_stream << C_COLOR_YELLOW;
        m_stream << "LEVEL :: ";
        m_stream << INFO_LEVEL;
        m_stream << " :: ";
        break;
    }

    m_stream << t;
    return *this;
}

Logger::~Logger()
{
    
    m_stream << "\n";
    m_stream << C_COLOR_RESET;
    m_out << m_stream.rdbuf();
    m_out.flush();
}

}

#endif