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
    }
}