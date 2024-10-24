#include <vector>
#include "client_attr.h"
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstdlib>
#include <unistd.h>
#include <cstring>
#include <iostream>
#pragma once
using namespace std;
typedef struct pollfd pollfd;
const char *OPTIONS = "for rock type r,for paper type p and for scissor type s ";
class Room
{
public:
Room(vector<pollfd> pfds_room_,int port_,int fd_){
port=port_;
pfds_room=pfds_room_;
fd=fd_;
cout<<port<<" "<<fd<<" "<<'\n';
}
int get_port(){return port;}



private:
    vector <pollfd> pfds;
    int fd;
    bool empty;
    int port;
    vector <Client_attr> players;
    std::vector<pollfd> pfds_room;



};