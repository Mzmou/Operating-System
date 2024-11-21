#include <iostream>
#include <dirent.h>   // For opendir(), readdir(), closedir()
#include <sys/types.h>
#include <sys/wait.h> // For wait()
#include <unistd.h>   // For fork(), exec()

int main(int argc, char* argv[]) {
    // Check if the folder name is provided
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <folder_name> <warehouse_path>" << std::endl;
        return 1;
    }

    std::string folderName = argv[1];
    std::string warehousePath = argv[2]; // Path to the warehouse executable

    // Open the directory
    DIR* dir = opendir(folderName.c_str());
    if (dir == nullptr) {
        std::cerr << "Error: Could not open directory " << folderName << std::endl;
        return 1;
    }

    struct dirent* entry;
    while ((entry = readdir(dir)) != nullptr) {
        std::string fileName = entry->d_name;

        // Skip directories and non-CSV files
        if (entry->d_type == DT_DIR || fileName.substr(fileName.find_last_of('.') + 1) != "csv") {
            continue;
        }

        // Skip "Parts.csv"
        if (fileName == "Parts.csv") {
            continue;
        }

        // Create a child process for each file
        pid_t pid = fork();

        if (pid < 0) {
            std::cerr << "Error: Fork failed for file " << fileName << std::endl;
            return 1;
        }

        if (pid == 0) {
            // Child process
            std::cout << "Child Process (PID: " << getpid() << ") handling file: " << fileName << std::endl;

            // Construct the full file path
            std::string filePath = folderName + "/" + fileName;

            // Replace the current process with the `warehouse` program
            execl(warehousePath.c_str(), warehousePath.c_str(), filePath.c_str(), NULL);

            // If exec fails, print an error and exit
            std::cerr << "Error: exec failed for file " << fileName << std::endl;
            _exit(1);
        } else {
            // Parent process
            int status;
            //waitpid(pid, &status, 0); // Wait for the child to finish
        }
    }

    closedir(dir);
    return 0;
}
