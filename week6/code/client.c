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

int containsSpace(char *str) {
    int length = strlen(str);
    
    for (int i = 0; i < length; i++) {
        if (str[i] == ' ') {
            return 1; // Chuỗi có chứa dấu cách
        }
    }
    
    return 0; // Chuỗi không chứa dấu cách
}


int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        printf("Usage: %s <IPAdress> <PortNumber> \n", argv[0]);
        return 1;
    }

    // check if input id is valid
    // if (!isValidIpAddress(argv[1]))
    // {
    //     printf("%s: Not a valid ip address!\n", argv[1]);
    //     return 1;
    // }

    // if (!isValidPort(argv[2]))
    // {
    //     printf("%s: Not a valid port number!\n", argv[2]);
    //     return 1;
    // }

    int sockfd;
    socklen_t n, len;
    struct sockaddr_in server_addr;

    // Step 1: Construct socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        printf("Create TCP socket failed!\n");
        exit(EXIT_FAILURE);
    }

    memset(&server_addr, 0, sizeof(server_addr));

    // Step 2: Specify server address
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
    char buffer[MAX_BUFF_SIZE];

    // Step 4: Communicate with server
    printf("Enter username: ");
    //fflush(stdin);
    scanf("%s", username);
    username[strcspn(username, "\n")] = 0;
    n = send(sockfd, (const char *)username, sizeof(username), 0);
    if (n < 0)
    {
        printf("Error!Cannot send data from sever!\n");
        exit(EXIT_FAILURE);
    }

    n = recv(sockfd, (char *)buffer, MAX_BUFF_SIZE, 0);
    if (n < 0)
    {
        printf("Error!Cannot send data from sever!\n");
        exit(EXIT_FAILURE);
    }
    buffer[n] = '\0';
    printf("Buffer: %s\n", buffer);

    while (!strcmp(buffer, "Account not found"))
    {
        printf("Please re-enter username: ");
        //fflush(stdin);
        scanf("%s", username);
        username[strcspn(username, "\n")] = 0;
        n = send(sockfd, (const char *)username, sizeof(username), 0);
        if (n < 0)
        {
            printf("Error!Cannot send data from sever!\n");
            exit(EXIT_FAILURE);
        }

        n = recv(sockfd, (char *)buffer, MAX_BUFF_SIZE, 0);
        if (n < 0)
        {
            printf("Error!Cannot send data from sever!\n");
            exit(EXIT_FAILURE);
        }
        buffer[n] = '\0';
        printf("Buffer: %s\n", buffer);
    }

    printf("Enter password: ");
    //fflush(stdin);
    scanf("%s", password);
    password[strcspn(password, "\n")] = 0;

    printf("password: %s\n", password);
    n = send(sockfd, (const char *)password, sizeof(password), 0);
    printf("n = %d\n", n);
    if (n < 0)
    {
        printf("Error!Cannot send data from sever!\n");
        exit(EXIT_FAILURE);
    }

    n = recv(sockfd, (char *)buffer, MAX_BUFF_SIZE, 0);
     buffer[n] = '\0';
    printf("Buffer: %s\n", buffer);
    printf("n = %d\n", n);
    
    if (n < 0)
    {
        printf("Error!Cannot send data from sever!\n");
        exit(EXIT_FAILURE);
    }
    buffer[n] = '\0';
    printf("Buffer: %s\n", buffer);

    while (!strcmp(buffer, "Incorrect password"))
    {
        printf("Please re-enter password: ");
        fflush(stdin);
        scanf("%s", password);
        password[strcspn(password, "\n")] = 0;
        n = send(sockfd, (const char *)password, sizeof(password), 0);
        if (n < 0)
        {
            printf("Error!Cannot send data from sever!\n");
            exit(EXIT_FAILURE);
        }

        n = recv(sockfd, (char *)buffer, MAX_BUFF_SIZE, 0);
        if (n < 0)
        {
            printf("Error!Cannot send data from sever!\n");
            exit(EXIT_FAILURE);
        }
        buffer[n] = '\0';
        printf("Buffer: %s\n", buffer);

        if (!strcmp(buffer, "Account is blocked")) {
            printf("Account is blockes!\n");
            break;
        }
    }

    if (!strcmp(buffer, "OK")) {
        printf("Logged in successfully!\n");
    }


    // Step 4: Close socket
    close(sockfd);
    return 0;
}