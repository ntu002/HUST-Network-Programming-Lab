#include <stdio.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdlib.h>

int hostnameToIp(char *param) {
    struct hostent *he;
    struct in_addr **addr_list;

    if ((he = gethostbyname(param)) == NULL) {
        herror ("Not found information");
        return 1;
    }

    //printf("Official IP: %s\n", he->h_name);
    addr_list = (struct in_addr **) he->h_addr_list;

    printf("Official IP: %s\n", inet_ntoa(*addr_list[0]));
    printf("Alias IP: ");

    for(int i = 1; addr_list[i] != NULL; ++i) {
        printf("%s ", inet_ntoa(*addr_list[i]));
    } 

    printf("\n");
    return 0;
}

int ipToHostname(char *param, struct in_addr ip) {
    struct hostent *hp;
    if ((hp = gethostbyaddr((const void *)&ip, sizeof ip, AF_INET)) == NULL) {
        herror("Not found infomation");
        return 1;
    }

    //printf("IP: %s \n", param);
    printf("Official name: %s \n", hp->h_name);
    printf("Alias name: \n");
    for (int i = 0; hp-> h_aliases[i] != NULL; i++) {
        printf("%s\n", hp->h_aliases[i]);
    }

    return 0;
}

struct in_addr ip;

int main(int argc, char *argv[]) {

    if (argc != 3) {
        printf("Error!!! \n");
        return 1;
    }

    int param1 = atoi(argv[1]);


    if (param1 == 1) {
        if (inet_pton(AF_INET, argv[2], &ip) == 1) {
            return ipToHostname(argv[2], ip);
        } else printf("Wrong parameter \n");
    } else {
        if (param1 == 2) {
            if (!inet_pton(AF_INET, argv[2], &ip)) {
                return hostnameToIp(argv[2]);
            } else printf("Wrong parameter \n");
        } else printf("Wrong parameter \n");
    }
    
    return 0;
}