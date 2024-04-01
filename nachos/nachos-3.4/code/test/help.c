#include "syscall.h"
#include "copyright.h"

int main()
{
    OpenFileID dataFileID;
    char contentBlock[100];
    int realSize;
    
    // mo file mota bang system call Open
    dataFileID = Open("mota.txt", 1);

    // Kiem tra co ton tai file mota(file chua du lieu can in)
    if (dataFileID == -1)
    {
        PrintString("Error with data\n");
        return 0;
    }

    // doc tung khoi 99 ki tu
    while (1)
    {
        realSize = Read(contentBlock, 99, dataFileID);
        if (realSize > 0) 
        {
            PrintString(contentBlock);
            contentBlock[realSize] = '\0';
        }
        if (realSize < 99) break;
    }

    if (dataFileID != -1) Close(dataFileID);

    return 0;
}