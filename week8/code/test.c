#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <netdb.h>

void separate_Password(char *buffer)
{
  int length = strlen(buffer);
    for (int i = length; i >= 0; i--)
    {
        buffer[i + 1] = buffer[i];
    }
    buffer[0] = '\n';
    for (int i = 0; i < length; i++)
    {
        if (buffer[i] == ' ')
        {
            buffer[i] = '\n';
        }
    }
}

int getInfo(char *buffer, char *username, char *password, int *client_flag)
{
    char response[100000];
    int items_matched = sscanf(buffer, "'%[^']' $%d", response, client_flag);

    if (items_matched != 2)
    {
        return -1;
    }
    // printf("%i", items_matched);
    sscanf(response, "%s %s", username, password);
    return 0;
}

int main() {
    char s[100];
    //scanf("%s", s);
    strcpy(s, "'nguyen ngoc' $0");
    printf("%s", s);
    char u1[100], u2[100];
    int flag;

    printf("%d\n", getInfo(s,u1,u2,&flag));
    printf("%s - %s ", u1, u2);
    return 0;
}