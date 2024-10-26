#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstdlib>
#include <cstring>
#include <vector>
#include <poll.h>
#include <unistd.h>
#include <iostream>
#include <string.h>
using namespace std;
#include "client_attr.h"
typedef struct pollfd pollfd;
#include "room.cpp"

#define STDIN 0
#define STDOUT 1
#define BUFFER_SIZE 1024
#pragma once
const char *SERVER_LAUNCHED = "Server Launched!\n";
const char *ROOM_LAUNCHED = "Room Launched!\n";
// const char *NEW_CONNECTION = "New Connection!\n";
const char *WELCOMED = "Hello! Welcome to our game. what is your name?\n";
const char *CHOOSE_ROOM = "Please choose a room to enter.\n";
const char *CORRECT_ROOM = "You successfully joined the room!\n";
const char *INVALID_ROOM = "You chose an invalid room. please choose a correct room again.\n";
const char *FULL_ROOM = "You chose a full room. please choose another room again.\n";
const char *CONNECTION = "You connected successfully\n";
const char *WRITE_NAME = "Write your name\n";
const char *NOT_AVAILABLE = "room is not availble\n";
const char *CHOOSE_RIGHT = "You can enter the room\n";
string make_available_rooms_list(vector<Room> rooms)
{
    string result;

    for (size_t i = 0; i < rooms.size(); ++i)
    {
        if (rooms[i].check_available())
        {
            if (i)
                result += " "; //  space for separation

            result += to_string(i); // Concatenate index to result
        }
    }
    result += "\n";

    return result; // Return the concatenated string
}
char *convert_string_to_char(string input)
{
    char *charIndices = new char[input.length() + 1];
    std::strcpy(charIndices, input.c_str());
    return charIndices;
}
char *intToCharPointer(int number)
{
    // Convert int to string
    std::string strNumber = std::to_string(number);

    // Allocate memory for char* (+1 for null terminator)
    char *charNumber = new char[strNumber.length() + 1];

    // Copy the string data to char*
    strcpy(charNumber, strNumber.c_str());

    return charNumber; // Caller is responsible for deleting this memory
}

int main(int argc, char *argv[])
{
    if (argc != 4)
        perror("Invalid Arguments");

    char *ipaddr = argv[1];
    struct sockaddr_in server_addr;
    int server_fd, room_fd, opt = 1, total_rooms = stoi(argv[3]);
    vector<pollfd> pfds;
    vector<Client_attr> players;
    vector<int> room_fds;
    vector<Room> rooms_data;

    server_addr.sin_family = AF_INET;
    if (inet_pton(AF_INET, ipaddr, &(server_addr.sin_addr)) == -1)
        perror("FAILED: Input ipv4 address invalid");

    if ((server_fd = socket(PF_INET, SOCK_STREAM, 0)) == -1)
        perror("FAILED: Socket was not created");

    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1)
        perror("FAILED: Making socket reusable failed");

    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt)) == -1)
        perror("FAILED: Making socket reusable port failed");

    memset(server_addr.sin_zero, 0, sizeof(server_addr.sin_zero));

    server_addr.sin_port = htons(strtol(argv[2], NULL, 10));

    if (bind(server_fd, (const struct sockaddr *)(&server_addr), sizeof(server_addr)) == -1)
        perror("FAILED: Bind unsuccessfull");

    if (listen(server_fd, 20) == -1)
        perror("FAILED: Listen unsuccessfull");

    write(STDOUT, SERVER_LAUNCHED, strlen(SERVER_LAUNCHED));

    pfds.push_back(pollfd{server_fd, POLLIN, 0});
    auto first_port = 10000;
    for (int i = 0; i < total_rooms; i++)
    {
        std::vector<pollfd> pfds_room;
        struct sockaddr_in server_addr_room;
        server_addr_room.sin_family = AF_INET;
        // int server_room_fd;
        int room_opt = 1;
        if (inet_pton(AF_INET, ipaddr, &(server_addr_room.sin_addr)) == -1)
            perror("FAILED: Input ipv4 address invalid");
        if ((room_fd = socket(PF_INET, SOCK_STREAM, 0)) == -1)
            perror("FAILED: Socket was not created");

        if (setsockopt(room_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1)
            perror("FAILED: Making socket reusable failed");

        if (setsockopt(room_fd, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt)) == -1)
            perror("FAILED: Making socket reusable port failed");

        memset(server_addr_room.sin_zero, 0, sizeof(server_addr_room.sin_zero));

        server_addr_room.sin_port = htons(first_port + i);

        if (bind(room_fd, (const struct sockaddr *)(&server_addr_room), sizeof(server_addr_room)) == -1)
            perror("FAILED: Bind unsuccessfull");

        if (listen(room_fd, 20) == -1)
            perror("FAILED: Listen unsuccessfull");

        // server_addr_room.sin_port = htons(first_port + i);
        write(STDOUT, ROOM_LAUNCHED, strlen(ROOM_LAUNCHED));

        room_fds.push_back(room_fd);
        pfds.push_back(pollfd{room_fd, POLLIN, 0});
        pfds_room.push_back(pollfd{room_fd, POLLIN, 0});

        Room temp_room(pfds_room, first_port + i, room_fd);
        rooms_data.push_back(temp_room);
    }

    while (1)
    {
        if (poll(pfds.data(), (nfds_t)(pfds.size()), -1) == -1)
            perror("FAILED: Poll");

        for (size_t i = 0; i < pfds.size(); ++i)
        {
            if (pfds[i].revents & POLLIN)
            {
                if (pfds[i].fd == server_fd) // new user
                {
                    struct sockaddr_in new_addr;
                    Client_attr new_user;
                    socklen_t new_size = sizeof(new_addr);
                    int new_fd = accept(server_fd, (struct sockaddr *)(&new_addr), &new_size);

                    write(STDOUT, NEW_CONNECTION, strlen(NEW_CONNECTION));

                    new_user.set_fd(new_fd);

                    players.push_back(new_user);
                    pfds.push_back(pollfd{new_fd, POLLIN, 0});

                    send(new_fd, WRITE_NAME, strlen(WRITE_NAME), 0);
                }
                else // message from user
                {
                    char buffer[BUFFER_SIZE];
                    int fd = pfds[i].fd;

                    memset(buffer, 0, BUFFER_SIZE);
                    recv(fd, buffer, BUFFER_SIZE, 0);
                    auto length = 0;
                    while (length < players.size())
                    {
                        if (players[length].check_fd(fd))
                        {
                            if (!players[length].check_set_name())
                            {
                                players[length].set_name(buffer);
                                auto available_room = convert_string_to_char(make_available_rooms_list(rooms_data));

                                send(fd, available_room, strlen(available_room), 0);
                            }
                            else
                            {
                                int choice_number = stoi(buffer);

                                if (rooms_data[choice_number].check_available())
                                {
                                    send(fd, CORRECT_ROOM, strlen(CORRECT_ROOM), 0);

                                    rooms_data[choice_number].add_player(players[length]);

                                    memset(buffer, 0, BUFFER_SIZE);
                                    recv(fd, buffer, BUFFER_SIZE, 0);
                                    write(1, buffer, 1024);
                                    auto port_number = rooms_data[choice_number].get_port();
                                    auto n = send(fd, intToCharPointer(port_number), 1024, 0);
                                    // auto room_pfd = rooms_data[choice_number].get_pfds();
                                    cout << n << endl;
                                    memset(buffer, 0, BUFFER_SIZE);
                                    auto bytes = recv(fd, buffer, BUFFER_SIZE, 0); // succesufuly connected
                                    cout << bytes << '\n';
                                    write(1, buffer, 1024);
                                    if (!strcmp(buffer, CONNECTION))
                                    {
                                        cout << "start to play\n";
                                        rooms_data[choice_number].play();
                                    }
                                    // if(!rooms_data[choice_number - 1].check_available()){{}

                                    // }
                                    // start game
                                }
                                else
                                {
                                    send(fd, FULL_ROOM, strlen(FULL_ROOM), 0);
                                }
                            }
                            // break;
                        }
                        length++;
                    }
                }
            }
        }
    }
}