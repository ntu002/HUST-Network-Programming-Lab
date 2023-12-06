int getInfo(char *buffer, char *username, char *password, int *client_flag);
int assignFlag(char *buffer, int *server_flag);
void separate(char *newpass);
bool passwordFormat(char *password);
void login(const char *filename, char *username, char *password, char *response, char *last_username_signed_in, int *signin_err_flag, int *server_flag);
void changePassword(const char *filename, char *username, char *newpass, char *response, int *server_flag);

int handle_client(int socket_desc, int rcvBytes)
{
    int client_sock = socket_desc;

    // rcvBytes = recv(client_sock, buff, sizeof(buff), 0);

    if (rcvBytes < 0)
    {
        perror("Error: ");
        return 0;
    }
    if (rcvBytes == 0)
    {
        printf("Lost connection from client.\n");
        return 0;
    }
    buffer[rcvBytes] = '\0';

    getInfo(buffer, username, password, &client_flag);
    printf("[+]Send from client: %s - %s\n", username, password);

    if (client_flag == 0)
    {
        login(filename, username, password, buffer, last_username_signed_in, &sign_in_error_flag, &server_flag);
    }
    else
    {
        changePassword(filename, username, password, buffer, &server_flag);
    }

    assignFlag(buffer, &server_flag);

    sendBytes = send(client_sock, buffer, sizeof(buffer), 0);
    printf("[+]Server replied: %s\n\n", buffer);
    if (sendBytes < 0)
    {
        perror("Error: ");
        return 0;
    }

    return 0;
}

int getInfo(char *buffer, char *username, char *password, int *client_flag)
{
    char ack[MAX_BUFF_SIZE];
    int items_matched = sscanf(buffer, "'%[^']' $%d", ack, client_flag);

    if (items_matched != 2)
    {
        return -1;
    }
    // printf("%i", items_matched);
    sscanf(ack, "%s %s", username, password);
    return 0;
}

int assignFlag(char *buffer, int *server_flag)
{
    char ack[MAX_BUFF_SIZE];
    snprintf(ack, MAX_BUFF_SIZE, "'%s' $%d", buffer, *server_flag);
    strcpy(buffer, ack);
    return 0;
}

void separate(char *newpass)
{
    int l1 = 0;
    int l2 = 0;
    char word_result[MAX_BUFF_SIZE] = "";
    char number_result[MAX_BUFF_SIZE] = "";
    for (int i = 0; buffer[i] != '\0'; ++i)
    {
        if (isalpha(buffer[i]))
        {
            word_result[l1] = buffer[i];
            l1++;
        }
        else if (isdigit(buffer[i]))
        {
            number_result[l2] = buffer[i];
            l2++;
        }
        else if (!isspace(buffer[i]))
        {
            printf("Error \n");
        }
    }
    word_result[l1] = '\0';
    number_result[l2] = '\0';

    strcpy(newpass, strcat(strcat(word_result, " "), number_result));
    return;
}

bool passwordFormat(char *password)
{
    int length = strlen(password);
    for (int i = 0; i < length; i++)
    {
        if (!((password[i] >= 'A' && password[i] <= 'Z') ||
              (password[i] >= 'a' && password[i] <= 'z') ||
              (password[i] >= '0' && password[i] <= '9')))
        {
            return false;
        }
    }
    return true;
}

void login(const char *filename, char *username, char *password, char *response, char *last_username_signed_in, int *signin_err_flag, int *server_flag)
{
    int max_attempt = 3;
    *server_flag = 1;
    Tree_T user;
    user = SearchUsername(username, T);
    if (user == NULL)
    {
        strcpy(response, "Account does not exist!");
        return;
    }
    if (user->data.status == 0)
    {
        strcpy(response, "Account is blocked!");
        return;
    }
    if (strcmp(user->data.password, password) == 0)
    {
        if (user->data.status == 1)
        {
            strcpy(response, "OK");
            *server_flag = 0;
            return;
        }
        else if (user->data.status == 0)
        {
            strcpy(response, "Account is blocked!");
            return;
        }
        else
        {
            strcpy(response, "Account is not ready!");
            return;
        }
    }
    else
    {
        *signin_err_flag += 1;
        sprintf(response, "(Not Ok): %d/%i tried attempted!", *signin_err_flag, max_attempt);
        if (strcmp(last_username_signed_in, username) == 0)
        {
            if (*signin_err_flag >= max_attempt)
            {
                strcpy(response, "Account is blocked!");
                user->data.status = 0;
                //writeToFile(userList, filename);
            }
        }
        else
        {
            strcpy(last_username_signed_in, username);
            *signin_err_flag = 1;
        }
        return;
    }
    return;
}

void changePassword(const char *filename, char *username, char *newpass, char *response, int *server_flag)
{
    *server_flag = 3;

    if (strcmp(newpass, "bye") == 0)
    {
        strcpy(response, "Goodbye");
        strcat(response, username);
        *server_flag = 4;
        return;
    }

    Tree_T acc;
    acc = SearchUsername(username, T);
    if (acc == NULL)
    {
        strcpy(response, "Account does not exist");
        return;
    }

    if (!passwordFormat(newpass))
    {
        strcpy(response, "Error: Wrong password format - letters and numbers only!");
        return;
    }
    else if (strcmp(acc->data.password, newpass) == 0)
    {
        strcpy(response, "Error: New password must be different from current password!");
        return;
    }
    else
    {
        *server_flag = 2;
        strcpy(acc->data.password, newpass);
        //writeToFile(userList, filename);
        separate(newpass);
        strcpy(response, newpass);
        return;
    }
    return;
}
