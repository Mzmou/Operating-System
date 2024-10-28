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
#include <unordered_map>
#include "client_attr.h"
typedef struct pollfd pollfd;
#include "room.cpp"
#include <fcntl.h>
#define STDIN 0
#define STDOUT 1
#define BUFFER_SIZE 1024
#pragma once
const char *SERVER_LAUNCHED = "Server Launched!\n";
const char *ROOM_LAUNCHED = "Room Launched!\n";
const char *WELCOMED = "Hello! Welcome to our game. what is your name?\n";
const char *CHOOSE_ROOM = "Please choose a room to enter.\n";
const char *CORRECT_ROOM = "You successfully joined the room!\n";
const char *INVALID_ROOM = "You chose an invalid room. please choose a correct room again.\n";
const char *FULL_ROOM = "You chose a full room. please choose another room again.\n";
const char *CONNECTION = "You connected successfully\n";
const char *WRITE_NAME = "Write your name\n";
const char *NOT_AVAILABLE = "room is not availble\n";
const char *CHOOSE_RIGHT = "You can enter the room\n";
const char *first_n = "break_point1";
const char *WRONG_ROOM = "255255255";
const char *UN_ROOM = "255255256";
const char *end_game_massage = "END_GAME\n";
template <typename K, typename V>
void printUnorderedMap(const std::unordered_map<K, V> &umap)
{
    for (const auto &pair : umap)
    {
        std::cout << "Key: " << pair.first << ", Value: " << pair.second << std::endl;
    }
}
void execute_end_game(vector<Room> rooms)
{
    for (int j = 0; j < rooms.size(); j++)
    {
        auto players_in_game = rooms[j].get_players();

        for (int i = 0; i < players_in_game.size(); i++)
        {
            players_in_game[i].print_status();
        }
    }
    exit(1);
}
int find_room_number(int fd, vector<Room> rooms)
{
    for (int i = 0; i < rooms.size(); i++)
    {
        if (rooms[i].check_player_in(fd))
            return i;
    }
    return -1;
}
string make_available_rooms_list(vector<Room> rooms)
{
    string result;

    for (size_t i = 0; i < rooms.size(); ++i)
    {
        if (rooms[i].check_available())
        {
            if (i)
                result += " ";

            result += to_string(i);
        }
    }
    result += "\n";

    return result;
}
int search_room(int fd, vector<int> fds)
{
    for (int i = 0; i < fds.size(); i++)
    {
        if (fds[i] == fd)
            return i;
    }
    return -1;
}
bool search_clinets(int fd, vector<pollfd> clients)
{
    for (int i = 0; i < clients.size(); i++)
    {
        if (clients[i].fd == fd)
        {
            return 1;
        }
    }
    return 0;
}
char *convert_string_to_char(string input)
{
    char *charIndices = new char[input.length() + 1];
    std::strcpy(charIndices, input.c_str());
    return charIndices;
}
char *intToCharPointer(int number)
{
    std::string strNumber = std::to_string(number);

    char *charNumber = new char[strNumber.length() + 1];

    strcpy(charNumber, strNumber.c_str());

    return charNumber;
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
    std::unordered_map<int, char *> player_fd_name_map;

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

        write(STDOUT, ROOM_LAUNCHED, strlen(ROOM_LAUNCHED));
        room_fds.push_back(room_fd);
        pfds_room.push_back(pollfd{room_fd, POLLIN, 0});
        pfds.push_back(pollfd{room_fd, POLLIN, 0});

        Room temp_room(pfds_room, first_port + i, room_fd);
        rooms_data.push_back(temp_room);
    }
    int flag_game = 0;
    int choice_number = 0;
    vector<pollfd> clients_in_game;
    int room_number = 0;
    pfds.push_back(pollfd{0, POLLIN, 0});
    while (1)
    {

        if (poll(pfds.data(), (nfds_t)(pfds.size()), -1) == -1)
            perror("FAILED: Poll");
        for (size_t i = 0; i < pfds.size(); ++i)
        {

            if (pfds[i].revents & POLLIN)
            {
                if (pfds[i].fd == server_fd)
                {

                    struct sockaddr_in new_addr;
                    Client_attr new_user;
                    socklen_t new_size = sizeof(new_addr);
                    int new_fd = accept(server_fd, (struct sockaddr *)(&new_addr), &new_size);

                    new_user.set_fd(new_fd);

                    players.push_back(new_user);
                    pfds.push_back(pollfd{new_fd, POLLIN, 0});

                    send(new_fd, WRITE_NAME, strlen(WRITE_NAME), 0);
                }
                else if (search_room(pfds[i].fd, room_fds) != -1)
                {

                    room_number = search_room(pfds[i].fd, room_fds);
                    if (rooms_data[room_number].get_size() < 2)
                    {
                        auto cli = rooms_data[room_number].add_player(pfds[i].fd);
                        pfds.push_back(pollfd{cli, POLLIN, 0});
                        clients_in_game.push_back(pollfd{cli, POLLIN, 0});
                        rooms_data[room_number].set_flag_game();
                    }
                    if (rooms_data[room_number].get_size() == 2)
                    {
                        write(1, first_n, strlen(first_n));

                        // ;
                        //  break;
                    }
                }

                else
                {
                    char buffer[BUFFER_SIZE];
                    int fd = pfds[i].fd;
                    room_number = find_room_number(fd, rooms_data);

                    // cout << "flag game is " << rooms_data[room_number].get_flag_game() << '\n';
                    cout << "room number is " << room_number << '\n';
                    printUnorderedMap(player_fd_name_map);
                    memset(buffer, 0, BUFFER_SIZE);
                    recv(fd, buffer, BUFFER_SIZE, 0);
                    // if (fd == 0 and !strcmp(buffer,end_game_massage))
                    // {
                    //     execute_end_game(rooms_data);
                    // }
                    write(1, buffer, BUFFER_SIZE);
                    if (room_number != -1 && rooms_data[room_number].get_flag_game())
                    {
                        rooms_data[room_number].add_choice(buffer, fd);
                        // if(rooms_data[room_number].get_choice_ready()){

                        // }
                    }
                    auto length = 0;
                    while (length < players.size())
                    {
                        if (players[length].check_fd(fd))
                        {
                            if (!players[length].check_set_name())
                            {
                                players[length].set_name(buffer);
                                auto available_room = convert_string_to_char(make_available_rooms_list(rooms_data));
                                player_fd_name_map[fd] = buffer;
                                //  cout << player_fd_name_map[fd] << '\n';

                                send(fd, available_room, strlen(available_room), 0);
                            }
                            else
                            {
                                choice_number = stoi(buffer);

                                if (rooms_data[choice_number].check_available())
                                {

                                    rooms_data[choice_number].inc_member();

                                    memset(buffer, 0, BUFFER_SIZE);
                                    auto port_number = rooms_data[choice_number].get_port();
                                    auto n = send(fd, intToCharPointer(port_number), 1024, 0);
                                    cout << n << endl;
                                    memset(buffer, 0, BUFFER_SIZE);
                                    // auto name = player_fd_name_map[fd];
                                    cout << player_fd_name_map[fd];
                                    // write(1, name, strlen(name));
                                    //  Client_attr temp(fd, name);
                                    //  rooms_data[choice_number].add_player(temp);
                                    //    flag_game = (!rooms_data[choice_number].check_available());
                                    //  cout << "flag game is " << flag_game << '\n';
                                    break;
                                }

                                else
                                {
                                    send(fd, WRONG_ROOM, strlen(WRONG_ROOM), 0);
                                }
                            }
                        }
                        length++;
                    }
                }
            }
        }
        // if(flag_game)
        // rooms_data[room_number].play();
    }
}
