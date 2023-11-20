#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const int Max = 100;

typedef struct
{
    char username[Max];
    char password[Max];
    int status;
    char code[Max];
    int count;
    int check;
} eType;

typedef struct nodeType
{
    eType data;
    struct nodeType *left, *right;
} node_Type;

typedef node_Type *Tree_T;
Tree_T T;

eType *a;
int total;
#include "linklist.h"

FILE *file1, *file2;

void readFile()
{
    if ((file1 = fopen("nguoidung.txt", "r")) == NULL)
    {
        printf("Cannot open %s!!!", "nguoidung.txt");
        exit(1);
    }

    char c;
    for (c = getc(file1); c != EOF; c = getc(file1))
        if (c == '\n')
            total = total + 1;
    rewind(file1);

    for (int i = 0; i < total + 1; i++)
    {
        eType tmp;
        float a, b;
        int ss;
        fscanf(file1, "%s\t%s\t%d", tmp.username, tmp.password, &tmp.status);
        InsertNode(tmp, &T);
    }

    // inorderprint(T);
}

// Chức năng 1 (Register): Đăng ký thông tin tài khoản mới. Các thông tin được nhập từ bàn phím
// bao gồm: tên tài khoản (username), mật khẩu (password).
void func1()
{
    readFile();

    char inpUsername[Max], inpPassword[Max];
    printf("1. Resident: Please enter username and password \n");
    printf("Username: ");
    fflush(stdin);
    scanf("%s", inpUsername);
    printf("Password: ");
    fflush(stdin);
    scanf("%s", inpPassword);
    // printf("%s%s", inpUsername, inpPassword);

    Tree_T c1;
    c1 = SearchUsername(inpUsername, T);

    if (c1 != NULL)
    {
        printf("Tai khoan da ton tai\n");
    }
    else
    {
        eType tmp;
        strcpy(tmp.username, inpUsername);
        strcpy(tmp.password, inpPassword);
        tmp.status = 2;
        InsertNode(tmp, &T);
    }
    inorderprint(T);
}

// Chức năng 2 (Activate): Người dùng cần phải kích hoạt tài khoản. Các thông tin cần nhập bao
// gồm: tên tài khoản (username), mật khẩu (password), mã kích hoat (activation code).
void func2()
{

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
    // printf("%s%s", inpUsername, inpPassword);

    Tree_T c2;
    c2 = Search(inpUsername, inpPassword, T);

    if (c2 != NULL)
    {
        int i;
        if (c2->data.status == 2)
        {
            while (c2->data.count < 4)
            {
                printf("Activation code: ");
                fflush(stdin);
                scanf("%s", code);
                if (strcmp(c2->data.code, code) == 0)
                {
                    printf("User: %s da duoc kich hoat thanh cong!\n", inpUsername);
                    c2->data.status = 1;
                    break;
                }
                else
                {
                    printf("Nhap sai code! y/c nhap lai!\n");
                    c2->data.count++;
                }
            }
            if (c2->data.count >= 4)
                c2->data.status = 0;
        }
        else if (c2->data.status == 1)
        {
            printf("Tai khoan da duoc kich hoat!\n");
        }
        else if (c2->data.status == 0)
        {
            printf("Tai khoan da bi khoa! \n");
        }
    }
    else
    {
        printf("Input usernam or password bi sai hoac chua ton tai!\n");
    }

    inorderprint(T);
}

// Chức năng 3 (Sign in): Đăng nhập. Người dùng cần nhập các thông tin từ bàn phím bao gồm: tên
// tài khoản (username), mật khẩu (password).
void func3()
{

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

    if (c3 != NULL)
    {
        int i;
        if (c3->data.status == 1)
        {
            c3->data.check = 1;
            printf("Dang nhap thanh cong! \n");
        }
        else if (c3->data.status == 0)
        {
            printf("Tai khoan da bi khoa! \n");
        }
        else if (c3->data.status == 2)
        {
            printf("Tai khoan chua duoc kich hoat! \n");
        }
    }
    else
    {
        Tree_T elsec3;
        elsec3 = SearchUsername(inpUsername, T);
        if (elsec3 != NULL)
        {
            elsec3->data.count++;
            printf("Nhap password sai! \n");
            if (elsec3->data.count > 3)
            {
                printf("Tai khoan da nhap password sai qua 3 lan! \n");
                elsec3->data.status = 0;
            }
        }
        else
        {
            printf("Input usernam or password bi sai hoac chua ton tai!");
        }
    }

    // inorderprint(T);
}

// Chức năng 4 (Search): Tìm kiếm thông tin một tài khoản. Thông tin nhập vào từ bàn phím là tên
// tài khoản. Chương trình cần hiển thị tên tài khoản tìm thấy và thông báo trạng thái của tài khoản là active hay blocked.
void func4()
{
    char inpUsername[Max];
    printf("1. Activate: Please enter username \n");
    printf("Username: ");
    fflush(stdin);
    scanf("%s", inpUsername);

    Tree_T c4;
    c4 = SearchUsername(inpUsername, T);

    if (c4 != NULL)
    {
        int i;
        if (c4->data.status == 1)
        {
            printf("Tai khoan da duoc kich hoat! \n");
        }
        else if (c4->data.status == 0)
        {
            printf("Tai khoan da bi khoa! \n");
        }
        else if (c4->data.status == 2)
        {
            printf("Tai khoan chua duoc kich hoat! \n");
        }
    }
    else
    {
        printf("Tai khoan khong ton tai!!!");
    }
}

// Chức năng 5 (Change password): Người dùng yêu cầu đổi mật khẩu của mình
void func5()
{
    char inpUsername[Max], inpPassword[Max];
    printf("1. Activate: Please enter username \n");
    printf("Username: ");
    fflush(stdin);
    scanf("%s", inpUsername);

    Tree_T c5;
    c5 = SearchUsername(inpUsername, T);

    if (c5 != NULL)
    {
        printf("Password: ");
        fflush(stdin);
        scanf("%s", inpPassword);
        if (strcmp(c5->data.password, inpPassword) == 0)
        {

            char newPassword[Max];
            printf("New Password: ");
            fflush(stdin);
            scanf("%s", newPassword);
            strcpy(c5->data.password, newPassword);
        }
        else
        {
            printf("Nhap sai password! y/c nhap lai!\n");
        }
    }
    else
    {
        printf("Tai khoan khong ton tai!!!");
    }
}

// Chức năng 6 (Sign out): Đăng xuất. Người dùng cần nhập vào từ bàn phím Tên tài khoản đã đăng nhập.
void func6()
{
    char inpUsername[Max];
    printf("1. Activate: Please enter username \n");
    printf("Username: ");
    fflush(stdin);
    scanf("%s", inpUsername);

    Tree_T c6;
    c6 = SearchUsername(inpUsername, T);

    if (c6 != NULL)
    {
        if (c6->data.check)
            printf("Dang xuat thanh cong!\n");
        else
            printf("Tai khoan chua dang nhap! \n");
    }
    else
    {
        printf("Tai khoan khong ton tai!!!");
    }
}

void menu()
{
    int func;
    int cnt = 0;
    printf("USER MANAGEMENT PROGRAM\n");
    printf("-----------------------------------\n");
    do
    {
        printf("\n1. Register\n2. Activate\n3. Sign in\n4. Search\n5. Change password\n6. Sign out\n");
        printf("Your choice (1-6, other to quit):");
        fflush(stdin);
        scanf("%d", &func);
        while (func < 1 || func > 6)
        {
            printf("Khong Hop Le! Nhap Lai: ");
            fflush(stdin);
            scanf("%d", &func);
        }
        switch (func)
        {
        case 1:
            func1();
            break;
        case 2:
            func2();
            break;
        case 3:
            cnt++;
            if (cnt >= 1)
            {
                func3();
                break;
            }
        case 4:
            if (cnt == 0)
            {
                printf("\nChoose option 3 first!\n");
                break;
            }
            func4();
            break;
        case 5:
            if (cnt == 0)
            {
                printf("\nChoose option 3 first!\n");
                break;
            }
            func5();
            break;
        case 6:
            if (cnt == 0)
            {
                printf("\nChoose option 3 first!\n");
                break;
            }
            func6();
            break;
        }
    } while (func != 6);
}

int main()
{
    menu();
    return 0;
}