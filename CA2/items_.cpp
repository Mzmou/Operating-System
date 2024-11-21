#include <iostream>
#include <unistd.h> // For getpid()

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        std::cerr << "Usage: " << argv[0] << " <item_name>" << std::endl;
        return 1;
    }

    std::string itemName = argv[1];

    // Simulate processing the item
    std::cout << "Child process " << getpid() << " is processing item: " << itemName << std::endl;

    // Simulate some processing delay
    sleep(2);

    std::cout << "Child process " << getpid() << " finished processing item: " << itemName << std::endl;

    return 0;
}
