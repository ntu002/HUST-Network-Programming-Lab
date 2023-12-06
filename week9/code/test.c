#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <unistd.h>
#include <poll.h>

#define MAX_BUFF_SIZE 255
#define CONNECTION_LIMIT 10

#include "linked_list.h"

LinkedList *userList;
const char *filename = "nguoidung.txt";
char usernameSignIn[MAX_BUFF_SIZE];
char last_username_signed_in[MAX_BUFF_SIZE];
char username[MAX_BUFF_SIZE];
char password[MAX_BUFF_SIZE];
int client_flag;
int server_flag;
int sign_in_error_flag = 0;
int server_socket, sendBytes;
char buffer[MAX_BUFF_SIZE];

#include "handle_client_connection.h"

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("Usage: %s <PortNumber>\n", argv[0]);
        return 1;
    }
    int port = atoi(argv[1]);
    userList = createLinkedList();
    readAccountsFromFile(userList, filename);

    struct sockaddr_in server_address, client_address;
    struct pollfd fds[CONNECTION_LIMIT + 1];
    int nfds = 1;

    if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("Error: ");
        return 0;
    }

    // Set master socket to allow multiple connections
    int opt = 1;
    if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt)) < 0)
    {
        perror("setsockopt");
        return 0;
    }

    bzero(&server_address, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = htonl(INADDR_ANY);
    server_address.sin_port = htons(port);

    if (bind(server_socket, (struct sockaddr *)&server_address, sizeof(server_address)))
    {
        perror("Error: ");
        return 0;
    }

    if (listen(server_socket, CONNECTION_LIMIT) == -1)
    {
        perror("Listen");
        return 1;
    }

    // Initialize the pollfd structure
    memset(fds, 0, sizeof(fds));
    fds[0].fd = server_socket;
    fds[0].events = POLLIN;

    printf("Server started successfully!\n");

    while (1)
    {
        if (poll(fds, nfds, -1) < 0)
        {
            perror("poll error");
            break;
        }

        if (fds[0].revents & POLLIN)
        {
            socklen_t addrlen = sizeof(client_address);
            int new_socket = accept(server_socket, (struct sockaddr *)&client_address, &addrlen);
            if (new_socket < 0)
            {
                perror("accept");
                continue;
            }

            printf("New connection, socket fd is %d, ip is: %s, port: %d\n",
                   new_socket, inet_ntoa(client_address.sin_addr), ntohs(client_address.sin_port));

            for (int i = 1; i <= CONNECTION_LIMIT; i++)
            {
                if (fds[i].fd == 0)
                {
                    fds[i].fd = new_socket;
                    fds[i].events = POLLIN;
                    nfds = (i + 1 > nfds) ? i + 1 : nfds;
                    break;
                }
            }
        }

        for (int i = 1; i < nfds; i++)
        {
            if (fds[i].fd > 0 && (fds[i].revents & POLLIN))
            {
                int n = recv(fds[i].fd, buffer, sizeof(buffer), 0);
                if (n <= 0)
                {
                    printf("Client disconnected, fd %d\n", fds[i].fd);
                    close(fds[i].fd);
                    fds[i].fd = 0;
                }
                else
                {
                    handle_client(fds[i].fd, n);
                }
            }
        }
    }

    for (int i = 0; i < nfds; i++)
    {
        if (fds[i].fd > 0)
        {
            close(fds[i].fd);
        }
    }

    freeLinkedList(userList);
    close(server_socket);
    return 0;
}

