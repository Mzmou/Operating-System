#include <iostream>
#include <sstream>
#include <vector>
#include "item.h"
#include <sys/stat.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <cstring>
#include <fstream>
#include <set>
#include <fcntl.h>
#include <map>
string global_process_name;
using namespace std;
#include <iomanip>

map<string, int> make_inventory(vector<string> item_names, vector<item> items_in)
{

    std::map<std::string, int> inventory;

    for (const auto &key : item_names)
    {
        inventory[key] = 0;
    }

    for (const auto &[key, value] : inventory)
    {
        for (auto i : items_in)
        {
            if (i.check_name(key))
            {
                inventory[key] += i.get_price();
            }
        }
        // std::cout << key << ": " << value << std::endl;
    }
    return inventory;
}
map<string, int> make_inventory_remained(vector<string> item_names, vector<item> items_in)
{

    std::map<std::string, int> inventory;

    for (const auto &key : item_names)
    {
        inventory[key] = 0;
    }

    for (const auto &[key, value] : inventory)
    {
        for (auto i : items_in)
        {
            if (i.check_name(key))
            {
                inventory[key] += i.get_number();
            }
        }
        // std::cout << key << ": " << value << std::endl;
    }
    return inventory;
}
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
void send_massage(string part_name, string city_name, string massage)
{
    auto pipe_name = city_name + part_name;

    cout << "";
    int fd = open(pipe_name.c_str(), O_WRONLY);
    if (fd < 0)
    {
        cerr << "pipe no working\n";
    }

    write(fd, massage.c_str(), strlen(massage.c_str()));

    close(fd);
}
void printItems(const vector<item> &items)
{
    for (const auto &i : items)
    {
        i.print_attribute();
    }
}

void createNamedPipeAndWrite(const string &pipeName, const string &message)
{
    const char *directory = "nameddpipe";

    struct stat st;
    if (stat(directory, &st) == -1)
    {
        mkdir(directory, 0777);
    }

    string fullPipePath = string(directory) + "/" + pipeName;

    if (access(fullPipePath.c_str(), F_OK) == -1)
    {

        if (mkfifo(fullPipePath.c_str(), 0666) == -1)
        {
            perror("Failed to create named pipe");
            return;
        }
        else
        {
            cout << "PIPE created\n";
        }
    }
    else
    {
        cout << "Named pipe already exists: " << fullPipePath << endl;
        return;
    }

    int fd = open(fullPipePath.c_str(), O_WRONLY);
    if (fd != -1)
    {
        write(fd, message.c_str(), message.size());
        close(fd);
    }
    else
    {
        perror("Failed to open named pipe");
    }
}
int calculate_profit(vector<item> &items_in, vector<item> items_out)
{
    int profit = 0;
    // printItems(items_in);
    // printItems(items_out);
    for (long unsigned int i = 0; i < items_out.size(); i++)
    {
        for (long unsigned int j = 0; j < items_in.size(); j++)
        {
            if (items_out[i].check_name(items_in[j].get_name()))
            {
                // cout << items_in[j].get_name() << "  " << items_out[i].get_number() << '\n';
                if (items_in[j].get_number() >= items_out[i].get_number())
                {

                    profit += (items_out[i].get_value() - items_in[j].get_value()) * (items_out[i].get_number());
                    items_in[j].decrease_number(items_out[i].get_number());
                    items_out[i].decrease_number(items_out[i].get_number());
                    break;
                }
                else
                {
                    profit += (items_out[i].get_value() - items_in[j].get_value()) * (items_in[j].get_number());
                    items_out[i].decrease_number(items_in[j].get_number());
                    items_in[j].decrease_number(items_in[j].get_number());
                }
            }
        }
    }
    // cout << "---------\n";
    // printItems(items_in);
    //  printItems(items_out);
    return profit;
}

int main(int argc, char *argv[])
{
    string line;
    vector<item> items_in;
    vector<item> items_out;

    string filename = argv[1];
    string foldername = argv[5];
    string filePath = foldername + '/' + filename;
    global_process_name = filePath + " ";
    int pipeFd = std::stoi(argv[2]);
    int pipeFd2 = std::stoi(argv[3]);
    std::ifstream file(filePath);

    auto item_concatted = argv[4];
    const char d = '#';
    auto items_ = splitString(item_concatted, d);

    while (getline(file, line))
    {

        stringstream ss(line);
        string name, valueStr, numberStr, inputStr;

        getline(ss, name, ',');
        getline(ss, valueStr, ',');
        getline(ss, numberStr, ',');
        getline(ss, inputStr, ',');

        int value = stoi(valueStr);
        int number = stoi(numberStr);
        bool input = (inputStr.find("input") != string::npos);

        if (input)
        {
            items_in.push_back(item(name, value, number, input));
        }
        else
        {
            items_out.push_back(item(name, value, number, input));
        }
    }

    auto p = calculate_profit(items_in, items_out);
    std::string str = std::to_string(p);
    const char *char_ptr = str.c_str();

    auto price = make_inventory(items_, items_in);
    auto remained = make_inventory_remained(items_, items_in);
    const char *message = "helloworld";
    write(pipeFd, char_ptr, strlen(char_ptr));

    string city_name;
    for (const auto &name : items_)
    {
        // cout << name << endl;

        send_massage(name, filename, to_string(price[name]) + "#" + to_string(remained[name]) + "#");

        // cout << global_process_name << "I blocked\n";
    }

    return 0;
}