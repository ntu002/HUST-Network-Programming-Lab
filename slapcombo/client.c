#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#define PORT 8080

int main() {
    int sock = 0, valread;
    struct sockaddr_in serv_addr;
    char *message = "Paddle Position: 100";
    char buffer[1024] = {0};

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n Socket creation error \n");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // Chuyển đổi địa chỉ IPv4 và IPv6 từ text sang binary
    if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("\nConnection Failed \n");
        return -1;
    }

    while(1) {
        // Gửi thông tin tới server
        send(sock, message, strlen(message), 0);
        printf("Message sent\n");

        // Đọc phản hồi từ server
        valread = read(sock, buffer, 1024);
        printf("Server: %s\n", buffer);

        // Xử lý dữ liệu nhận được ở đây
        // Ví dụ: cập nhật vị trí thanh trượt dựa trên thông tin từ server

        sleep(1); // Giả sử gửi thông tin mỗi giây
    }

    return 0;
}
