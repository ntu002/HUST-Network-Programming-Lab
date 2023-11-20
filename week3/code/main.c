#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>


const int Max = 100;

typedef struct 
{
    char username[Max];
    char password[Max];
    int status;
    char code[Max];
    int count;
    int check;
    char homepage[Max];
    char domainName[Max];
    char ipAddress[Max];
} eType;

typedef struct nodeType{
    eType data;
    struct nodeType *left, *right;
} node_Type;

typedef node_Type* Tree_T;
Tree_T T;

eType *a;
int total;
#include "linklist.h"

FILE *file1, *file2;
int cnt = 0;

void readFile() {
    if ((file1 = fopen("nguoidung.txt", "r")) == NULL) {
        printf("Cannot open %s!!!", "nguoidung.txt");
        exit(1);
    }

    char c;
    for(c = getc(file1); c != EOF; c = getc(file1))
        if (c == '\n') total = total+1;
    rewind(file1);
    
    for(int i = 0; i < total+1; i++) {
        eType tmp;
        float a, b;
        int ss;
        fscanf(file1, "%s\t%s\t%d\t%s", tmp.username, tmp.password, &tmp.status, tmp.homepage);
        //printf("%-20s%-10s%-10d%-10s\n\n", tmp.username , tmp.password, tmp.status, tmp.homepage);
        InsertNode(tmp, &T);
    }
    //inorderprint(T);
}

// Chức năng 1 (Register): Đăng ký thông tin tài khoản mới. Các thông tin được nhập từ bàn phím
//bao gồm: tên tài khoản (username), mật khẩu (password).
void func1(){
    readFile(); 

    char inpUsername[Max], inpPassword[Max], inpHomepage[Max];
    printf("1. Resident: Please enter username and password and Homepage!!! \n");
    printf("Username: "); 
    fflush(stdin); 
    scanf("%s", inpUsername);
    printf("Password: "); 
    fflush(stdin); 
    scanf("%s", inpPassword);
    printf("Homepage: "); 
    fflush(stdin); 
    scanf("%s", inpHomepage);
    
    //printf("%s%s", inpUsername, inpPassword);

    Tree_T c1;
    c1 = SearchUsername(inpUsername, T);

    if (c1 != NULL) {
        printf("Tai khoan da ton tai\n");
    } else {
        eType tmp;
        strcpy(tmp.username, inpUsername);
        strcpy(tmp.password, inpPassword);
        strcpy(tmp.homepage, inpHomepage);
        tmp.status = 2;
        InsertNode(tmp, &T);
    }
    inorderprint(T);


}

//Chức năng 2 (Activate): Người dùng cần phải kích hoạt tài khoản. Các thông tin cần nhập bao
//gồm: tên tài khoản (username), mật khẩu (password), mã kích hoat (activation code). 
void func2(){

    char inpUsername[Max], inpPassword[Max], code[Max];
    printf("1. Activate: Please enter username, password and activation code \n");
    printf("Username: "); 
    fflush(stdin); 
    scanf("%s", inpUsername);
    printf("Password: "); 
    fflush(stdin); 
    scanf("%s", inpPassword);
    // printf("Activation code: ");
    // fflush(stdin);
    // scanf("%s", code);
    //printf("%s%s", inpUsername, inpPassword);

     Tree_T c2;
    c2 = Search(inpUsername, inpPassword, T);

    if (c2 != NULL) {
        int i;
        if (c2->data.status == 2) {
            while (c2->data.count < 4) {
                printf("Activation code: ");
                fflush(stdin);
                scanf("%s", code);
                if (strcmp(c2->data.code, code) == 0) {
                    printf("User: %s da duoc kich hoat thanh cong!\n", inpUsername);
                    c2->data.status = 1;   
                    break;
                } else {
                    printf("Nhap sai code! y/c nhap lai!\n");
                    c2->data.count++;
                }
            } 
            if (c2->data.count >= 4) c2->data.status = 0;
        } else if(c2->data.status == 1) {
            printf("Tai khoan da duoc kich hoat!\n");
        } else if(c2->data.status == 0) {
            printf("Tai khoan da bi khoa! \n");
        }
    } else {
       printf("Input usernam or password bi sai hoac chua ton tai!\n");
    }
    inorderprint(T);
}
eType signIn;

//Chức năng 3 (Sign in): Đăng nhập. Người dùng cần nhập các thông tin từ bàn phím bao gồm: tên
//tài khoản (username), mật khẩu (password). 
void func3(){

    if (cnt == 1) {
        printf("\nTai khoan da dang nhap!\n");
    } else {
        char inpUsername[Max], inpPassword[Max];
        printf("1. Activate: Please enter username, password \n");
        printf("Username: "); 
        fflush(stdin); 
        scanf("%s", inpUsername);
        printf("Password: "); 
        fflush(stdin); 
        scanf("%s", inpPassword);
    
        Tree_T c3;
        c3 = Search(inpUsername, inpPassword, T);

        if (c3 != NULL) {
            int i;
            if (c3->data.status == 1) {
                c3->data.check = 1;
                printf("Dang nhap thanh cong! \n");
                cnt = 1;
                signIn = c3->data;
                c3->data.count = 0;
            } else if (c3->data.status == 0) {
                printf("Tai khoan da bi khoa! \n");
            } else if (c3->data.status == 2) {
                printf("Tai khoan chua duoc kich hoat! \n");
            }
        } else {
            Tree_T elsec3;
            elsec3 = SearchUsername(inpUsername, T);
            if (elsec3 != NULL) {
                elsec3->data.count++;
                printf("Nhap password sai! \n");
                if (elsec3->data.count > 3) {
                    printf("Tai khoan da nhap password sai qua 3 lan! \n");
                    elsec3->data.status = 0;
                }
            } else {
                printf("Input usernam or password bi sai hoac chua ton tai!");
            }
        }
    }
    //inorderprint(T);
}

//Chức năng 4 (Search): Tìm kiếm thông tin một tài khoản. Thông tin nhập vào từ bàn phím là tên
//tài khoản. Chương trình cần hiển thị tên tài khoản tìm thấy và thông báo trạng thái của tài khoản là active hay blocked.
void func4(){
    char inpUsername[Max];
    printf("1. Activate: Please enter username \n");
    printf("Username: "); 
    fflush(stdin); 
    scanf("%s", inpUsername);
    
    Tree_T c4;
    c4 = SearchUsername(inpUsername, T);

    if (c4 != NULL) {
        int i;
        if (c4->data.status == 1) {
            printf("Tai khoan da duoc kich hoat! \n");
        } else if (c4->data.status == 0) {
            printf("Tai khoan da bi khoa! \n");
        } else if (c4->data.status == 2) {
            printf("Tai khoan chua duoc kich hoat! \n");
        }
    } else {
       printf("Tai khoan khong ton tai!!!");
    }

}

//Chức năng 5 (Change password): Người dùng yêu cầu đổi mật khẩu của mình
void func5(){
    char inpUsername[Max], inpPassword[Max];
    printf("1. Activate: Please enter username \n");
    printf("Username: "); 
    fflush(stdin); 
    scanf("%s", inpUsername);
    
    Tree_T c5;
    c5 = SearchUsername(inpUsername, T);

    if (c5 != NULL) {
        printf("Password: "); 
        fflush(stdin); 
        scanf("%s", inpPassword);
        if (strcmp(c5->data.password, inpPassword) == 0) {
            
            char newPassword[Max];
            printf("New Password: "); 
            fflush(stdin); 
            scanf("%s", newPassword);
            strcpy(c5->data.password, newPassword);
            signIn = c5->data;
        } else {
            printf("Nhap sai password! y/c nhap lai!\n");
        }

    } else {
       printf("Tai khoan khong ton tai!!!");
    }
    inorderprint(T);
}

//Chức năng 6 (Sign out): Đăng xuất. Người dùng cần nhập vào từ bàn phím Tên tài khoản đã đăng nhập. 
void func6(){

    if (cnt == 1) {
        char inpUsername[Max];
        printf("1. Activate: Please enter username \n");
        printf("Username: "); 
        fflush(stdin); 
        scanf("%s", inpUsername);
    
        Tree_T c6;
        c6 = SearchUsername(inpUsername, T);

        if (c6 != NULL) {
            if (c6->data.check) {
                printf("Dang xuat thanh cong!\n");
                cnt = 0;
            }
            else printf("Tai khoan chua dang nhap! \n");
        } else {
            printf("Tai khoan khong ton tai!!!");
        }
    } else {
        printf("Tai khoan chua dang nhap \n");
    }
    
}

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
    // printf("Alias IP: ");

    // for(int i = 1; addr_list[i] != NULL; ++i) {
    //     printf("%s ", inet_ntoa(*addr_list[i]));
    // } 

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
    // printf("Alias name: \n");
    // for (int i = 0; hp-> h_aliases[i] != NULL; i++) {
    //     printf("%s\n", hp->h_aliases[i]);
    // }

    return 0;
}

struct in_addr ip;

// Homepage with domain name
// int fun7() {
//     if(!inet_pton(AF_INET,signIn.homepage, &ip)) {
//         return hostnameToIp(signIn.homepage);
//     } else printf("wrong!!! \n");

// }

// Homepage with IP address
// int fun8() {
//     if(!inet_pton(AF_INET,signIn.homepage, &ip)) {
//         printf("Wrong!!! \n");
//     } else {
//         return ipToHostname(signIn.homepage, ip);
//     }

// }


void menu()
{
    int func;
    printf("USER MANAGEMENT PROGRAM\n");
    printf("-----------------------------------\n");
	do 
	{
        printf("\n1. Register\n2. Activate\n3. Sign in\n4. Search\n5. Change password\n6. Sign out");
        printf("\n7. Homepage with domain name\n8. Homepage with IP address\n9. Exit\n\n");
        
        printf("Your choice (1-9, other to quit):");
		fflush(stdin);
		scanf("%d", &func);
		while(func < 1 || func > 9)
		{
			printf("Khong Hop Le! Nhap Lai: ");
			fflush(stdin);
			scanf("%d", &func);
		}
		switch(func)
		{
			case 1: 
                func1();
                break;
			case 2: 
                func2();
                break;
			case 3: 
                cnt++;
                if (cnt == 1) {
                    func3();
                    break;
                } else {
                    printf("\nTai khoan da dang nhap!\n");
                }
            case 4: 
                if (cnt == 0) {
                    printf("\nChoose option 3 first!\n");
                    break;
                }
                func4();
                break;
            case 5:
                if (cnt == 0) {
                    printf("\nChoose option 3 first!\n");
                    break;
                }
                func5();
                break;
			case 6:
                if (cnt == 0) {
                    printf("\nChoose option 3 first!\n");
                    break;
                }
                cnt--;
                func6();
                break;
            case 7:
                if (cnt == 0) {
                    printf("\nChoose option 3 first!\n");
                    break;
                }
                //func7();
                break;
            case 8:
                if (cnt == 0) {
                    printf("\nChoose option 3 first!\n");
                    break;
                }
                //func8();
                break;
		}
	} while(func != 9);
}

int main() {
    //menu();

        int func;
	printf("USER MANAGEMENT PROGRAM\n");
    printf("-----------------------------------\n");
	do 
	{
        printf("\n1. Register\n2. Activate\n3. Sign in\n4. Search\n5. Change password\n6. Sign out");
        printf("\n7. Homepage with domain name\n8. Homepage with IP address\n9. Exit\n\n");
        
        printf("Your choice (1-9, other to quit):");
		fflush(stdin);
		scanf("%d", &func);
		while(func < 1 || func > 9)
		{
			printf("Khong Hop Le! Nhap Lai: ");
			fflush(stdin);
			scanf("%d", &func);
		}
		switch(func)
		{
			case 1: 
                func1();
                break;
			case 2: 
                func2();
                break;
			case 3: 
                func3();
                break;
            case 4: 
                if (cnt == 0) {
                    printf("\nChoose option 3 first!\n");
                    break;
                }
                func4();
                break;
            case 5:
                if (cnt == 0) {
                    printf("\nChoose option 3 first!\n");
                    break;
                }
                func5();
                break;
			case 6:
                if (cnt == 0) {
                    printf("\nChoose option 3 first!\n");
                    //printf("cnt = %d", cnt);
                    break;
                }
                func6();
                break;
            case 7:
                if (cnt == 0) {
                    printf("\nChoose option 3 first!\n");
                    break;
                }
                //func7();
                if(!inet_pton(AF_INET,signIn.homepage, &ip)) {
                    int i;
                    //return hostnameToIp(signIn.homepage);
                    i = hostnameToIp(signIn.homepage);
                } else printf("Wrong!!! \n");

                break;
            case 8:
                if (cnt == 0) {
                    printf("\nChoose option 3 first!\n");
                    break;
                }
                //func8();
                if(!inet_pton(AF_INET,signIn.homepage, &ip)) {
                    printf("Wrong!!! \n");
                } else {
                    int i;
                    //return ipToHostname(signIn.homepage, ip);
                    i = ipToHostname(signIn.homepage, ip);
                }
                break;
		}
	} while(func != 9);

    return 0;
}