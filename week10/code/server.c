#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <netdb.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <time.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>

#define MAX_BUFF_SIZE 255
#define MAX_HEADER 20

const int MAX = 100;
FILE *fptr, *fpMsg;
int n;
int signInStatus = 0;
char account[20];

char clientHeader[3][MAX_HEADER] = {"LOGIN", "MESSAGE", "LOGOUT"};
char serverHeader[2][MAX_HEADER] = {"SUCCESS", "ERROR"};

typedef struct input
{
    char username[MAX_BUFF_SIZE];
    char password[MAX_BUFF_SIZE];
    int status;
} Account;

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

typedef Account AccountList;

struct node_t
{
    AccountList element;
    struct node_t *next;
};

typedef struct node_t node;
node *root, *cur, *pre;
AccountList *a;

int step = 1;

// #include"GenSLL.h"

int sockfd, new_socket, rcvBytes, sendBytes;
socklen_t len;


int getMsgToFile();
//int checkAccountUsername();
int checkMsgType();
int handleRecvData(Input *input);
int handleSendData(Output *output);
void handleAction();

int main(int argc, char **argv)
{

    if (argc != 2)
    {
        printf("Wrong parameters!!! \n");
        return 1;
    }

    // Step 1: Construct socket
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("Opening stream socket \n");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in server_addr, client_addr;
    int port = atoi(argv[1]);

    memset(&server_addr, 0, sizeof(server_addr));
    memset(&client_addr, 0, sizeof(client_addr));

    // Initialize server address
    server_addr.sin_family = AF_INET; // IPv4
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(port);

    // Bind ther server socket
    if (bind(sockfd, (const struct sockaddr *)&server_addr,
             sizeof(server_addr)) < 0)
    {
        perror("Error in binding \n");
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(sockfd, 10) < 0)
    {
        perror("Listening failed \n");
        exit(EXIT_FAILURE);
    }
    else
        printf("[+]Server is running... Waiting for connections.\n");

    int clientfd = accept(sockfd, (struct sockaddr *)&client_addr, &len);
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

    // Communicate with clients
    while (1)
    {
        len = sizeof(client_addr);

        rcvBytes = handleRecvData(&input);
        if (rcvBytes < 0)
        {
            printf("Error!Cannot receive data from client!\n");
            exit(EXIT_FAILURE);
        }
        printf("%s - %s\n", input.msg_type, input.data_type);

        handleAction();
        strcpy(input.data_type, "");
        strcpy(input.msg_type, "");

        sendBytes = handleSendData(&output);
        if (sendBytes < 0)
        {
            printf("Error!Cannot send data from client!\n");
            exit(EXIT_FAILURE);
        }
        strcpy(output.data_type, "");
        strcpy(output.msg_type, "");
    }
    close(sockfd);
    return 0;
}


int getMsgToFile()
{

    time_t t; // not a primitive datatype
    time(&t);
    char *time = ctime(&t);
    time[strlen(time) - 1] = '\0';
    char msg[1025] = "";
    strcat(msg, "[");
    strcat(msg, time);
    strcat(msg, "] ");
    strcat(msg, input.data_type);
    char filename[20] = "message/";
    strcat(filename, account);
    strcat(filename, ".txt");
    fpMsg = fopen(filename, "a+");
    if (fpMsg == NULL)
    {
        printf("Cannot open file!\n");
        return 1;
    }
    fprintf(fpMsg, "%s\n", msg);
    fclose(fpMsg);
    return 0;
}

// int checkAccountUsername()
// {
//     if (checkUsername(input.data_type, root) == 0)
//     {
//         strcpy(output.data_type, "Login Success");
//         strcpy(output.msg_type, serverHeader[0]);
//         signInStatus = 1;
//         strcpy(account, input.data_type);
//     }
//     else
//     {
//         strcpy(output.data_type, "Account not exist!");
//         strcpy(output.msg_type, serverHeader[1]);
//     }
//     return 0;
// }

int checkMsgType()
{
    if (strcmp(input.msg_type, clientHeader[0]) == 0)
    {
        return 1;
    }
    else if (strcmp(input.msg_type, clientHeader[1]) == 0)
    {
        return 2;
    }
    else if (strcmp(input.msg_type, clientHeader[2]) == 0)
    {
        return 3;
    }
    return 0;
}

int handleRecvData(Input *input)
{
    return read(new_socket, input, sizeof(Input));
}

int handleSendData(Output *output)
{
    return send(new_socket, output, sizeof(Output), 0);
}

void handleAction()
{
    switch (checkMsgType())
    {
    case 1:
        signInStatus = 1;
        strcpy(output.data_type, "Logged in successfully!");
        strcpy(output.msg_type, serverHeader[1]);
        break;
    case 2:
        if (signInStatus == 1)
        {
            int res = getMsgToFile();
            if (res == 1)
            {
                strcpy(output.data_type, "Send message failed!");
                strcpy(output.msg_type, serverHeader[1]);
            }
            else
            {
                strcpy(output.data_type, "Send message success!");
                strcpy(output.msg_type, serverHeader[0]);
            }
        }
        else
        {
            strcpy(output.data_type, "You are not logged in!");
            strcpy(output.msg_type, serverHeader[1]);
        }
        break;
    case 3:
        strcpy(output.data_type, "Goodbye!");
        strcpy(output.msg_type, serverHeader[0]);
        signInStatus = 0;
        strcpy(account, "");
        printf("Logging out...\n");
        break;
    default:
        printf("[+]Server Error in Switch case!\n");
        strcpy(output.data_type, "Internal Server Error!");
        strcpy(output.msg_type, serverHeader[0]);
        break;
    }
}
