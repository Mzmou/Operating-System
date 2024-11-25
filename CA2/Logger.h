#pragma once
#include <iostream>
#include <string>
using namespace std;

class Logger
{
public:
    enum class LogLevel
    {
        INFO,
        WARNING,
        ERROR
    };

    static void log(const string &msg, LogLevel level = LogLevel::INFO, const string &processName = "")
    {
        string prefix;
        switch (level)
        {
        case LogLevel::INFO:
            prefix = "\033[32m[INFO]\033[0m";
            break;
        case LogLevel::WARNING:
            prefix = "\033[33m[WARNING]\033[0m"; // Changed to yellow for warning
            break;
        case LogLevel::ERROR:
            prefix = "\033[31m[ERROR]\033[0m"; // Changed to red for error
            break;
        }

        if (!processName.empty())
        {
            cout << prefix << " [" << processName << "] " << msg << endl;
        }
        else
        {
            cout << prefix << " " << msg << endl;
        }
    }
};
