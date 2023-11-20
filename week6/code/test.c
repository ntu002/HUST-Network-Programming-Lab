#include <stdio.h>
#include <string.h>

int containsSpace(char *str) {
    int length = strlen(str);
    
    for (int i = 0; i < length; i++) {
        if (str[i] == ' ') {
            return 1; // Chuỗi có chứa dấu cách
        }
    }
    
    return 0; // Chuỗi không chứa dấu cách
}

int main() {
    char input[100];

    printf("Nhập vào một chuỗi: ");
    fgets(input, sizeof(input), stdin);

    // Xóa ký tự newline từ chuỗi nhập
    int length = strlen(input);
    if (input[length - 1] == '\n') {
        input[length - 1] = '\0';
    }
    if (containsSpace(input)) {
        printf("Chuỗi có chứa dấu cách.\n");
    } else {
        printf("Chuỗi không chứa dấu cách.\n");
    }

     printf("Input: %s \n", input);


    return 0;
}
