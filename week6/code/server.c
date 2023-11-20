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
#define ACCOUNT_FILE "account.txt"

const int SizeMax = 255;

typedef struct
{
    char username[SizeMax];
    char password[SizeMax];
    int status;
    char code[SizeMax];
    int count;
    int check;
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

#include "linklist.h"
#include "handle_client.h"

FILE *file1, *file2;

void readFile()
{
    if ((file1 = fopen(ACCOUNT_FILE, "r")) == NULL)
    {
        printf("Cannot open %s!!!", ACCOUNT_FILE);
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
    // if (!isValidPort(argv[1]))
    // {
    //     printf("Invalid port number!\n");
    //     return 1;
    // }

    // Get data from account.txt
    readFile();

    // Socket variables declaration
    int sockfd;
    struct sockaddr_in server_addr, client_addr;

    printf("[+]Connecting to server...\n");

    // Create a TCP socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        printf("Create TCP socket failed!\n");
        exit(EXIT_FAILURE);
    }

    printf("[+]Successful create socket.\n");

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
        perror("Error in binding!\n");
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
    pid_t pid = fork();

    while (1)
    {
        // Accept a connection
        int clientfd;
        clientfd = accept(sockfd, (struct sockaddr *)&client_addr, &len);
        if (clientfd < 0)
        {
            perror("Error: \n");
            // exit(EXIT_FAILURE);
        }

        printf("Accept successful!\n");
        printf("[%s:%d]\n", inet_ntoa(client_addr.sin_addr),
               ntohs(client_addr.sin_port));

        // Fork a child process to handle the client connection.
        pid = fork();
        if (pid == -1)
        {
            close(clientfd);
            printf("Fork failed!\n");
            exit(1);
        }
        else if (pid > 0)
        {
            close(clientfd);
            // Non-blocking wait for the child process to exit.
            // int status;
            // do
            // {
            //     waitpid(pid, &status, WNOHANG);
            // } while (!WIFEXITED(status) && !WIFSIGNALED(status));
            continue;
        }
        else if (pid == 0)
        {
            // Close the original socket in the child process
            close(sockfd);

            // Handle the client
            handle_client(clientfd);

            // Terminate the child process
            exit(EXIT_SUCCESS);
        }
    }

    // Close the socket
    close(sockfd);

    return 0;
}
