#include <iostream>
#include <dirent.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fstream>
#include <cstring>
#include <sys/stat.h>
#include <sstream>
#include <vector>
#include <bits/stdc++.h>
#include "item.h"
#include "Logger.h"

using namespace std;
const char *msg = "hello receives\n";
string part_name_path = "Parts.csv";
const char *process_name = "main";
const string pipes[] = {"shekar_pipe", "roghan_pipe", "berenj_pipe", "makaroni_pipe"};
#include <iomanip>
vector<item> final_merchendise;
map<string, float> total_profit_items;
void initialize_map(vector<string> item_names)
{

    std::map<std::string, int> inventory;

    for (const auto &key : item_names)
    {
        total_profit_items[key] = 0;
    }
}
string code_names(vector<string> filename)
{
    string coded_string;
    for (auto f : filename)
    {
        coded_string = coded_string + f + "#";
    }
    return coded_string;
}
vector<string> create_ui(vector<string> items)
{

    cout << "choose number of the item you want: \n";
    for (long unsigned int i = 0; i < items.size(); i++)
    {
        cout << (i + 1) << ". " << items[i] << '\n';
    }
    std::string inputLine;
    std::getline(std::cin, inputLine);

    std::istringstream iss(inputLine);
    std::vector<std::string> words;
    std::string word;
    while (iss >> word)
    {
        words.push_back(word);
    }
    return words;
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

pair<int, int> extract_price_remained(string message)
{
    const char d = '#';
    auto prices = splitString((message), d);

    std::pair<int, int> p = std::make_pair(stoi(prices[0]), stoi(prices[1]));
    return p;
}
void printHex(const std::string &str)
{
    for (unsigned char c : str)
    {
        std::cout << std::hex << std::setw(2) << std::setfill('0') << (int)c << " ";
    }
    std::cout << std::endl;
}
vector<string> extract_items(string line)
{

    stringstream ss(line);
    vector<string> partNames;

    string partName;
    while (getline(ss, partName, ','))
    {

        partNames.push_back(partName);
    }
    return partNames;
}
void create_named_pipe(vector<string> items_name, vector<string> cities)
{
    for (long unsigned int j = 0; j < cities.size(); j++)

        for (long unsigned int i = 0; i < items_name.size(); i++)
        {
            string pipe_name = cities[j] + items_name[i];

            mkfifo(pipe_name.c_str(), 0666);
        }
}

vector<string> extract_name_files(string folderName)
{
    vector<string> file_names;
    DIR *dir = opendir(folderName.c_str());
    if (dir == nullptr)
    {
        std::cerr << "Error: Could not open directory " << folderName << std::endl;
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != nullptr)
    {
        std::string fileName = entry->d_name;

        if (entry->d_type == DT_DIR || fileName.substr(fileName.find_last_of('.') + 1) != "csv")
        {
            continue;
        }
        file_names.push_back(fileName);
    }

    closedir(dir);
    return file_names;
}
vector<string> get_parts(string folderName, string fileName)
{
    vector<string> partNames;
    std::string filePath = folderName + "/" + fileName;
    ifstream file(filePath);
    if (!file.is_open())
    {
        cerr << "Error: Could not open Parts.csv" << endl;
    }

    string line;
    getline(file, line);

    partNames = extract_items(line);
    return partNames;
}
int create_part(string item_name, string part_path, string cities_name_concatted, int num_of_city)
{

    int pipefd[2];
    if (pipe(pipefd) == -1)
    {
        std::cerr << "Error: Pipe creation failed for file " << item_name << std::endl;
        return 1;
    }

    pid_t pid = fork();

    if (pid < 0)
    {
        std::cerr << "Error: Fork failed for file " << item_name << std::endl;
        return 1;
    }

    if (pid == 0)
    {

        char pipeArg1[10];
        snprintf(pipeArg1, sizeof(pipeArg1), "%d", pipefd[1]);

        execl(part_path.c_str(), part_path.c_str(), item_name.c_str(), pipeArg1, cities_name_concatted.c_str(), nullptr);

        std::cerr << "Error: exec failed for file " << item_name << std::endl;
        _exit(1);
    }
    else
    {
        int total_price = 0;
        int total_leftover = 0;
        for (int i = 0; i < num_of_city; i++)
        {

            char buffer[1024];
            ssize_t bytesRead = read(pipefd[0], buffer, sizeof(buffer) - 1);
            if (bytesRead > 0)
            {
                buffer[bytesRead] = '\0';
            }
            auto p = extract_price_remained(string(buffer));
            total_price += p.first;
            total_leftover += p.second;
        }
        final_merchendise.push_back(item(item_name, total_price, total_leftover));

        close(pipefd[0]);
        return 0;
    }
}
int create_warehouse(string fileName, string folderName, string warehousePath, string item_concatted, vector<string> item_names)
{

    int pipefd[2];
    if (pipe(pipefd) == -1)
    {
        std::cerr << "Error: Pipe creation failed for file " << fileName << std::endl;
        return 1;
    }

    pid_t pid = fork();

    if (pid < 0)
    {
        std::cerr << "Error: Fork failed for file " << fileName << std::endl;
        return 1;
    }

    if (pid == 0)
    {

        char pipeArg1[10];
        snprintf(pipeArg1, sizeof(pipeArg1), "%d", pipefd[1]);
        char pipeArg2[10];
        snprintf(pipeArg2, sizeof(pipeArg2), "%d", pipefd[0]);
        std::string filePath = folderName + "/" + fileName;
        execl(warehousePath.c_str(), warehousePath.c_str(), fileName.c_str(), pipeArg1, pipeArg2, item_concatted.c_str(), folderName.c_str(), nullptr);

        std::cerr << "Error: exec failed for file " << fileName << std::endl;
        _exit(1);
    }
    else
    {
        int total_profit = 0;

        char buffer[2048];
        ssize_t bytesRead = read(pipefd[0], buffer, sizeof(buffer) - 1);

        if (bytesRead > 0)
        {
            Logger::log("got message", Logger::LogLevel::INFO, process_name);
            buffer[bytesRead] = '\0';

            cout << "";

            char d = '#';
            auto res = splitString(string(buffer), d);

            for (long unsigned int k = 0; k < res.size(); k++)
            {
                total_profit_items[item_names[k]] += stof(res[k]);
            }
        }
        return total_profit;
    }
}

void createPipes()
{

    for (const string &pipeName : pipes)
    {
        if (mkfifo(pipeName.c_str(), 0666) == -1)
        {
            cerr << "Error creating pipe: " << pipeName << endl;
        }
    }
}

void cleanupPipes(vector<string> pipes, vector<string> cities)
{
    for (long unsigned int j = 0; j < cities.size(); j++)
        for (const string &pipeName : pipes)
        {
            string pipe_name = cities[j] + pipeName;
            unlink(pipe_name.c_str());
        }
}
int calculate_total_profit(vector<string> fileNames, string folderName, string warehousePath, string items_concatted, vector<string> item_names)
{
    int profit = 0;
    for (auto f : fileNames)
    {
        if (f != "Parts.csv")
        {

            profit += create_warehouse(f, folderName, warehousePath, items_concatted, item_names);
        }
    }
    return profit;
}
void handle_leftover_merchendise(vector<string> part_names, string part_path, string cities_name_concatted, int num_cities)
{
    for (auto c : part_names)
    {

        Logger::log("part created", Logger::LogLevel::INFO, process_name);
        create_part(c, part_path, cities_name_concatted, num_cities);
    }
}
int main(int argc, char *argv[])
{
    vector<string> partNames;
    float total_profit = 0;

    if (argc != 4)
    {
        std::cerr << "Usage: " << argv[0] << " <folder_name> <warehouse_path> <part_path>" << std::endl;
        return 1;
    }

    std::string folderName = argv[1];
    std::string warehousePath = argv[2];
    string part_path = argv[3];
    auto fileNames = extract_name_files(folderName);

    auto part_names = get_parts(folderName, part_name_path);
    Logger::log("parts found", Logger::LogLevel::INFO, process_name);
    vector<string> cities_name;
    string cities_name_concatted = "";

    for (auto f : fileNames)
    {
        if (f != "Parts.csv")
        {
            cities_name.push_back(f);
            cities_name_concatted = cities_name_concatted + f + "#";
        }
    }
    auto items_concatted = code_names(part_names);

    create_named_pipe(part_names, cities_name);
    Logger::log("pipe created", Logger::LogLevel::INFO, process_name);

    auto sortedWords = part_names;
    std::sort(sortedWords.begin(), sortedWords.end());

    total_profit = calculate_total_profit(fileNames, folderName, warehousePath, items_concatted, sortedWords);

    handle_leftover_merchendise(part_names, part_path, cities_name_concatted, cities_name.size());
    cleanupPipes(part_names, cities_name);
    auto choice = create_ui(part_names);

    for (auto c : choice)
    {
        final_merchendise[stoi(c) - 1].print_attribute_end();
        total_profit += total_profit_items[part_names[stoi(c) - 1]];
    }

    cout
        << "total profit is " << round(total_profit) << endl;
    Logger::log("process end dear ta", Logger::LogLevel::INFO, process_name);

    wait(NULL);
}