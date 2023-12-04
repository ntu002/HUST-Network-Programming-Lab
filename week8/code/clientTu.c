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

int isValidIpAddress(char *ipAddress)
{
    struct sockaddr_in sa;
    int result = inet_pton(AF_INET, ipAddress, &(sa.sin_addr));
    if (result != 0)
        return 1;
    else
        return 0;
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

int containsSpace(char *str)
{
    int length = strlen(str);

    for (int i = 0; i < length; i++)
    {
        if (str[i] == ' ')
        {
            return 1; // Chuỗi có chứa dấu cách
        }
    }

    return 0; // Chuỗi không chứa dấu cách
}

int checkSpace(char str[])
{
    size_t l = strlen(str);
    for (int i = 0; i < l; i++)
    {
        if (str[i] == ' ')
        {
            return 1;
        }
    }
    return 0;
}

int login(char *username, char *password)
{
    printf("Enter username: ");
    fgets(username, sizeof(username), stdin);

    size_t l_username = strlen(username);
    if (l_username == 0)
    {
        return -1;
    }
    if (l_username > 0 && username[l_username - 1] == '\n')
    {
        username[l_username - 1] = '\0';
    }

    while (checkSpace(username))
    {
        printf("Username is not valid! \n");
        printf("Please re-enter username: ");
        fgets(username, sizeof(username), stdin);
        size_t l = strlen(username);
        if (l > 0 && username[l - 1] == '\n')
        {
            username[l - 1] = '\0';
        }
    }

    printf("Enter password: ");
    fgets(password, sizeof(password), stdin);

    size_t l_pass = strlen(password);
    if (l_pass == 0)
    {
        return -1;
    }
    if (l_pass > 0 && password[l_pass - 1] == '\n')
    {
        password[l_pass - 1] = '\0';
    }

    while (checkSpace(password))
    {
        printf("Password is not valid! \n");
        printf("Please re-enter password: ");
        fgets(password, sizeof(password), stdin);
        size_t l = strlen(password);
        if (l > 0 && password[l - 1] == '\n')
        {
            password[l - 1] = '\0';
        }
    }

    return 0;
}

int changePassword(char *new_pass)
{
    printf("Enter new password (or 'bye' to sign out): ");
    scanf("%s", new_pass);
    if (strlen(new_pass) == 0)
    {
        return -1;
    }
    return 0;
}

int separate(char *buffer, int *server_flag)
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

int changeFlagClient(char *username, char *password, char *buffer, int client_flag)
{
    snprintf(buffer, MAX_BUFF_SIZE, "'%s %s' $%d", username, password, client_flag);
    //printf("%s\n", buffer);
    return 0;
}

void separate_Password(char *buffer)
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

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        printf("Usage: ./server <IPAdress> <PortNumber> \n");
        exit(EXIT_FAILURE);
    }

    // check if input id is valid
    if (!isValidIpAddress(argv[1]))
    {
        printf("%s: Not a valid ip address!\n", argv[1]);
        exit(EXIT_FAILURE);
    }

    // if (!isValidPort(argv[2]))
    // {
    //     printf("%s: Not a valid port number!\n", argv[2]);
    //     exit(EXIT_FAILURE);
    // }

    int sockfd;
    struct sockaddr_in server_addr;

    // Step 1: Construct socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        printf("Create TCP socket failed!\n");
        exit(EXIT_FAILURE);
    }

    // Step 2: Specify server address
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(atoi(argv[2]));
    server_addr.sin_addr.s_addr = inet_addr(argv[1]);

    // Step 3: Request to connect server
    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(struct sockaddr)) < 0)
    {
        printf("\nError! Can not connect to sever!\n");
        exit(EXIT_FAILURE);
    }

    char username[MAX_BUFF_SIZE];
    char password[MAX_BUFF_SIZE];
    char new_password[MAX_BUFF_SIZE];
    char buffer[MAX_BUFF_SIZE];
    int client_flag = 0;
    int server_flag;
    socklen_t n, len;

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

        // Buffer: username + new password + flag 
        if (client_flag == 1) {
            changeFlagClient(username, new_password, buffer, client_flag);
        } else {
            // Buffer: username + password + flag 
            changeFlagClient(username, password, buffer, client_flag);
        }

        printf("Buffer: %s", buffer);

        n = send(sockfd, buffer, sizeof(buffer), 0);
        if (n < 0)
        {
            printf("Error!Cannot send data from sever!\n");
            exit(EXIT_FAILURE);
        }

        n = recv(n, buffer, sizeof(buffer), 0);
        if (n < 0)
        {
            printf("Error!Cannot send data from sever!\n");
            exit(EXIT_FAILURE);
        }

        if (n == 0)
        {
            printf("Lost connection from server.\n");
            exit(EXIT_FAILURE);
        }

        buffer[n] = '\0';

        separate(buffer, &server_flag);

        if (server_flag == 0) // login successfully
        {
            client_flag = 1;
        }
        else if (server_flag == 2) // change password
        {
            separate_Password(buffer);
        }
        else if (server_flag == 4) // signed out
        {
            client_flag = 0;
        }

        printf("\nReply from server: %s\n\n", buffer);
    }

    if (!strcmp(buffer, "OK"))
    {
        printf("Logged in successfully!\n");
    }

    // Step 4: Close socket
    close(sockfd);
    return 0;
}