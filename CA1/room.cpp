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
const char *NEW_CONNECTION = "New Connection!\n";
#pragma once
using namespace std;
typedef struct pollfd pollfd;
const char *OPTIONS = "for rock type r,for paper type p and for scissor type s ";
class Room
{
public:
    Room(vector<pollfd> pfds_room_, int port_, int fd_)
    {
        port = port_;
        pfds_room = pfds_room_;
        fd = fd_;
        num_people = 0;
        cout << port << " " << fd << " " << '\n';
    }
    int get_port() { return port; }
    std::vector<pollfd> get_pfds() { return pfds_room; }
    bool check_available()
    {
        if (num_people == 2)
            return 0;
        return 1;
    }
    void add_player(Client_attr new_player)
    {
        players.push_back(new_player);
        inc_member();
    }
    // bool check_ready_to_play(){return (num)}
    void play()
    {
        char *buffer2[1024];
        while (1)

        {
            cout << "hidiii" << '\n';

            if (poll(pfds_room.data(), (nfds_t)(pfds_room.size()), -1) == -1)
                perror("FAILED: Poll");
            for (size_t i = 0; i < pfds_room.size(); ++i)
            {
                if (pfds_room[i].revents & POLLIN)
                {
                    if (pfds_room[i].fd == fd)
                    {
                        // recv(fd, buffer2, 1024, 0);
                        struct sockaddr_in new_addr;
                        Client_attr new_user;
                        socklen_t new_size = sizeof(new_addr);
                        int new_fd = accept(fd, (struct sockaddr *)(&new_addr), &new_size);

                        write(1, NEW_CONNECTION, strlen(NEW_CONNECTION));

                        pfds_room.push_back(pollfd{new_fd, POLLIN, 0});

                        send(new_fd, NEW_CONNECTION, strlen(NEW_CONNECTION), 0);
                    }
                    else
                    {

                    }
                    // new user
                }
                /* code */
            }
        }
    }

private:
    int num_people;
    void inc_member() { num_people += 1; }
    // vector<pollfd> pfds;
    int fd;
    bool empty;
    int port;
    vector<Client_attr> players;
     vector<Client_attr> players_during_poling;
    std::vector<pollfd> pfds_room;
};