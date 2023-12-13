#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define MAX_BUFF_SIZE 255
#define MAX_HEADER 20

char clientHeader[3][MAX_HEADER] = {"LOGIN", "MESSAGE", "LOGOUT"};
char serverHeader[2][MAX_HEADER] = {"SUCCESS", "ERROR"};

int logoutStatus = 0;

typedef struct __attribute__((packed))
{
    char msg_type[MAX_HEADER];
    char data_type[MAX_BUFF_SIZE];
} Input;
Input input;

typedef struct __attribute__((packed))
{
    char msg_type[MAX_HEADER];
    char data_type[MAX_BUFF_SIZE];
} Output;
Output output;

int sockfd;

int handleAction();
int handleSendData(Input *input);
int handleRecvData(Output *output);

int main(int argc, char **argv)
{

    if (argc != 3)
    {
        printf("Usage: %s <IP address> <Port number>\n", argv[0]);
        return 1;
    }

    int rcvBytes, sendBytes;
    socklen_t len;
    struct sockaddr_in server_addr, client_addr;

    // Step 1: Construct socket
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
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
    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)
    {
        printf("\nError! Can not connect to sever!\n");
        exit(EXIT_FAILURE);
    }

    // Step 4: Communicate with server
    while (1)
    {
        if (handleAction() == 1)
        {
            printf("\nInvalid choice! Please try again\n");
            continue;
        }
        sendBytes = handleSendData(&input);
        strcpy(input.data_type, "");
        strcpy(input.msg_type, "");
        if (sendBytes < 0)
        {
            printf("Error!Cannot send data from sever!\n");
            exit(EXIT_FAILURE);
        }
        rcvBytes = handleRecvData(&output);
        if (rcvBytes < 0)
        {
            printf("Error!Cannot receive data from sever!\n");
            exit(EXIT_FAILURE);
        }
        printf("\n[%s:%d] %s - %s\n\n", inet_ntoa(server_addr.sin_addr), ntohs(server_addr.sin_port), output.msg_type, output.data_type);
        strcpy(output.data_type, "");
        strcpy(output.msg_type, "");
        if (logoutStatus == 1)
        {
            break;
        }
    }
    close(sockfd);
    return 0;
}


int handleAction()
{
    printf("Choose action:\n");
    printf("----------------------------\n");
    printf("1. Sign in\n");
    printf("2. Send message\n");
    printf("3. Exit\n");
    printf("-----------------------------\n");
    printf("Enter option: ");
    int choice;
    scanf("%d", &choice);
    fflush(stdin);
    switch (choice)
    {
    case 1:
        printf("Enter username: ");
        fgets(input.data_type, sizeof(input.data_type), stdin);
        fflush(stdin);
        input.data_type[strlen(input.data_type) - 1] = '\0';
        strcpy(input.msg_type, clientHeader[0]);
        break;
    case 2:
        printf("Enter message: ");
        fgets(input.data_type, sizeof(input), stdin);
        fflush(stdin);
        input.data_type[strlen(input.data_type) - 1] = '\0';
        strcpy(input.msg_type, clientHeader[1]);
        break;
    case 3:
        printf("Exit...\n");
        strcpy(input.data_type, "Logout request");
        strcpy(input.msg_type, clientHeader[2]);
        logoutStatus = 1;
        break;
    default:
        return 1;
    }
    return 0;
}

int handleSendData(Input *input)
{
    return send(sockfd, input, sizeof(Input), 0);
}

int handleRecvData(Output *output)
{
    return recv(sockfd, output, sizeof(Output), 0);
}

