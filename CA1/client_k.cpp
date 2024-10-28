#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstdlib>
#include <unistd.h>
#include <cstring>
#include <iostream>
#include <fcntl.h>

#include <string.h>

#include <sys/select.h>
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
const char *END_MASSAGE = "END";
const char *WRONG_ROOM = "255255255";
const char *UN_ROOM = "255255256";
const char *OPTIONS = "for rock type r,for paper type p and for scissor type s \n";
const char *DEFAULT_VALUE = "d";

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
    int a = recv(sock, buffer, 1024, 0);
    write(1, buffer, 1024);
    cout << a;
}
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
int main(int argc, char *argv[])
{
    if (argc != 3)
        perror("Invalid Arguments");

    char *ipaddr = argv[1];
    struct sockaddr_in server_addr;
    struct sockaddr_in room_addr;
    int server_fd, opt = 1;
    int room_fd;
    // write(1, first_n, strlen(first_n));
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
    // write(1, first_n, strlen(first_n));
    recv(server_fd, buffer, BUFFER_SIZE, 0);
    write(STDOUT, buffer, strlen(buffer));

    memset(buffer, 0, BUFFER_SIZE);
    read(STDIN, buffer, BUFFER_SIZE);
    send(server_fd, buffer, strlen(buffer), 0);

    memset(buffer, 0, BUFFER_SIZE);
    recv(server_fd, buffer, BUFFER_SIZE, 0);
    write(STDOUT, buffer, strlen(buffer));
    while (1)
    {
        memset(buffer, 0, BUFFER_SIZE);
        read(STDIN, buffer, BUFFER_SIZE);
        send(server_fd, buffer, strlen(buffer), 0);
        memset(buffer, 0, BUFFER_SIZE);
        recv(server_fd, buffer, BUFFER_SIZE, 0);

        if (strcmp(buffer, WRONG_ROOM))
        {
            room_addr.sin_port = htons(strtol(buffer, NULL, 10));
            write(STDOUT, buffer, strlen(buffer));
            write(STDOUT, CONNECTION, strlen(CONNECTION));
            if (connect(room_fd, (sockaddr *)(&room_addr), sizeof(room_addr)))
                perror("FAILED: Connect");
            else
            {
                write(STDOUT, CONNECTION, strlen(CONNECTION));
                break;
            }
        }
    }
    // memset(buffer, 0, BUFFER_SIZE);
    // auto bytes = recv(room_fd, buffer, BUFFER_SIZE, 0);
    // write(STDOUT, buffer, strlen(buffer));
    while (1)
    {
        write(1, OPTIONS, strlen(OPTIONS));

        // Prepare for select
        fd_set read_fds;
        struct timeval timeout;

        // Clear the set and add stdin to it
        FD_ZERO(&read_fds);
        FD_SET(STDIN_FILENO, &read_fds);

        // Set timeout to 10 seconds
        timeout.tv_sec = 10;
        timeout.tv_usec = 0;

        // Wait for input on stdin
        int activity = select(STDIN_FILENO + 1, &read_fds, NULL, NULL, &timeout);

        if (activity < 0)
        {
            perror("select error");
            break; // Handle select error
        }
        else if (activity == 0)
        {
            // Timeout occurred, use the default value
            strncpy(buffer, DEFAULT_VALUE, BUFFER_SIZE);
        }
        else
        {
            // Input is available, read it
            memset(buffer, 0, BUFFER_SIZE);
            read(STDIN_FILENO, buffer, BUFFER_SIZE);
        }
        //
        // Send the buffer to the room
        send(room_fd, buffer, strlen(buffer), 0);
        memset(buffer, 0, BUFFER_SIZE);

        // Receive response from the room
        auto bytes = recv(room_fd, buffer, BUFFER_SIZE, 0);
        write(1, buffer, bytes); // Use bytes for accurate write length
    }
}