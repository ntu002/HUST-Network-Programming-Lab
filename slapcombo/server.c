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
#include <pthread.h>

#define MAX_BUFF_SIZE 255
#define ACCOUNT_FILE "account.txt"

#define LISTENQ 1
#define MAX_CLIENT 10

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
int num_client = 0;

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

#include "linked_list.h"
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
        fscanf(file1, "%s\t%s", tmp.username, tmp.password);
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
        printf("Usage: ./server <PortNumber> \n");
        return 1;
    }

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
    listen(sockfd, 10);
    if (listen(sockfd, 10) < 0)
    {
        printf("Listen failed!\n");
        exit(EXIT_FAILURE);
    }
    
    printf("[+]Server is running... Waiting for connections.\n");
    
    // Accept and incoming connection

    int client_sock;
    socklen_t len = sizeof(client_addr);

    while(1) {
        client_sock = accept(sockfd, (struct sockaddr *)&client_addr, &len);

        if (client_sock < 0) {
            perror("Accept failed");
            exit(EXIT_FAILURE);
        }

        printf("[+]Accept successful!\n");
        printf("[%s:%d]\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

        pthread_t ptid;
        if(pthread_create(&ptid, NULL, handle_client, (void*)&client_sock) < 0) {
            perror("Could not create thread");
            exit(EXIT_FAILURE);
        }

        pthread_detach(ptid);
    }

    if (client_sock < 0) {
        perror("Accept failed");
        exit(EXIT_FAILURE);
    }
    // Close the socket
    close(sockfd);
    close(client_sock);

    return 0;
}
