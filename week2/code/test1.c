#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <arpa/inet.h>

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: ./resolver parameter1 parameter2\n");
        return 1;
    }

    int param1 = atoi(argv[1]);
    char *param2 = argv[2];

    if (param1 == 1) {
        struct hostent *he;
        struct in_addr ipv4addr;

        if (inet_pton(AF_INET, param2, &ipv4addr) == 1) {
            he = gethostbyaddr(&ipv4addr, sizeof(ipv4addr), AF_INET);
            if (he == NULL) {
                printf("Official hostname: Not found information\n");
                return 0;
            }

            printf("Official hostname: %s\n", he->h_name);

            printf("Alias list:\n");
            for (int i = 0; he->h_aliases[i]; i++) {
                printf("%s\n", he->h_aliases[i]);
            }
        } else {
            printf("Wrong parameter\n");
        }
    } else if (param1 == 2) {
        struct hostent *he = gethostbyname(param2);

        if (he == NULL) {
            printf("Official IP: Not found information\n");
            return 0;
        }

        printf("Official IP: %s\n", inet_ntoa(*(struct in_addr *)he->h_addr));

        printf("IP list:\n");
        for (int i = 0; he->h_aliases[i]; i++) {
            printf("%s\n", inet_ntoa(*(struct in_addr *)he->h_aliases[i]));
        }
    } else {
        printf("Wrong parameter\n");
    }

    return 0;
}
