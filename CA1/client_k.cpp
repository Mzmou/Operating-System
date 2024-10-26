#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstdlib>
#include <unistd.h>
#include <cstring>
#include <iostream>
using namespace std;

typedef struct pollfd pollfd;

#define STDIN 0
#define STDOUT 1
#define BUFFER_SIZE 1024

const char *CORRECT_ROOM = "You successfully joined the room!\n";
const char *CONNECTION = "You connected successfully\n";
char buffer[BUFFER_SIZE];
const char *first_n = "break_point1";
const char *sec_n = "break_point2";
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

    memset(buffer, 0, BUFFER_SIZE);
    recv(server_fd, buffer, BUFFER_SIZE, 0); ////what is your namw
    write(STDOUT, buffer, strlen(buffer));

    memset(buffer, 0, BUFFER_SIZE);
    read(STDIN, buffer, BUFFER_SIZE); // enter name
    send(server_fd, buffer, strlen(buffer), 0);

    memset(buffer, 0, BUFFER_SIZE);
    recv(server_fd, buffer, BUFFER_SIZE, 0); /// ask for choice
    write(STDOUT, buffer, strlen(buffer));

    while (1)
    {
        memset(buffer, 0, BUFFER_SIZE);
        read(STDIN, buffer, BUFFER_SIZE);
        send(server_fd, buffer, strlen(buffer), 0); /// enter choice
                                                    // write(STDOUT, first_n, strlen(first_n));
        memset(buffer, 0, BUFFER_SIZE);
        recv(server_fd, buffer, BUFFER_SIZE, 0); ////if the choice is right or wrong
                                                 // write(STDOUT, CONNECTION, strlen(CONNECTION));

        // write(STDOUT, CONNECTION, strlen(CONNECTION));

        if (!strcmp(buffer, CORRECT_ROOM))
        {
            write(STDOUT, buffer, strlen(buffer));
            // write(STDOUT, CONNECTION, strlen(CONNECTION));
            memset(buffer, 0, BUFFER_SIZE);
            // write(STDOUT, sec_n, strlen(sec_n));
            send(server_fd, sec_n, strlen(sec_n), 0);
            recv(server_fd, buffer, BUFFER_SIZE, 0); // port number
                                                     //  write(STDOUT, buffer, strlen(buffer));
            room_addr.sin_port = htons(strtol(buffer, NULL, 10));
            // write(STDOUT, CONNECTION, strlen(CONNECTION));
            write(STDOUT, buffer, strlen(buffer));
            if (connect(room_fd, (sockaddr *)(&room_addr), sizeof(room_addr)))
                perror("FAILED: Connect");
            else
            {
                write(STDOUT, CONNECTION, strlen(CONNECTION));
                send(server_fd, CONNECTION, strlen(CONNECTION), 0);
                break;

                // memset(buffer, 0, BUFFER_SIZE);
                // auto num = recv(room_fd, buffer, BUFFER_SIZE, 0); /// ask for choice
                // cout << num << '\n';
                // write(STDOUT, buffer, strlen(buffer));
            }
            // break;
        }
    }
    memset(buffer, 0, BUFFER_SIZE);
    auto bytes=recv(room_fd, buffer, BUFFER_SIZE, 0); ////what is your namw
    write(STDOUT, buffer, strlen(buffer));
    // auto num = recv(room_fd, buffer, BUFFER_SIZE, 0); /// ask for choice
    // cout << num << '\n';
}