#include "syscall.h"
#include "copyright.h"

char c[1];

int main()
{
    int i;
    // mo file mota bang system call Open
    int isCreate = Create("ascii.txt");
    OpenFileID fileID = Open("ascii.txt", 0);

    
    // In các ký tự ra màn hình
    // Các ký tự có thể hiển thị được có mã từ 32 đến 126
    PrintString("Ascii  table:\n"); // In chuỗi "Ascii table:\n" ra màn hình
    for(i = 32; i < 127; i++) // Lặp từ mã 32 đến mã 126 của ASCII
    {
        PrintInt(i); // In mã ASCII của ký tự
        PrintString(" : "); // In dấu hai chấm và dấu cách
        PrintChar(i); // In ký tự tương ứng với mã ASCII
        PrintChar('\n'); // In ký tự xuống dòng
    }


    if (fileID < 0) return 0;

    Write("Ascii  table:\n", 14, fileID);
    c[0] = 32;

    
    for(i = 32; i < 127; i++) // Lặp từ mã 32 đến mã 126 của ASCII
    {
        WriteInt(i, fileID); // Ghi mã ASCII vào tập tin
        Write(" : ", 3, fileID); // Ghi dấu hai chấm và dấu cách vào tập tin
        Write(c, 1, fileID); // Ghi ký tự vào tập tin
        Write("\n", 1, fileID); // Ghi ký tự xuống dòng vào tập tin
        c[0]++; // Tăng giá trị ký tự cho lần lặp tiếp theo
    }
    Close(fileID); // Đóng tập tin
    return 0; // Kết thúc chương trình
}