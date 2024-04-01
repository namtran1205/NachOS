#include "syscall.h"
#include "copyright.h"

char c[1]; // bien luu ki tu tuong ung voi gia tri i
int arr[1]; // bien luu ma cua ki tu
int main()
{
    // mo file mota bang system call Open
    int isCreate = Create("ascii.txt");
    OpenFileID fileID = Open("ascii.txt", 0);


    int i;
    // in cac ki tu ra man hinh
    // cac ki tu co the hien thi duoc co ma tu 32 den 126
    PrintString("Ascii  table:\n");
    for(i = 32; i < 127; i++)
    {
        PrintInt(i);
        PrintString(" : ");
        PrintChar(i);
        PrintChar('\n');
    }

    // trong truong hop chay lan dau tien
    // => chua co du lieu trong file ascii
    // => can ghi vao file cac ki tu
    if (isCreate || fileID < 0) return 0;

    Write("Ascii  table:\n", 14, fileID);

    c[0] = (char) 32;
    arr[0] = 32;
    for(i = 32; i < 127; i++)
    {
        WriteInt((int)arr[0], fileID);
        // Write(" : ", 3, fileID);
        // Write(c, 1, fileID);
        Write("\n", 1, fileID);
        c[0]++; // chuyen thanh ki tu tiep theo
        arr[0]++;
    }

    Halt();
    return 0;
}