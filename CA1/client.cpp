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

#define STDIN 0
#define STDOUT 1
#define BUFFER_SIZE 1024
const char *WRITE_NAME = "Write your name\n";
const char *NOT_AVAILABLE = "room is not availble\n";
const char *CHOOSE_RIGHT = "You can enter the room\n";

int main(int argc, char *argv[])
{
    if (argc != 3)
        perror("Invalid Arguments");

    char *ipaddr = argv[1];
    struct sockaddr_in server_addr;
    struct sockaddr_in room_addr;
    int server_fd, opt = 1;
    int room_fd;

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
    if (connect(server_fd, (sockaddr *)(&server_addr), sizeof(server_addr)))
        perror("FAILED: Connect");
    ///////second port

    room_addr.sin_family = AF_INET;
    if (inet_pton(AF_INET, ipaddr, &(room_addr.sin_addr)) == -1)
        perror("FAILED: Input ipv4 address invalid");

    if ((room_fd = socket(PF_INET, SOCK_STREAM, 0)) == -1)
        perror("FAILED: Socket was not created");

    if (setsockopt(room_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1)
        perror("FAILED: Making socket reusable failed");

    if (setsockopt(room_fd, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt)) == -1)
        perror("FAILED: Making socket reusable port failed");

    memset(room_addr.sin_zero, 0, sizeof(room_addr.sin_zero));
    char buffer[BUFFER_SIZE];
    memset(buffer, 0, BUFFER_SIZE);

    recv(server_fd, buffer, 1024, 0); // take name massage
    write(1, buffer, strlen(buffer));
    memset(buffer, 0, BUFFER_SIZE);
    read(STDIN, buffer, BUFFER_SIZE);
    send(server_fd, buffer, strlen(buffer), 0); // give name

    memset(buffer, 0, BUFFER_SIZE);
    int bytes_received = recv(server_fd, buffer, 1024, 0); // get option
    write(1, buffer, strlen(buffer));
    memset(buffer, 0, BUFFER_SIZE);
    read(STDIN, buffer, BUFFER_SIZE);
    send(server_fd, buffer, BUFFER_SIZE, 0);

    bytes_received = recv(server_fd, buffer, 1024, 0); // get port
    write(1, buffer, strlen(buffer));
    room_addr.sin_port = htons(strtol(buffer, NULL, 10));
    if (connect(room_fd, (sockaddr *)(&room_addr), sizeof(room_addr)))
        perror("FAILED: Connect");

    while (1)
    {
        char buffer[BUFFER_SIZE];
        memset(buffer, 0, BUFFER_SIZE);

       
        read(STDIN, buffer, BUFFER_SIZE);
        send(server_fd, buffer, strlen(buffer), 0);
    }
}

// memset(buffer, 0, BUFFER_SIZE);
// read(STDIN, buffer, BUFFER_SIZE);
// send(server_fd, buffer, BUFFER_SIZE, 0); // send nae// send _start
// while (1)
//{

// cout<<bytes_received<<'\n';

// memset(buffer, 0, BUFFER_SIZE);
// bytes_received = recv(server_fd, buffer, 1024, 0);//available rooms
// write(1, buffer, strlen(buffer));

// while (!strcmp(buffer, WRITE_NAME))
// {
//     read(STDIN, buffer, BUFFER_SIZE); // enter name
//     send(server_fd, buffer, strlen(buffer), 0);
//     bytes_received = recv(server_fd, buffer, 1024, 0); // availabel rooms
//     write(1, buffer, bytes_received);
//     cout << "**********************************************" << endl;
//     // bytes_received = recv(server_fd, buffer, 1024, 0);
//     read(STDIN, buffer, BUFFER_SIZE); // send room
//     send(server_fd, buffer, strlen(buffer), 0);
//     bytes_received = recv(server_fd, buffer, 1024, 0); // if we choose correct

//     while (!strcmp(buffer, CHOOSE_RIGHT))
//     {
//         cout << "we choose right \n";
//         write(1, buffer, bytes_received);
//         // struct sockaddr_in server_addr_room;
//         bytes_received = recv(server_fd, buffer, 1024, 0);
//         // char *port_num = new char[bytes_received + 1];
//         //  for(int i=0;i<=bytes_received;i++){
//         //      port_num[i]=buffer[i];
//         //  }
//         //  port_num=buffer;
//         write(1, buffer, bytes_received);
//         // memset(server_addr_room.sin_zero, 0, sizeof(server_addr_room.sin_zero));

//         room_addr.sin_port = htons(strtol(buffer, NULL, 10));

//         if (connect(room_fd, (sockaddr *)(&room_addr), sizeof(room_addr)))
//             perror("FAILED: Connect");
//         while (1)
//         {
//             char buffer[BUFFER_SIZE];
//             memset(buffer, 0, BUFFER_SIZE);

//             read(STDIN, buffer, BUFFER_SIZE);
//             send(server_fd, buffer, strlen(buffer), 0);
//             int bytes_received = recv(server_fd, buffer, 1024, 0);
//             // cout<<bytes_received<<'\n';
//             write(1, buffer, strlen(buffer));
//         }
//     }
// }
// }