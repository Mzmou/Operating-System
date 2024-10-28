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
#include "client_attr.h"
#include <fcntl.h>
const char *NEW_CONNECTION = "New Connection!\n";
#pragma once
using namespace std;
typedef struct pollfd pollfd;
const char *OPTIONS = "for rock type r,for paper type p and for scissor type s ";
const char *rock = "r\n";
const char *scissor = "s\n";
const char *paper = "p\n";
const char *GOT_CHOICE = "Got your choice\n";
const char *END_MASSAGE = "END";
const char *WIN_MASSAGE = "YOU WON\n";
const char *LOST_MASSAGE = "YOU LOST\n";
const char *EQUAL = "equal\n";
const char *fifi = "fifi\n";
const char *NO_CHOICE = "d";
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

void broadcast()
{
    int sock, broadcast = 1, opt = 1;
    char buffer[1024] = {0};
    struct sockaddr_in bc_address;
    sock = socket(AF_INET, SOCK_DGRAM, 0);
    setsockopt(sock, SOL_SOCKET, SO_BROADCAST, &broadcast, sizeof(broadcast));
    setsockopt(sock, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt));

    bc_address.sin_family = AF_INET;
    bc_address.sin_port = htons(8080);
    bc_address.sin_addr.s_addr = inet_addr("10.0.2.255");

    bind(sock, (struct sockaddr *)&bc_address, sizeof(bc_address));
    memset(buffer, 0, 1024);
    int a = sendto(sock, OPTIONS, strlen(OPTIONS), 0, (struct sockaddr *)&bc_address, sizeof(bc_address));
}
class Room
{
public:
    Room(vector<pollfd> pfds_room_, int port_, int fd_)
    {
        port = port_;
        pfds_room = pfds_room_;
        fd = fd_;
        num_people = 0;
        choice_counter = 0;
        choices_ready = 0;
        flag_game = 0;
        name_counter = 0;
        // char choices[0][1024];
        memset(choices[0], 0, 1024);
        memset(choices[1], 0, 1024);
        // char choices[1][1024];
        //  memset(choices[1], 0, 1024);
        cout << port << " " << fd << " " << '\n';
    }
    int get_port() { return port; }
    vector<Client_attr> get_players()
    {
        return players_during_poling;
    }

    int check_player_in(int fd)
    {
        for (int i = 0; i < players_during_poling.size(); i++)
        {
            if (players_during_poling[i].check_fd(fd))
                return 1;
        }
        return 0;
    }
    std::vector<pollfd> get_pfds() { return pfds_room; }
    bool check_available()
    {
        if (num_people == 2)
            return 0;
        return 1;
    }
    // void add_player(Client_attr new_player)
    // {
    //     players.push_back(new_player);
    //     inc_member();
    // }
    void play()
    {
        // int flag = 10;

        char *buffer2[1024];
        cout << choices[0] << " " << choices[1] << " \n";

        if (!strcmp(choices[0], rock) && !strcmp(choices[1], scissor))
        {
            players_during_poling[0].update_win();
            players_during_poling[1].update_loss();
            //  cout << "a\n";
            show_result(1);
        }
        else if (!strcmp(choices[0], rock) && !strcmp(choices[1], paper))
        {
            players_during_poling[0].update_loss();
            players_during_poling[1].update_win();
            show_result(2);
            // cout << "b\n";
        }
        else if (!strcmp(choices[0], paper) && !strcmp(choices[1], scissor))
        {
            players_during_poling[0].update_loss();
            players_during_poling[1].update_win();
            show_result(1);
            // cout << "c\n";
        }
        else if (!strcmp(choices[1], rock) && !strcmp(choices[0], scissor))
        {
            players_during_poling[1].update_win();
            players_during_poling[0].update_loss();
            cout << "d\n";
            // show_result(2);
        }
        else if (!strcmp(choices[1], rock) && !strcmp(choices[0], paper))
        {
            players_during_poling[1].update_loss();
            players_during_poling[0].update_win();
            show_result(1);
            // cout << "e\n";
        }
        else if (!strcmp(choices[1], paper) && !strcmp(choices[0], scissor))
        {
            players_during_poling[1].update_loss();
            players_during_poling[0].update_win();
            show_result(1);

            // cout << "f\n";
        }
        else if (!strcmp(choices[1], NO_CHOICE) && (!strcmp(choices[0], rock) || !strcmp(choices[0], paper) || !strcmp(choices[0], scissor)))
        {
            players_during_poling[1].update_loss();
            players_during_poling[0].update_win();
            show_result(1);

            // cout << "f\n";
        }
        else if (!strcmp(choices[0], NO_CHOICE) && (!strcmp(choices[1], rock) || !strcmp(choices[1], paper) || !strcmp(choices[1], scissor)))
        {
            players_during_poling[1].update_loss();
            players_during_poling[0].update_win();
            show_result(2);

            // cout << "f\n";
        }
        else if (!strcmp(choices[1], NO_CHOICE) && !strcmp(choices[0], NO_CHOICE))
        {
            players_during_poling[1].update_loss();
            players_during_poling[0].update_win();
            show_result(0);

            // cout << "f\n";
        }
        else
        {
            write(1, fifi, strlen(fifi));
            players_during_poling[0].update_equal();
            players_during_poling[1].update_equal();

            show_result(0);
        }

        players_during_poling[0].print_status();
        players_during_poling[1].print_status();
        // players_during_poling[0].reset_choice();
        // players_during_poling[1].reset_choice();
        choices_ready = 0;
        choice_counter = 0;
    }
    int add_player(int room_fd)
    {
        struct sockaddr_in new_addr;
        Client_attr new_user;
        socklen_t new_size = sizeof(new_addr);
        int new_fd = accept(room_fd, (struct sockaddr *)(&new_addr), &new_size);

        write(1, NEW_CONNECTION, strlen(NEW_CONNECTION));
        new_user.set_fd(new_fd);
       // new_user.set_name(players[name_counter].get_name());

         players_during_poling.push_back(new_user);
        // name_counter++;

        // pfds_room.push_back(pollfd{new_fd, POLLIN, 0});

        // send(new_fd, OPTIONS, strlen(OPTIONS), 0);
        return new_fd;
    }
    // void add_choice(char *choice) {}
    int get_size() { return players_during_poling.size(); }
    void inc_member() { num_people += 1; }
    void add_choice(char *choice_, int fd_player)
    {

        for (int i = 0; i < players_during_poling.size(); i++)
        {
            if (players_during_poling[i].check_fd(fd_player))
                strcpy(choices[i], choice_);
        }

        // cout << choices[choice_counter] << '\n';
        choice_counter += 1;
        cout << "choice counter is " << choice_counter << '\n';
        // write(1, fifi, strlen(fifi));
        if (choice_counter == 2)

        {
            write(1, fifi, strlen(fifi));
            play();
            // write(1, fifi, strlen(fifi));
        }
    }
    int get_choice_ready() { return choices_ready; }
    void set_flag_game() { flag_game = 1; }
    int get_flag_game() { return flag_game; }
    void add_player(Client_attr cli) { players.push_back(cli); }
    // int add_player(Client_att)

private:
    void show_result(int winner)
    {
        if (winner == 1)
        {
            send(players_during_poling[0].get_fd(), WIN_MASSAGE, strlen(WIN_MASSAGE), 0);
            send(players_during_poling[1].get_fd(), LOST_MASSAGE, strlen(LOST_MASSAGE), 0);
        }

        else if (winner == 2)
        {
            send(players_during_poling[0].get_fd(), LOST_MASSAGE, strlen(LOST_MASSAGE), 0);
            send(players_during_poling[1].get_fd(), WIN_MASSAGE, strlen(WIN_MASSAGE), 0);
        }
        else
        {

            send(players_during_poling[0].get_fd(), EQUAL, strlen(EQUAL), 0);
            send(players_during_poling[1].get_fd(), EQUAL, strlen(EQUAL), 0);
        }
    }
    int num_people;
    int choices_ready;
    char choices[2][1024];
    int choice_counter;
    int name_counter;
    // char choices[1][1024];

    int fd;
    bool empty;
    int port;
    int flag_game;
    vector<Client_attr> players;
    vector<Client_attr> players_during_poling;
    std::vector<pollfd> pfds_room;
};