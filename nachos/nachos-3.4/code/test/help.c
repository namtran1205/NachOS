#include "syscall.h"
#include "copyright.h"

int main()
{
    // mo file mota bang system call Open
    OpenFileID dataFileID = Open("mota.txt", 1);

    // Kiem tra co ton tai file mota(file chua du lieu can in)
    if (dataFileID == -1)
    {
        PrintString("Error with data\n");
        return 0;
    }

    // doc tung khoi 99 ki tu
    char* contentBlock = (char*) maloc(100 * sizeof(char));
    while (1)
    {
        int realSize = read(contentBlock, 99, dataFileID);
        if (realSize > 0) 
        {
            PrintString(contentBlock);
            contentBlock[realSize] = '\0';
        }
        if (realSize < 99) break;
    }

    if (dataFileID != -1) close(dataFileID);
    free(contentBlock);


    return 0;
}