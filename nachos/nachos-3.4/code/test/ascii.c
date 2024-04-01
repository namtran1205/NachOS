#include "syscall.h"
#include "copyright.h"

char asciiCharacter[7] = " : \" \"\n";


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
        PrintString(" : \"");
        PrintChar(i);
        PrintChar('\"\n');
    }

    // trong truong hop chay lan dau tien
    // => chua co du lieu trong file ascii
    // => can ghi vao file cac ki tu
    if (isCreate || fileID < 0) return 0;

    Write("Ascii  table:\n", 14, fileID);

    for(i = 32; i < 127; i++)
    {
        WriteInt(i, fileID);
        Write(asciiCharacter, 7, fileID);
        asciiCharacter[4]++; // cap nhat thanh ki tu tiep theo
    }

    Halt();
    return 0;
}