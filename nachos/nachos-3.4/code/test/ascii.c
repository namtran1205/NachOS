#include "syscall.h"
#include "copyright.h"

char c[1];

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


    if (fileID < 0) return 0;

    Write("Ascii  table:\n", 14, fileID);
    c[0] = 32;
    for(i = 32; i < 127; i++)
    {
        WriteInt(i, fileID);
        Write(" : ", 3, fileID);
        Write(c, 1, fileID);
        Write("\n", 1, fileID);
        c[0]++;
    }
    Close(fileID);

    Halt();
    return 0;
}