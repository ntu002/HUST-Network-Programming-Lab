#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <netdb.h>
#include <stdbool.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <unistd.h>
#include <poll.h>

#define MAX_BUFF_SIZE 255
#define CONNECTION_LIMIT 10

typedef struct
{
    char username[MAX_BUFF_SIZE];
    char password[MAX_BUFF_SIZE];
    int status;
} eType;

typedef struct nodeType
{
    eType data;
    struct nodeType *left, *right;
} node_Type;

typedef node_Type *Tree_T;
Tree_T T;

#include "linklist.h"

const char *filename = "nguoidung.txt";
char usernameSignIn[MAX_BUFF_SIZE];
char last_username_signed_in[MAX_BUFF_SIZE];
char username[MAX_BUFF_SIZE];
char password[MAX_BUFF_SIZE];
int client_flag, server_flag;
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
    
    readFile(filename);

    struct sockaddr_in server_addr, client_addr;
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

    memset(&server_addr, 0, sizeof(server_addr));
    memset(&client_addr, 0, sizeof(client_addr));
    // Initialize server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(port);

    // Bind ther server socket
    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)))
    {
        perror("Error: ");
        return 0;
    }

    // Listen for incomming connections
    if (listen(server_socket, CONNECTION_LIMIT) == -1)
    {
        perror("Listen");
        return 1;
    }

    printf("[+]Server is running... Waiting for connections.\n");

    // Initialize the pollfd structure
    memset(fds, 0, sizeof(fds));
    fds[0].fd = server_socket;
    fds[0].events = POLLIN;

    // Accept and incoming connection
    while (1)
    {
        if (poll(fds, nfds, -1) < 0)
        {
            perror("poll error");
            break;
        }

        if (fds[0].revents & POLLIN)
        {
            socklen_t addrlen = sizeof(client_addr);
            int new_socket = accept(server_socket, (struct sockaddr *)&client_addr, &addrlen);
            if (new_socket < 0)
            {
                perror("accept");
                continue;
            }

            printf("[+]Accept successful!!!\n");
            printf("[%s:%d]\n\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

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
                    printf("[+]Server: Client disconnected - fd %d\n", fds[i].fd);
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

    freeTree(T);
    close(server_socket);
    return 0;
}

