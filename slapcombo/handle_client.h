void *handle_client(void *sockfd)
{
    socklen_t n;
    char buffer[MAX_BUFF_SIZE];
    int clientfd = *(int *)sockfd;

    pthread_mutex_lock(&mutex);
    if (num_client < MAX_CLIENT)
    {
        num_client++;
    }
    else
    {
        char err_msg[] = "Server is full. Try again later. \n";
        send(clientfd, err_msg, MAX_BUFF_SIZE, 0);
        close(clientfd);
    }
    pthread_mutex_unlock(&mutex);

    while (1)
    {
        /*
        Sign in
        */
        // Receive the username from client
        n = recv(clientfd, (char *)buffer, MAX_BUFF_SIZE, 0);
        buffer[n] = '\0';
        char username[MAX_BUFF_SIZE];
        strcpy(username, buffer);

        printf("Buffer username: %s\n", buffer);

        Tree_T c;
        c = SearchUsername(username, T);
        if (c != NULL)
        {

            // Receive the password
            n = recv(clientfd, (char *)buffer, MAX_BUFF_SIZE, 0);
            buffer[n] = '\0';
            char password[MAX_BUFF_SIZE];
            printf("Buffer password: %s\n", buffer);
            strcpy(password, buffer);

            if (strcmp(c->data.password, password) == 0)
            {
                char *ack = "OK";
                n = send(clientfd, (const char *)ack, MAX_BUFF_SIZE, 0);
                if (n < 0)
                {
                    printf("Server send failed\n");
                    exit(EXIT_FAILURE);
                }
            }
        }

        /*
        Lựa chọn cách chơi:
        Nhập message: chơi với người - chơi với máy
        chơi với máy: 1
        chơi với người: 2
        */

        // Receive the choose how to play from client
        n = recv(clientfd, (char *)buffer, MAX_BUFF_SIZE, 0);
        buffer[n] = '\0';
        char chooseGame[MAX_BUFF_SIZE];
        strcpy(chooseGame, buffer);

        char cnt[MAX_BUFF_SIZE];
        printf("Buffer choose how to play: %s\n", buffer);
        if (strcmp(chooseGame, "1") == 0) {
            sprintf(cnt, "%d\n", c->data.blood);
                n = send(clientfd, (const char *)cnt, MAX_BUFF_SIZE, 0);
                if (n < 0)
                {
                    printf("Server send failed\n");
                    exit(EXIT_FAILURE);
                }

            sprintf(cnt, "%d\n", c->data.power);
                n = send(clientfd, (const char *)cnt, MAX_BUFF_SIZE, 0);
                if (n < 0)
                {
                    printf("Server send failed\n");
                    exit(EXIT_FAILURE);
                }
            
            sprintf(cnt, "%d\n", c->data.gold);
                n = send(clientfd, (const char *)cnt, MAX_BUFF_SIZE, 0);
                if (n < 0)
                {
                    printf("Server send failed\n");
                    exit(EXIT_FAILURE);
                }

        } else if (strcmp(chooseGame, "2") == 0) {
            
        }
    }
}