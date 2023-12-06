#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#define MAX_BUFF_SIZE 255

int isValidIpAddress(char *ipAddress);
int login(char *username, char *password);
int changePassword(char *new_password);
int changeClientFlag(char *username, char *password, char *buffer, int client_flag);
int getInfo(char *buffer, int *server_flag);
void decodePassword(char *buffer);

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        printf("Usage: %s <IP address> <Port number>\n", argv[0]);
        return 1;
    }

    // check if input id is valid
    if (!isValidIpAddress(argv[1]))
    {
        printf("%s: Not a valid ip address!\n", argv[1]);
        exit(EXIT_FAILURE);
    }

    int client_socket;
    char buffer[MAX_BUFF_SIZE];
    struct sockaddr_in server_addr;

    int client_flag = 0;
    int server_flag;

    // Step 1: Construct socket
    if ((client_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("Create TCP socket failed!\n");
        exit(EXIT_FAILURE);
    }

    char *server_ip = argv[1];
    int port = atoi(argv[2]);
    // Step 2: Specify server address
    memset(&server_addr, 0, sizeof(server_addr));

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(server_ip);
    server_addr.sin_port = htons(port);

    // Step 3: Request to connect server
    if (connect(client_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)
    {
        printf("\nError! Can not connect to sever!\n");
        exit(EXIT_FAILURE);
    }

    char username[MAX_BUFF_SIZE];
    char password[MAX_BUFF_SIZE];
    char new_password[MAX_BUFF_SIZE];
    int n;

    // Step 4: Communicate with server
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

        if (client_flag == 1)
        {
            changeClientFlag(username, new_password, buffer, client_flag);
        }
        else
        {
            changeClientFlag(username, password, buffer, client_flag);
        }

        n = send(client_socket, buffer, sizeof(buffer), 0);
        if (n < 0)
        {
            printf("Error!Cannot send data from sever!\n");
            exit(EXIT_FAILURE);
        }

        n = recv(client_socket, buffer, sizeof(buffer), 0);
        if (n < 0)
        {
            printf("Error!Cannot send data from sever!\n");
            exit(EXIT_FAILURE);
        }

        if (n == 0)
        {
            printf("Lost connection from server.\n");
            return 0;
        }

        buffer[n] = '\0';

        getInfo(buffer, &server_flag);

        // login successfully
        if (server_flag == 0) 
        {
            client_flag = 1;
        }
        else // change password
         if (server_flag == 2) 
        {
            decodePassword(buffer);
        }
        else // signed out
        if (server_flag == 4) 
        {
            client_flag = 0;
        }

        printf("\n[+]Reply from server: %s\n\n", buffer);
        if (strstr(buffer, "Goodbye") != NULL)
            break;
    }
    
    close(client_socket);
    return 0;
}

int isValidIpAddress(char *ipAddress)
{
    struct sockaddr_in sa;
    int result = inet_pton(AF_INET, ipAddress, &(sa.sin_addr));
    if (result != 0)
        return 1;
    else
        return 0;
}

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

int changeClientFlag(char *username, char *password, char *buffer, int client_flag)
{
    snprintf(buffer, MAX_BUFF_SIZE, "'%s %s' $%d", username, password, client_flag);
    // printf("%s\n", buff);
    return 0;
}

int getInfo(char *buffer, int *server_flag)
{
    char ack[MAX_BUFF_SIZE];
    int items_matched = sscanf(buffer, "'%[^']' $%d", ack, server_flag);

    if (items_matched != 2)
    {
        return -1;
    }
    strcpy(buffer, ack);
    return 0;
}

void decodePassword(char *buffer)
{
    int length = strlen(buffer);
    for (int i = length; i >= 0; i--)
    {
        buffer[i + 1] = buffer[i];
    }
    buffer[0] = '\n';
    for (int i = 0; i < length; i++)
    {
        if (buffer[i] == ' ')
        {
            buffer[i] = '\n';
        }
    }
}
