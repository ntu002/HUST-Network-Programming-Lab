#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#define BUFF_SIZE 1024
#define MAX_USERNAME_LENGTH 50
#define MAX_PASSWORD_LENGTH 50

int login(char *username, char *password)
{
    printf("Enter username: ");
    scanf("%s", username);
    if (strlen(username) == 0)
    {
        return -1;
    }

    printf("Enter password: ");
    scanf("%s", password);
    if (strlen(password) == 0)
    {
        return -1;
    }
    return 0;
}

int changePassword(char *new_password)
{
    printf("Enter new password (or 'bye' to sign out): ");
    scanf("%s", new_password);
    if (strlen(new_password) == 0)
    {
        return -1;
    }
    return 0;
}

int changeClientFlag(char *username, char *password, char *buff, int client_flag)
{
    snprintf(buff, BUFF_SIZE, "'%s %s' $%d", username, password, client_flag);
    printf("%s\n", buff);
    return 0;
}

int getInfo(char *buff, int *server_flag)
{
    char response[BUFF_SIZE];
    int items_matched = sscanf(buff, "'%[^']' $%d", response, server_flag);

    if (items_matched != 2)
    {
        return -1;
    }
    strcpy(buff, response);
    return 0;
}

void decodePassword(char *buff)
{
    int length = strlen(buff);
    for (int i = length; i >= 0; i--)
    {
        buff[i + 1] = buff[i];
    }
    buff[0] = '\n';
    for (int i = 0; i < length; i++)
    {
        if (buff[i] == ' ')
        {
            buff[i] = '\n';
        }
    }
}

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        printf("Usage: %s <IP address> <Port number>\n", argv[0]);
        return 1;
    }

    char *server_ip = argv[1];
    int port = atoi(argv[2]);

    char username[MAX_USERNAME_LENGTH];
    char password[MAX_PASSWORD_LENGTH];
    char new_password[MAX_PASSWORD_LENGTH];

    int client_socket, rcvBytes, sendBytes;
    char buff[BUFF_SIZE + 1];
    struct sockaddr_in server_address;

    int client_flag = 0;
    int server_flag;

    if ((client_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("Error: ");
        return 0;
    }

    bzero(&server_address, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = inet_addr(server_ip);
    server_address.sin_port = htons(port);

    if (connect(client_socket, (struct sockaddr *)&server_address, sizeof(server_address)) == -1)
    {
        perror("Connect error: ");
        return 1;
    }

    while (1)
    {
        if (client_flag == 1 && changePassword(new_password) == -1)
        {
            return 0;
        }

        if (client_flag == 0 && login(username, password) == -1)
        {
            return 0;
        }

        changeClientFlag(username, (client_flag == 1) ? new_password : password, buff, client_flag);

        sendBytes = send(client_socket, buff, sizeof(buff), 0);
        if (sendBytes < 0)
        {
            perror("Error: ");
            return 0;
        }

        rcvBytes = recv(client_socket, buff, sizeof(buff), 0);
        if (rcvBytes < 0)
        {
            perror("Error: ");
            return 0;
        }

        if (rcvBytes == 0)
        {
            printf("Lost connection from server.\n");
            return 0;
        }

        buff[rcvBytes] = '\0';

        getInfo(buff, &server_flag);

        if (server_flag == 0) // login successfully
        {
            client_flag = 1;
        }
        else if (server_flag == 2) // change password
        {
            decodePassword(buff);
        }
        else if (server_flag == 4) // signed out
        {
            client_flag = 0;
        }

        printf("\nReply from server: %s\n\n", buff);
    }
    return 0;
}
