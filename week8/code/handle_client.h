char last_username_signed_in[MAX_BUFF_SIZE];
char username_signin[MAX_BUFF_SIZE];
char username[MAX_BUFF_SIZE], password[MAX_BUFF_SIZE];
int sign_in_err_flag = 0;
int client_flag, server_flag, n;
char ack[MAX_BUFF_SIZE];


int getInfo(char *buffer, char *username, char *password, int *client_flag);

int assignFlag(char *buff, int *server_flag);

void passwordSeparate(char *newpass);

int passwordFormatCheck(char *password);

void login(const char *filename,
           char *username, char *password, char *ack,
           char *last_username_signed_in, int *sign_in_error_flag, int *server_flag);

void changePassword(const char *filename,
                    char *username, char *newpass,
                    char *ack, int *server_flag);


int handle_client(int socket_desc)
{
    int client_sock = socket_desc;

    getInfo(buffer, username, password, &client_flag);
    printf("Client input: %s - %s\n", username, password);

    if (client_flag == 0)
    {
        login(file, username, password, buffer, last_username_signed_in, &sign_in_err_flag, &server_flag);
    }
    else
    {
        changePassword(file, username, password, buffer, &server_flag);
    }

    assignFlag(buffer, &server_flag);

    n = send(client_sock, buffer, sizeof(buffer), 0);
    if (n < 0)
    {
        perror("Error: ");
        return 0;
    }
    //writeAccountsToFile(userList, filename);

    return 0;
}


int getInfo(char *buffer, char *username, char *password, int *client_flag)
{
    char response[MAX_BUFF_SIZE];
    int items_matched = sscanf(buffer, "'%[^']' $%d", response, client_flag);

    if (items_matched != 2)
    {
        return -1;
    }
    // printf("%i", items_matched);
    sscanf(response, "%s %s", username, password);
    return 0;
}

int assignFlag(char *buff, int *server_flag)
{
    char response[MAX_BUFF_SIZE];
    snprintf(response, MAX_BUFF_SIZE, "'%s' $%d", buff, *server_flag);
    strcpy(buff, response);
    return 0;
}

void passwordSeparate(char *newpass)
{
    int length = strlen(newpass);
    char word_string[MAX_BUFF_SIZE] = "";
    char number_list[MAX_BUFF_SIZE] = "";
    int word_count = 0;
    int number_count = 0;

    for (int i = 0; i < length; i++)
    {
        if (newpass[i] >= 'A' && newpass[i] <= 'Z')
        {
            word_string[word_count] = newpass[i];
            word_count++;
        }
        else if (newpass[i] >= 'a' && newpass[i] <= 'z')
        {
            word_string[word_count] = newpass[i];
            word_count++;
        }
        else if (newpass[i] >= '0' && newpass[i] <= '9')
        {
            number_list[number_count] = newpass[i];
            number_count++;
        }
    }
    strcpy(newpass, strcat(strcat(word_string, " "), number_list));
    return;
}

int passwordFormatCheck(char *password)
{
    int length = strlen(password);
    for (int i = 0; i < length; i++)
    {
        if (!((password[i] >= 'A' && password[i] <= 'Z') ||
              (password[i] >= 'a' && password[i] <= 'z') ||
              (password[i] >= '0' && password[i] <= '9')))
        {
            return 0;
        }
    }
    return 1;
}

void login(const char *filename,
           char *username, char *password, char *ack,
           char *last_username_signed_in, int *sign_in_error_flag, int *server_flag)
{
    int max_attempt = 3;
    *server_flag = 1;
    Tree_T c;
    c = SearchUsername(username, T);
    if (c == NULL)
    {
        strcmp(ack, "Account not found");
        return;
    }
    if (c->data.status == 0)
    {
        strcmp(ack, "Account is blocked!");
        return;
    }
    if (strcmp(c->data.password, password) == 0)
    {
        if (c->data.status == 1)
        {
            strcmp(ack, "OK");
            *server_flag = 0;
            return;
        }
        else if (c->data.status == 0)
        {
            strcmp(ack, "Account is blocked!");
            return;
        }
        else
        {
            strcmp(ack, "Account is not ready!");
            return;
        }
    }
    else
    {
        *sign_in_error_flag += 1;
        sprintf(ack, "Wrong password, %d/%i tries attempted!", *sign_in_error_flag, max_attempt);
        if (strcmp(last_username_signed_in, username) == 0)
        {
            if (*sign_in_error_flag >= max_attempt)
            {
                strcpy(ack, "Account is blocked!");
                c->data.status = 0;
                //writeAccountsToFile(userList, filename);
            }
        }
        else
        {
            strcpy(last_username_signed_in, username);
            *sign_in_error_flag = 1;
        }
        return;
    }
    return;
}

void changePassword(const char *filename,
                    char *username, char *newpass,
                    char *ack, int *server_flag)
{
    *server_flag = 3;

    if (strcmp(newpass, "bye") == 0)
    {
        strcpy(ack, "Goodbye ");
        strcat(ack, username);
        *server_flag = 4;
        return;
    }

    Tree_T c = SearchUsername(username, T);
    if (c == NULL)
    {
        strcpy(ack, "Account does not exist");
        return;
    }

    if (!passwordFormatCheck(newpass))
    {
        strcpy(ack, "Error: Wrong password format - letters and numbers only!");
        return;
    }
    else if (strcmp(c->data.password, newpass) == 0)
    {
        strcpy(ack, "Error: New password must be different from current password!");
        return;
    }
    else
    {
        *server_flag = 2;
        strcpy(c->data.password, newpass);
        //writeAccountsToFile(T, filename);
        passwordSeparate(newpass);
        strcpy(ack, newpass);
        return;
    }
    return;
}


