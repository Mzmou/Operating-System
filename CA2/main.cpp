#include <iostream>
#include <dirent.h> // For opendir(), readdir(), closedir()
#include <sys/types.h>
#include <sys/wait.h> // For wait()
#include <unistd.h>   // For fork(), exec()
#include <fstream>
#include <cstring>
using namespace std;
const char *msg = "hello receives\n";
int main(int argc, char *argv[])
{
    int total_profit = 0;
    // Check if the folder name is provided
    if (argc != 3)
    {
        std::cerr << "Usage: " << argv[0] << " <folder_name> <warehouse_path>" << std::endl;
        return 1;
    }

    std::string folderName = argv[1];
    std::string warehousePath = argv[2]; // Path to the warehouse executable

    // Open the directory
    DIR *dir = opendir(folderName.c_str());
    if (dir == nullptr)
    {
        std::cerr << "Error: Could not open directory " << folderName << std::endl;
        return 1;
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != nullptr)
    {
        std::string fileName = entry->d_name;

        // Skip directories and non-CSV files
        if (entry->d_type == DT_DIR || fileName.substr(fileName.find_last_of('.') + 1) != "csv")
        {
            continue;
        }

        // Skip "Parts.csv"
        if (fileName == "Parts.csv")
        {
            continue;
        }
        int pipefd[2];
        if (pipe(pipefd) == -1)
        {
            std::cerr << "Error: Pipe creation failed for file " << fileName << std::endl;
            return 1;
        }

        // Create a child process for each file
        pid_t pid = fork();

        if (pid < 0)
        {
            std::cerr << "Error: Fork failed for file " << fileName << std::endl;
            return 1;
        }

        if (pid == 0)
        {
            // Child process

            std::cout << "Child Process (PID: " << getpid() << ") handling file: " << fileName << std::endl;
            // close(pipefd[1]); // Close write-end in the child

            // Redirect pipe read-end to STDIN
            char pipeArg1[10];
            snprintf(pipeArg1, sizeof(pipeArg1), "%d", pipefd[1]);
            char pipeArg2[10];
            snprintf(pipeArg2, sizeof(pipeArg2), "%d", pipefd[0]);
            std::string filePath = folderName + "/" + fileName;
            execl(warehousePath.c_str(), warehousePath.c_str(), filePath.c_str(), pipeArg1, pipeArg2, nullptr);
            // close(pipefd[0]);
            // Construct the full file path

            // Replace the current process with the `warehouse` program

            // s execl(warehousePath.c_str(), warehousePath.c_str(), NULL);
            //  cout << "hiiii\n";
            //  If exec fails, print an error and exit

            std::cerr << "Error: exec failed for file " << fileName << std::endl;
            _exit(1);
        }
        else
        {
            //  close(pipefd[1]); // Close write-end in the parent

            // Read the message from the pipe
            char buffer[1024];
            ssize_t bytesRead = read(pipefd[0], buffer, sizeof(buffer) - 1);
            if (bytesRead > 0)
            {
                buffer[bytesRead] = '\0'; // Null-terminate the string
                total_profit += stoi(buffer);
                std::cout << "Parent received message from " << fileName << ": " << buffer << std::endl;
            }
            //  write(pipefd[1], msg, strlen(msg));
            //  close(pipefd[0]); // Close read-end after reading
            waitpid(pid, nullptr, 0);
        }
    }
    cout << "total profit is " << total_profit << '\n';
    closedir(dir);
    return 0;
}