

void handle_client(int clientfd)
{
    socklen_t n;
    char buffer[MAX_BUFF_SIZE];
    int count = 0;

    while (1)
    {
        // Receive the username from client
        n = recv(clientfd, (char *)buffer, MAX_BUFF_SIZE, 0);
        if (n < 0)
        {
            printf("Server receive failed!\n");
            exit(EXIT_FAILURE);
        }
        buffer[n] = '\0';
        char username[MAX_BUFF_SIZE];
        strcpy(username, buffer);

        printf("Buffer username: %s\n", buffer);

        Tree_T c;
        c = SearchUsername(username, T);
        if (c == NULL)
        {
            char *ack = "Account not found";
            n = send(clientfd, (const char *)ack, MAX_BUFF_SIZE, 0);
            if (n < 0)
            {
                printf("Server send failed!\n");
                exit(EXIT_FAILURE);
            }
        }
        else
        {
            char *ack = "Insert password";
            n = send(clientfd, (const char *)ack, MAX_BUFF_SIZE, 0);
            if (n < 0)
            {
                printf("Server send failed!\n");
                exit(EXIT_FAILURE);
            }

            // Receive the password
            n = recv(clientfd, (char *)buffer, MAX_BUFF_SIZE, 0);
            if (n < 0)
            {
                printf("Receive failed\n");
                exit(EXIT_FAILURE);
            }
            buffer[n] = '\0';
            char password[MAX_BUFF_SIZE];
            printf("Buffer password: %s\n", buffer);
            strcpy(password, buffer);
            // printf("Invalid count: %d\n", invalid_login_count);

            // If the password is not matched
            while (strcmp(c->data.password, password) != 0)
            {
                count++;
                printf("count: %d\n", count);
                if (count == 3)
                    break;

                char *ack = "Incorrect password";
                n = send(clientfd, (const char *)ack, MAX_BUFF_SIZE, 0);
                if (n < 0)
                {
                    printf("Server send failed\n");
                    exit(EXIT_FAILURE);
                }
                n = recv(clientfd, (char *)buffer, MAX_BUFF_SIZE, 0);
                if (n < 0)
                {
                    printf("Receive failed\n");
                    exit(EXIT_FAILURE);
                }
                buffer[n] = '\0';
                char password[MAX_BUFF_SIZE];
                printf("Buffer password: %s\n", buffer);
                strcpy(password, buffer);
            }

            if (count == 3)
            {
                c->data.status = 0;
                // Send message
                char *ack = "Account is blocked";
                n = send(clientfd, (const char *)ack, MAX_BUFF_SIZE, 0);
                if (n < 0)
                {
                    printf("Server send failed\n");
                    exit(EXIT_FAILURE);
                }
                break;
            }
            else if (c->data.status == 1)
            {
                char *ack = "OK";
                n = send(clientfd, (const char *)ack, MAX_BUFF_SIZE, 0);
                if (n < 0)
                {
                    printf("Server send failed\n");
                    exit(EXIT_FAILURE);
                }
                break;
            }
            else
            {
                char ack2[MAX_BUFF_SIZE];
                if (c->data.status == 0)
                {
                    strcpy(ack2, "Account is blocked");
                }

                n = send(clientfd, (const char *)ack2, MAX_BUFF_SIZE, 0);
                if (n < 0)
                {
                    printf("Send failed\n");
                    exit(EXIT_FAILURE);
                }
                break;
            }
        }
    }
}
