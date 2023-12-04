#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <netdb.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>

#define MAX_BUFF_SIZE 255
#define MAX_CLIENT 10

const int SizeMax = 255;

char buffer[MAX_BUFF_SIZE];

typedef struct
{
    char username[SizeMax];
    char password[SizeMax];
    int status;
} eType;

typedef struct nodeType
{
    eType data;
    struct nodeType *left, *right;
} node_Type;

typedef node_Type *Tree_T;
Tree_T T;

eType *a;
int total;
FILE *file1, *file2;
const char *file = "nguoidung.txt";
char last_username_signed_in[MAX_BUFF_SIZE];
char username_signin[MAX_BUFF_SIZE];
char username[MAX_BUFF_SIZE], password[MAX_BUFF_SIZE];
int signin_err_flag = 0;
int client_flag, server_flag, n;

#include "linklist.h"
#include "handle_client.h"

void readFile()
{
    if ((file1 = fopen("nguoidung.txt", "r")) == NULL)
    {
        printf("Cannot open %s!!!", "nguoidung.txt");
        exit(1);
    }

    char c;
    for (c = getc(file1); c != EOF; c = getc(file1))
        if (c == '\n')
            total = total + 1;
    rewind(file1);

    for (int i = 0; i < total + 1; i++)
    {
        eType tmp;
        float a, b;
        int ss;
        fscanf(file1, "%s\t%s\t%d", tmp.username, tmp.password, &tmp.status);
        InsertNode(tmp, &T);
    }

    inorderprint(T);
    
}

int isValidPort(char *port)
{
    int port_num = atoi(port);
    if (port_num < 1 || port_num > 65535)
    {
        return 0;
    }

    for (int i = 0; i < strlen(port); ++i)
    {
        if (!isdigit(port[i]))
        {
            return 0;
        }
    }

    return 1;
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("Usage: %s <PortNumber> \n", argv[0]);
        return 1;
    }

    // If the port is invalid, exit the program
    if (!isValidPort(argv[1]))
    {
        printf("Invalid port number!\n");
        exit(EXIT_FAILURE);
    }

    // Get data from account.txt
    readFile();

    // Socket variables declaration
    int sockfd;
    struct sockaddr_in server_addr, client_addr;

    int client_socket[MAX_CLIENT];
    int max_clients = MAX_CLIENT;

    for (int i = 0; i < max_clients; ++i)
    {
        client_socket[i] = 0;
    }

    printf("[+]Connecting to server...\n");

    // Create a TCP socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        printf("Create TCP socket failed!\n");
        exit(EXIT_FAILURE);
    }

    printf("[+]Successful create socket.\n");

    int opt = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt)) < 0)
    {
        perror("Setsockopt");
        return 0;
    }

    // Initialize server address struct
    memset(&server_addr, 0, sizeof(server_addr));
    memset(&client_addr, 0, sizeof(client_addr));

    // Config
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(atoi(argv[1]));

    // Binding
    if (bind(sockfd, (const struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("Error in binding: ");
        exit(EXIT_FAILURE);
    }

    printf("[+]Binding successful!\n");
    printf("[%s:%d]\n", inet_ntoa(server_addr.sin_addr),
           ntohs(server_addr.sin_port));

    // Listen for incomming connections
    if (listen(sockfd, 10) < 0)
    {
        printf("Listen failed!\n");
        exit(EXIT_FAILURE);
    }

    printf("[+]Server is running... Waiting for connections.\n");

    socklen_t len = sizeof(client_addr);
    int max_sd, activity, valread, sd;
    fd_set readfds;
    int new_socket;
    int n;

    while (1)
    {
        FD_ZERO(&readfds);

        FD_SET(sockfd, &readfds);
        max_sd = sockfd;

        for (int i = 0; i < max_clients; ++i)
        {
            sd = client_socket[i];
            if (sd > 0)
            {
                FD_SET(sd, &readfds);
            }
            if (sd > max_sd)
            {
                max_sd = sd;
            }
        }

        activity = select(max_sd + 1, &readfds, NULL, NULL, NULL);
        if (activity < 0)
        {
            printf("Select Error!\n");
            continue;
        }

        if (FD_ISSET(sockfd, &readfds))
        {
            if ((new_socket = accept(sockfd, (struct sockaddr *)&client_addr, &len)) < 0)
            {
                perror("Accept: ");
                exit(EXIT_FAILURE);
            }

            for (int i = 0; i < max_clients; ++i)
            {
                if (client_socket[i] == 0)
                {
                    client_socket[i] = new_socket;
                    printf("Adding to list of sockets as %d\n", i);
                    break;
                }
            }
        }

        for (int i = 0; i < max_clients; ++i)
        {
            sd = client_socket[i];

            if (FD_ISSET(sd, &readfds))
            {
                n = recv(sockfd, buffer, sizeof(buffer), 0);
                printf("buffer: %s", buffer);

                if (n == 0)
                {
                    getpeername(sd, (struct sockaddr *)&client_addr, &len);
                    printf("Host disconnected, ip %s, port %d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
                    close(sd);
                    client_socket[i] = 0;
                }
                else
                {
                    handle_client(sd);
                }
            }
        }
    }

    // Close the socket
    close(sockfd);

    return 0;
}
