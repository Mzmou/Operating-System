#pragma once
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <cstdlib>
#include <cstring>
#include <vector>
#include <poll.h>
#include <unistd.h>
#include "client_attr.h"
#include "room.cpp"
#include "string"
typedef struct pollfd pollfd;

#define STDOUT 1
#define BUFFER_SIZE 1024

const char *SERVER_LAUNCHED = "Server Launched!\n";
const char *NEW_CONNECTION = "New Connection!\n";
const char *WELLCOME = "Write _start to enter the game\n";
const char *WRITE_NAME = "Write your name\n";
const char *HELLO = "hello ";
const char *start = "_start\n";
const char *CHOOSE_RIGHT = "You can enter the room\n";
const char *NOT_AVAILABLE = "room is not availble\n";
string make_available_rooms_list(vector<int> bitmap)
{
    string result;

    for (size_t i = 0; i < bitmap.size(); ++i)
    {
        if (bitmap[i] == 0)
        {
            if (!result.empty())
            {
                result += ", "; // Add a comma and space for separation
            }
            result += to_string(i); // Concatenate index to result
        }
    }

    return result; // Return the concatenated string
}
int make_int(char *choice, int length)
{
    char *temp = new char[length];
    for (int i = 0; i < length; i++)
    {
        temp[i] = choice[i];
    }

    return stoi(temp);
}
bool check_available(int room, vector<int> room_bit_map)
{
    if (room < room_bit_map.size() and room > -1)
    {
        return !room_bit_map[room];
    }
    return 0;
}
char *convert_int_to_char_pointer(int port)
{
    string strNumber = std::to_string(port);

    // Allocate char* and copy the string
    char *charNumber = new char[strNumber.length() + 1]; // +1 for null terminator
    strcpy(charNumber, strNumber.c_str());

    return charNumber;
}
#include <iostream>
using namespace std;
int main(int argc, char *argv[])
{
    int number_of_rooms;

    vector<Client_attr> clients_data;
    if (argc != 4)
        perror("Invalid Arguments");

    char *ipaddr = argv[1];
    struct sockaddr_in server_addr;
    int server_fd, opt = 1;
    std::vector<pollfd> pfds;
    number_of_rooms = atoi(argv[3]);
    cout << "number of room is" << number_of_rooms << '\n';
    vector<Room> rooms_data;
    vector<int> room_bit_map(number_of_rooms, 0);
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

    write(1, SERVER_LAUNCHED, strlen(SERVER_LAUNCHED));
    pfds.push_back(pollfd{server_fd, POLLIN, 0});
    /// initialize rooms
    int first_port = 10000;
    for (int i = 0; i < number_of_rooms; i++)
    {
        struct sockaddr_in server_addr_room;
        int server_room_fd;
        int room_opt = 1;

        server_addr_room.sin_family = AF_INET;
        server_addr_room.sin_port = htons(first_port + i); // Use the same port for all rooms

        if (inet_pton(AF_INET, ipaddr, &(server_addr_room.sin_addr)) <= 0)
        {
            perror("FAILED: Input IPv4 address invalid");
            continue;
        }

        if ((server_room_fd = socket(PF_INET, SOCK_STREAM, 0)) == -1)
        {
            perror("FAILED: Socket was not created");
            continue;
        }

        if (setsockopt(server_room_fd, SOL_SOCKET, SO_REUSEADDR, &room_opt, sizeof(room_opt)) == -1)
        {
            perror("FAILED: Making socket reusable failed");
            close(server_fd);
            continue;
        }

        if (setsockopt(server_room_fd, SOL_SOCKET, SO_REUSEPORT, &room_opt, sizeof(room_opt)) == -1)
        {
            perror("FAILED: Making socket reusable port failed");
            close(server_fd);
            continue;
        }

        memset(server_addr_room.sin_zero, 0, sizeof(server_addr_room.sin_zero));

        if (bind(server_room_fd, (const struct sockaddr *)(&server_addr_room), sizeof(server_addr_room)) == -1)
        {
            perror("FAILED: Bind unsuccessful");
            close(server_room_fd);
            continue;
        }

        if (listen(server_room_fd, 20) == -1)
        {
            perror("FAILED: Listen unsuccessful");
            close(server_room_fd);
            continue;
        }

        cout << server_room_fd << '\n';
        std::vector<pollfd> pfds_room;
        pfds_room.push_back(pollfd{server_room_fd, POLLIN, 0});
        Room temp_room(pfds_room, first_port + i, server_room_fd);
        rooms_data.push_back(temp_room);
        // Store the server file descriptor for future use
        // server_fds.push_back(server_room_fd);
    }

    cout << make_available_rooms_list(room_bit_map) << '\n';

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
                    socklen_t new_size = sizeof(new_addr);
                    int new_fd = accept(server_fd, (struct sockaddr *)(&new_addr), &new_size);
                    write(1, NEW_CONNECTION, strlen(NEW_CONNECTION));
                    pfds.push_back(pollfd{new_fd, POLLIN, 0});
                    int sents = send(new_fd, WELLCOME, strlen(WELLCOME), 0);

                    // cout << sents << '\n';
                    //  cout<<WELLCOME<<'\n';
                }
                else // message from user
                {

                    char buffer[BUFFER_SIZE];
                    memset(buffer, 0, BUFFER_SIZE);
                    recv(pfds[i].fd, buffer, BUFFER_SIZE, 0);
                    if (!strcmp(buffer, start))
                    {

                        char name_char[BUFFER_SIZE];
                        int sents = send(pfds[i].fd, WRITE_NAME, strlen(WRITE_NAME), 0);
                        recv(pfds[i].fd, name_char, BUFFER_SIZE, 0);
                        auto char_form = (char *)(HELLO);
                        auto name = string(name_char);
                        Client_attr temp(name);
                        clients_data.push_back(temp);
                        while (1)
                        {
                            auto avail_rooms = make_available_rooms_list(room_bit_map);
                            char *charIndices = new char[avail_rooms.length() + 1]; // +1 for null terminator
                            std::strcpy(charIndices, avail_rooms.c_str());
                            char choice[BUFFER_SIZE];
                            send(pfds[i].fd, charIndices, avail_rooms.length() + 1, 0);
                            auto len_r = recv(pfds[i].fd, choice, BUFFER_SIZE, 0);
                            // cout<<len_r<<endl;
                            write(1, choice, len_r);
                            if (check_available(make_int(choice, len_r), room_bit_map))
                            {
                                send(pfds[i].fd, CHOOSE_RIGHT, strlen(CHOOSE_RIGHT), 0);
                                auto port_number = rooms_data[make_int(choice, len_r)].get_port();
                                auto correct_version = convert_int_to_char_pointer(port_number);
                                send(pfds[i].fd,correct_version, strlen(correct_version), 0);
                                break;
                            }
                            else
                            {
                                send(pfds[i].fd, NOT_AVAILABLE, strlen(NOT_AVAILABLE), 0);
                            }
                        }

                        // send()
                    }
                }
            }
        }
    }
}