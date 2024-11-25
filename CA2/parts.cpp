#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cstring>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <fcntl.h>
#include <vector>
using namespace std;
string global_item = "djjd";
#include <iomanip>
#include "Logger.h"
void printHex(const std::string &str)
{
    for (unsigned char c : str)
    {
        std::cout << std::hex << std::setw(2) << std::setfill('0') << (int)c << " ";
    }
    std::cout << std::endl;
}
vector<string> splitString(const string &input, char delimiter)
{
    vector<string> result;
    stringstream ss(input);
    string token;

    while (getline(ss, token, delimiter))
    {
        if (!token.empty())
        {
            result.push_back(token);
        }
    }

    return result;
}
string get_massage(string part_name, string city_name)
{
    auto pipe_name = city_name + part_name;

    int fd = open(pipe_name.c_str(), O_RDONLY);
    if (fd < 0)
    {
        cerr << "pipe no working\n";
    }

    char buffer[1024];
    ssize_t bytesRead = read(fd, buffer, sizeof(buffer));
    if (bytesRead > 0)
    {
        buffer[bytesRead] = '\0';
    }
    close(fd);
    // cout << global_item << buffer << '\n';
    string msg = buffer;
    return msg;
}
int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        cerr << "Usage: " << argv[0] << " <named_pipe_directory>" << endl;
        return 1;
    }
    string item_name = argv[1];
    int pipeFd = stoi(argv[2]);
    auto cities_concatted = argv[3];

    const char d = '#';
    auto cities = splitString(cities_concatted, d);

    Logger::log("process started", Logger::LogLevel::INFO, item_name);
    global_item = item_name + " ";
    // const char *message = "helloworld";
    cout << "";
    // cout << global_item << "Part created" << '\n';
    for (auto c : cities)
    {
        auto m = get_massage(item_name, c);
        write(pipeFd, m.c_str(), strlen(m.c_str()));
        Logger::log("msg sent", Logger::LogLevel::INFO, item_name);
    }
    // cout << global_item << "exit\n";
    return 0;
}
