#include <stdio.h>

// Định nghĩa một struct
struct SinhVien {
    char ten[50];
    int tuoi;
    float diem;
};

// Hàm để ghi struct vào tệp tin
void ghiStructVaoTep(FILE *tep, struct SinhVien sv) {
    // Kiểm tra xem tệp có thể mở hay không
    if (tep == NULL) {
        printf("Không thể mở tệp tin.\n");
        return;
    }

    // Ghi thông tin từ struct vào tệp tin
    fprintf(tep, "%s\t%d\t%.2f", sv.ten, sv.tuoi, sv.diem);
        

    printf("Đã ghi struct vào tệp tin thành công.\n");
}

int main() {
    // Khai báo một struct và khởi tạo giá trị
    struct SinhVien sinhVien1 = {"Nguyen Van A", 20, 8.5};

    // Mở tệp tin để ghi
    FILE *file = fopen("output.txt", "w");

    // Gọi hàm để ghi struct vào tệp tin
    ghiStructVaoTep(file, sinhVien1);

    // Đóng tệp tin sau khi ghi
    fclose(file);

    return 0;
}
