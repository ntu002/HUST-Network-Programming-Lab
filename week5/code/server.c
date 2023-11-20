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

#define MAX_BUFFER_SIZE 1024

const int SizeMax = 100;

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

FILE *file1, *file2;

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
}

int separate(char *buff, char *result, char *number)
{
    int l1 = 0;
    int l2 = 0;
    for (int i = 0; buff[i] != '\0'; ++i)
    {
        if (isalpha(buff[i]))
        {
            result[l1] = buff[i];
            l1++;
        }
        else if (isdigit(buff[i]))
        {
            number[l2] = buff[i];
            l2++;
        }
        else if (!isspace(buff[i]))
        {
            printf("Error \n");
            return 1;
        }
    }
    result[l1] = '\0';
    number[l2] = '\0';

    return 0;
}

int dd1, dd2;

int main(int argc, char *argv[])
{

    if (argc != 2)
    {
        printf("Wrong parameters!!! \n");
        return 1;
    }

    int port = atoi(argv[1]);
    int server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;

    char buffer[MAX_BUFFER_SIZE];

    // Create socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0)
    {
        perror("Opening stream socket \n");
        exit(EXIT_FAILURE);
    }
    printf("[+]Successful create socket. \n");

    memset(&server_addr, 0, sizeof(server_addr));
    memset(&client_addr, 0, sizeof(client_addr));

    // Initialize server address
    server_addr.sin_family = AF_INET; // IPv4
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(port);

    // Bind ther server socket
    if (bind(server_socket, (const struct sockaddr *)&server_addr,
             sizeof(server_addr)) < 0)
    {
        perror("Error in binding \n");
        exit(EXIT_FAILURE);
    }

    if (listen(server_socket, 10) < 0)
    {
        perror("Listening failed \n");
        exit(EXIT_FAILURE);
    }
    else
        printf("[+]Server is running... Waiting for connections.\n");

    socklen_t client_addr_len = sizeof(client_addr);
    socklen_t n;

    int clientfd = accept(server_socket, (struct sockaddr *)&client_addr, &client_addr_len);
    if (clientfd < 0)
    {
        perror("Accept failed \n");
        exit(EXIT_FAILURE);
    }
    else
    {
        printf("[+]Accept successful!!!\n");
        printf("[%s:%d]\n\n", inet_ntoa(client_addr.sin_addr),
               ntohs(client_addr.sin_port));
    }

    readFile();

    char account[SizeMax], password[SizeMax], new_password[SizeMax];
    char result[SizeMax], number[SizeMax];

    while (1)
    {
        client_addr_len = sizeof(client_addr);

        memset(account, '\0', SizeMax);

        n = recv(clientfd, (char *)account, MAX_BUFFER_SIZE, 0);
        if (n < 0)
        {
            printf("[+]Server receive failed!!!\n");
            exit(EXIT_FAILURE);
        }
        account[n] = '\0';

        // func1(account);
        dd1 = 0;
        Tree_T c1;
        c1 = SearchUsername(account, T);
        if (c1 != NULL)
        {
            dd1 = c1->data.status;
            printf("%d", c1->data.status);
            // char *ack = "Account not found";
            // sendto(sockfd, (const char *)ack, strlen(ack), MSG_CONFIRM, (const struct sockaddr *)&client_address, len);

            n = send(clientfd, "Insert Password", sizeof("Insert Password"), 0);
            if (n < 0)
            {
                printf("[+]Server send failed!!!\n");
                exit(EXIT_FAILURE);
            }
            printf("Insert Password \n");

            int inva_account = 0;
            while (inva_account <= 2)
            {
                memset(password, '\0', SizeMax);
                n = recv(clientfd, (char *)password, MAX_BUFFER_SIZE, 0);
                if (n < 0)
                {
                    printf("[+]Server receive failed\n");
                    exit(EXIT_FAILURE);
                }

                password[n] = '\0';

                if (strcmp(c1->data.password, password) != 0)
                {
                    inva_account++;
                    n = send(clientfd, "Not OK", sizeof("Not OK"), 0);
                    if (n < 0)
                    {
                        printf("[+]Server send failed!!!\n");
                        exit(EXIT_FAILURE);
                    }
                    printf("NOT OK \n");
                }
                else if (c1->data.status == 1)
                {
                    n = send(clientfd, "OK", sizeof("OK"), 0);
                    if (n < 0)
                    {
                        printf("[+]Server send failed!!!\n");
                        exit(EXIT_FAILURE);
                    }
                    printf("OK \n");

                    while (1)
                    {
                        n = recv(clientfd, (char *)buffer, MAX_BUFFER_SIZE, 0);
                        if (n < 0)
                        {
                            printf("[+]Server receive failed\n");
                            exit(EXIT_FAILURE);
                        }
                        buffer[n] = '\0';

                        if (strcmp(buffer, "bye") == 0)
                        {
                            strcpy(c1->data.password, "bye");
                            n = send(clientfd, "Goodbye hust", sizeof("Goodbye hust"), 0);
                            if (n < 0)
                            {
                                printf("[+]Server send failed!!!\n");
                                exit(EXIT_FAILURE);
                            }
                            break;
                        }

                        char new_password[MAX_BUFFER_SIZE];
                        strcpy(new_password, buffer);

                        memset(result, '\0', SizeMax);
                        memset(number, '\0', SizeMax);
                        int x = separate(new_password, result, number);
                        if (x == 1)
                        {
                            printf("[+] [Error]: Invalid characters.\n");
                            strcpy(result, "Error: Invalid character");
                        }

                        char ack[MAX_BUFFER_SIZE];

                        if (strlen(result) == 0)
                        {
                            strcpy(ack, number);
                        }
                        else if (strlen(number) == 0)
                        {
                            strcpy(ack, result);
                        }
                        else
                        {
                            strcpy(ack, result);
                            strcat(ack, "\n");
                            strcat(ack, number);
                        }

                        n = send(clientfd, (const char *)ack, sizeof(ack), 0);
                        if (n < 0)
                        {
                            printf("[+]Send failed\n");
                            exit(EXIT_FAILURE);
                        }

                        printf("[+]Server reply: %s\n", ack);
                        printf("[+]Message was sent to client....\n");
                    }
                }
                else
                {
                    if (c1->data.status == 0)
                    {
                        n = send(clientfd, "Account not ready", sizeof("Account not ready"), 0);
                        if (n < 0)
                        {
                            printf("[+]Send failed\n");
                            exit(EXIT_FAILURE);
                        }
                    }
                    else if (c1->data.status == 2)
                    {
                        n = send(clientfd, "Account is blocked", sizeof("Account is blocked"), 0);
                        if (n < 0)
                        {
                            printf("[+]Send failed\n");
                            exit(EXIT_FAILURE);
                        }
                    }
                    break;
                }
            }

            if (inva_account > 2)
            {
                n = send(clientfd, "Account is blocked", sizeof("Account is blocked"), 0);
                if (n < 0)
                {
                    printf("[+]Send failed\n");
                    exit(EXIT_FAILURE);
                }
                c1->data.status = 2;
                break;
            }
        }
        else
        {
            n = send(clientfd, "Account not found", sizeof("Account not found"), 0);
            if (n < 0)
            {
                printf("[+]Send failed\n");
                exit(EXIT_FAILURE);
            }
            dd1 = -1;
        }
    }

    close(server_socket);
    return 1;
}