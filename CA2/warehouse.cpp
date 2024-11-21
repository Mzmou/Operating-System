#include <iostream>
#include <sstream>
#include <vector>
#include "item.h"

#include <dirent.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <cstring>
#include <fstream>
using namespace std;

void printItems(const vector<item> &items)
{
    for (const auto &i : items)
    {
        i.print_attribute();
    }
}

int main(int argc, char *argv[])
{
    string line;
    vector<item> items_in;
    vector<item> items_out;
    cout << "hi\n";
    string filePath = argv[1];
    int pipeFd = std::stoi(argv[2]);
    int pipeFd2 = std::stoi(argv[3]);
    std::ifstream file(filePath); // <-- Use std::ifstream to read the file

    while (getline(file, line))
    { // Read from STDIN

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

    printItems(items_in);
    cout << "hii\n";
    printItems(items_out);
    const char *message = "helloworld";
    write(pipeFd, message, strlen(message)); // Write message to the pipe

    // close(pipeFd);
    return 0;
}
