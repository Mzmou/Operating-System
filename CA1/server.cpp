#pragma once
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
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
void set_non_blocking(int sock)
{
    int flags = fcntl(sock, F_GETFL, 0);
    if (flags == -1)
    {
        perror("fcntl F_GETFL failed");
        exit(EXIT_FAILURE);
    }
    if (fcntl(sock, F_SETFL, flags | O_NONBLOCK) == -1)
    {
        perror("fcntl F_SETFL failed");
        exit(EXIT_FAILURE);
    }
}
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
int make_socket(int &server_fd, char *ipaddr, int opt, struct sockaddr_in &server_addr)
{
    if (inet_pton(AF_INET, ipaddr, &(server_addr.sin_addr)) == -1)
        perror("FAILED: Input ipv4 address invalid");

    if ((server_fd = socket(PF_INET, SOCK_STREAM, 0)) == -1)
        perror("FAILED: Socket was not created");

    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1)
        perror("FAILED: Making socket reusable failed");

    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt)) == -1)
        perror("FAILED: Making socket reusable port failed");

    if (bind(server_fd, (const struct sockaddr *)(&server_addr), sizeof(server_addr)) == -1)
        perror("FAILED: Bind unsuccessfull");

    if (listen(server_fd, 20) == -1)
        perror("FAILED: Listen unsuccessfull");
    //  set_non_blocking(server_fd);

    return server_fd;
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
    memset(server_addr.sin_zero, 0, sizeof(server_addr.sin_zero));

    server_addr.sin_port = htons(strtol(argv[2], NULL, 10));
    server_fd = make_socket(server_fd, ipaddr, opt, server_addr);

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

        server_room_fd = make_socket(server_room_fd, ipaddr, opt, server_addr_room);

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

                    char buffer[BUFFER_SIZE];
                    memset(buffer, 0, BUFFER_SIZE);
                    int sents = send(new_fd, WRITE_NAME, strlen(WRITE_NAME), 0);
                    cout << sents << '\n';
                    char name_char[BUFFER_SIZE];
                    memset(name_char, 0, BUFFER_SIZE);

                    auto bbb = recv(new_fd, name_char, BUFFER_SIZE, 0); // recive name
                    cout << bbb << '\n';
                    auto name = string(name_char);
                    Client_attr temp(name);
                    clients_data.push_back(temp);
                    cout << "name succesful" << '\n';
                    auto avail_rooms = make_available_rooms_list(room_bit_map);
                    char *charIndices = new char[avail_rooms.length() + 1];
                    std::strcpy(charIndices, avail_rooms.c_str());
                    send(new_fd, charIndices, avail_rooms.length() + 1, 0); // send options
                    //  while (1)
                    // send(pfds[i].fd, charIndices, avail_rooms.length() + 1, 0); // send options
                    char choice[BUFFER_SIZE];
                    auto len_r = recv(new_fd, choice, BUFFER_SIZE, 0);
                    cout << choice << '\n'; // recieve choice
                    auto choice_number = make_int(choice, len_r);
                    if (rooms_data[choice_number].check_available())
                    {
                        rooms_data[choice_number].inc_member();
                        rooms_data[choice_number].add_player(temp);
                        auto port_number = rooms_data[choice_number].get_port();
                        send(new_fd, intToCharPointer(port_number), avail_rooms.length() + 1, 0);
                        auto room_pfd = rooms_data[choice_number].get_pfds();
                    }
                }
                else // message from user
                {

                    char buffer[BUFFER_SIZE];
                    memset(buffer, 0, BUFFER_SIZE);
                    recv(pfds[i].fd, buffer, BUFFER_SIZE, 0);
                    
                }
            }
        }
    }
}
// auto name = string(name_char);
// Client_attr temp(name);
// clients_data.push_back(temp);
// cout << "name succesful" << '\n';
//  while (1)

// auto avail_rooms = make_available_rooms_list(room_bit_map);
// char *charIndices = new char[avail_rooms.length() + 1];
// std::strcpy(charIndices, avail_rooms.c_str());
// send(pfds[i].fd, charIndices, avail_rooms.length() + 1, 0); // send options
// char choice[BUFFER_SIZE];
// auto len_r = recv(pfds[i].fd, choice, BUFFER_SIZE, 0); // recieve choice
// cout << '\n'
//<< pfds[i].fd << " start pushed" << '\n';
//{

// char name_char[BUFFER_SIZE];
// int sents = send(pfds[i].fd, WRITE_NAME, strlen(WRITE_NAME), 0); // send write name massage
// recv(pfds[i].fd, name_char, BUFFER_SIZE, 0);                     // recive name
// auto name = string(name_char);
// Client_attr temp(name);
// clients_data.push_back(temp);
// while (1)
//{
// auto avail_rooms = make_available_rooms_list(room_bit_map);
// char *charIndices = new char[avail_rooms.length() + 1];
// std::strcpy(charIndices, avail_rooms.c_str());
// send(pfds[i].fd, charIndices, avail_rooms.length() + 1, 0); // send available rooms
// char choice[BUFFER_SIZE];
// auto len_r = recv(pfds[i].fd, choice, BUFFER_SIZE, 0); // recieve choice
// cout<<len_r<<endl;
//  write(1, choice, len_r);
// if (check_available(make_int(choice, len_r), room_bit_map))
//{
// send(pfds[i].fd, CHOOSE_RIGHT, strlen(CHOOSE_RIGHT), 0); // send right_choice_massage
// auto port_number = rooms_data[make_int(choice, len_r)].get_port();
// auto correct_version = convert_int_to_char_pointer(port_number);
// send(pfds[i].fd, correct_version, strlen(correct_version), 0); // send port_number
// auto pfd_data = rooms_data[make_int(choice, len_r)].get_pfds();
// while (1)
//{

//  if (poll(pfd_data.data(), (nfds_t)(pfd_data.size()), -1) == -1)
// perror("FAILED: Poll");
// for (size_t i = 0; i < pfd_data.size(); ++i)
// {

//     // if (pfd_data[i].revents & POLLIN)
//     // {
//     //     struct sockaddr_in new_user_addr;
//     //     socklen_t new_size = sizeof(new_user_addr);
//     //     int new_fd_user = accept(server_fd, (struct sockaddr *)(&new_user_addr), &new_size);
//     //     write(1, NEW_CONNECTION, strlen(NEW_CONNECTION));
//     //     pfd_data.push_back(pollfd{new_fd_user, POLLIN, 0});
//     //     int sents = send(new_fd_user, WELLCOME, strlen(WELLCOME), 0); // write name
//     // }

//     // else
//     // {
//     //     char buffer[BUFFER_SIZE];
//     //     memset(buffer, 0, BUFFER_SIZE);
//     //     recv(pfd_data[i].fd, buffer, BUFFER_SIZE, 0); // getting start
//     //                                                   /// start game
//     // }
// }
//  }
//  }
// else
// {
// send(pfds[i].fd, NOT_AVAILABLE, strlen(NOT_AVAILABLE), 0);
//}
//  }

// send()
// }
//}