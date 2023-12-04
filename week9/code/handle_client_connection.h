int getInfo(char *buffer, char *username, char *password, int *client_flag);
int assignFlag(char *buff, int *server_flag);
void passwordSeparate(char *newpass);
bool passwordFormatCheck(char *password);
void login(LinkedList *userList, const char *filename,
           char *username, char *password, char *response,
           char *last_username_signed_in, int *sign_in_error_flag, int *server_flag);
void changePassword(LinkedList *userList, const char *filename,
                    char *username, char *newpass,
                    char *response, int *server_flag);


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
    printf("Client input: %s - %s\n", username, password);

    if (client_flag == 0)
    {
        login(userList, filename, username, password, buffer, last_username_signed_in, &sign_in_error_flag, &server_flag);
    }
    else
    {
        changePassword(userList, filename, username, password, buffer, &server_flag);
    }

    assignFlag(buffer, &server_flag);

    sendBytes = send(client_sock, buffer, sizeof(buffer), 0);
    printf("%s\n", buffer);
    if (sendBytes < 0)
    {
        perror("Error: ");
        return 0;
    }

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

bool passwordFormatCheck(char *password)
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

void login(LinkedList *userList, const char *filename,
           char *username, char *password, char *response,
           char *last_username_signed_in, int *sign_in_error_flag, int *server_flag)
{
    int max_attempt = 3;
    *server_flag = 1;
    Account *user = findAccount(userList, username);
    if (user == NULL)
    {
        strcpy(response, "Account does not exist!");
        return;
    }
    if (user->status == 0)
    {
        strcpy(response, "Account is blocked!");
        return;
    }
    if (strcmp(user->password, password) == 0)
    {
        if (user->status == 1)
        {
            strcpy(response, "OK");
            *server_flag = 0;
            return;
        }
        else if (user->status == 0)
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
        *sign_in_error_flag += 1;
        sprintf(response, "Wrong password, %d/%i tries attempted!", *sign_in_error_flag, max_attempt);
        if (strcmp(last_username_signed_in, username) == 0)
        {
            if (*sign_in_error_flag >= max_attempt)
            {
                strcpy(response, "Account is blocked!");
                user->status = 0;
                writeAccountsToFile(userList, filename);
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

void changePassword(LinkedList *userList, const char *filename,
                    char *username, char *newpass,
                    char *response, int *server_flag)
{
    *server_flag = 3;

    if (strcmp(newpass, "bye") == 0)
    {
        strcpy(response, "Goodbye ");
        strcat(response, username);
        *server_flag = 4;
        return;
    }

    Account *account = findAccount(userList, username);
    if (account == NULL)
    {
        strcpy(response, "Account does not exist");
        return;
    }

    if (!passwordFormatCheck(newpass))
    {
        strcpy(response, "Error: Wrong password format - letters and numbers only!");
        return;
    }
    else if (strcmp(account->password, newpass) == 0)
    {
        strcpy(response, "Error: New password must be different from current password!");
        return;
    }
    else
    {
        *server_flag = 2;
        strcpy(account->password, newpass);
        writeAccountsToFile(userList, filename);
        passwordSeparate(newpass);
        strcpy(response, newpass);
        return;
    }
    return;
}
