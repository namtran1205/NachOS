#include "syscall.h"
#include "copyright.h"

int main()
{
    OpenFileID dataFileID;
    char contentBlock[300];
    int realSize;
    
    // mo file mota bang system call Open
    dataFileID = Open("mota.txt", 1);

    // Kiem tra co ton tai file mota(file chua du lieu can in)
    if (dataFileID == -1)
    {
        PrintString("Error with data\n");
        return 0;
    }

    // doc tung khoi 299 ki tu
    while (1)
    {
        realSize = Read(contentBlock, 299, dataFileID);
        if (realSize > 0) 
        {
            PrintString(contentBlock);
            contentBlock[realSize] = '\0';
        }
        if (realSize < 299) break; // dung khi so ki tu thuc su khong du
    }

    if (dataFileID != -1) Close(dataFileID);

    return 0;
}