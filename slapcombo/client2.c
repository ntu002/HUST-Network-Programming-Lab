#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080

int main() {
    struct sockaddr_in serv_addr;
    int sock = 0;
    char *hello = "Hello from client 2";
    char buffer[1024] = {0};

    // Tạo socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n Socket creation error \n");
        return -1;
    }
  
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
  
    // Chuyển đổi địa chỉ IP
    if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0) {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }
  
    // Kết nối tới server
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("\nConnection Failed \n");
        return -1;
    }

    // Gửi và nhận dữ liệu
    send(sock, hello, strlen(hello), 0);
    printf("Hello message sent\n");
    read(sock, buffer, 1024);
    printf("Message from server: %s\n", buffer);

    close(sock);
    return 0;
}
