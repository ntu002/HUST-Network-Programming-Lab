#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <stdbool.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>

const int SizeMax = 1024;

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        printf("Usage: %s IPAddress PortNumber\n", argv[0]);
        return 1;
    }

    // Implement TDP sockets
    int client_socket;
    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket < 0)
    {
        perror("Error in TCP socket creation");
        exit(EXIT_FAILURE);
    }

    printf("[+] Client socket created successfully. \n");

    // TCP Server that sends data back to client C
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));

    server_addr.sin_family = AF_INET;                 // Ipv4
    server_addr.sin_addr.s_addr = inet_addr(argv[1]); // Address
    server_addr.sin_port = htons(atoi(argv[2]));      // PORT

    // Connect to server
    if (connect(client_socket, (const struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        printf("Connect failed\n");
        exit(EXIT_FAILURE);
    }

    char account[SizeMax], password[SizeMax], new_password[SizeMax];
    socklen_t n, len;

    printf("\nPlease Insert account, password and new password: \n");


    memset(account, '\0', (strlen(account) + 1));
    memset(password, '\0', (strlen(password) + 1));

    printf("Enter account: ");
    fflush(stdin);
    scanf("%s", account);
    account[strcspn(account, "\n")] = 0;
    // if (account[0] == '\n')
    //     break;

    n = send(client_socket, (const char *)account, sizeof(account), 0);
    if (n < 0)
    {
        printf("[+] Client send failed\n");
        exit(EXIT_FAILURE);
    }

    char response1[SizeMax];
    n = recv(client_socket, (char *)response1, sizeof(response1), 0);
    if (n < 0)
    {
        printf("[+] Client receive failed\n");
        exit(EXIT_FAILURE);
    }
    response1[n] = '\0';
    printf("%s\n", response1);

    while (strcmp(response1, "Account not found") == 0)
    {
        printf("\nPlease re-enter username: ");
        fflush(stdin);
        scanf("%s", account);
        // Repeat the same process above to send to server and receive response
        account[strcspn(account, "\n")] = 0;

        n = send(client_socket, (const char *)account, sizeof(account), 0);
        if (n < 0)
        {
            printf("[+]Client send failed\n");
            exit(EXIT_FAILURE);
        }

        char response1[SizeMax];
        n = recv(client_socket, (char *)response1, sizeof(response1), 0);
        if (n < 0)
        {
            printf("[+]Client receive failed\n");
            exit(EXIT_FAILURE);
        }
        response1[n] = '\0';
        printf("%s\n", response1);
    }

    printf("Enter password: ");
    fflush(stdin);
    scanf("%s", password);

    n = send(client_socket, (const char *)password, sizeof(password), 0);
    if (n < 0)
    {
        printf("[+] Client send failed\n");
        exit(EXIT_FAILURE);
    }

    char response2[SizeMax];
    n = recv(client_socket, (char *)response2, sizeof(response2), 0);
    if (n < 0)
    {
        printf("[+] Client receive failed\n");
        exit(EXIT_FAILURE);
    }
    response2[n] = '\0';
    printf("%s\n", response2);

    while (strcmp(response2, "Not OK") == 0)
    {
        printf("\nPlease re-enter password (accout: %s): ", account);
        fflush(stdin);
        scanf("%s", password);

        n = send(client_socket, (const char *)password, sizeof(password), 0);
        if (n < 0)
        {
            printf("[+] Client send failed\n");
            exit(EXIT_FAILURE);
        }

        char response2[SizeMax];
        n = recv(client_socket, (char *)response2, sizeof(response2), 0);
        if (n < 0)
        {
            printf("[+]Client receive failed\n");
            exit(EXIT_FAILURE);
        }
        response2[n] = '\0';
        printf("%s\n\n", response2);
    }

    if (strcmp(response2, "OK") == 0)
    {
        printf("Login successful.\n");
        while (1)
        {
            printf("Enter new password (or 'bye' to sign out): ");
            fflush(stdin);
            scanf("%s", new_password);

            n = send(client_socket, (const char *)new_password, sizeof(new_password), 0);
            if (n < 0)
            {
                printf("[+]Client send failed\n");
                exit(EXIT_FAILURE);
            }

            char response3[SizeMax];
            n = recv(client_socket, (char *)response3, sizeof(response3), 0);
            if (n < 0)
            {
                printf("[+]Client receive failed\n");
                exit(EXIT_FAILURE);
            }
            response3[n] = '\0';

            printf("[+] Server reply \n%s\n", response3);

            if (strcmp(new_password, "bye") == 0)
            {
                printf("Signed out.\n");
                break;
            }
        }
    }

    close(client_socket);
}