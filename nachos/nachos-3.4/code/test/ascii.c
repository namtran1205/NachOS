#include "syscall.h"
#include "copyright.h"

int int2char(int x, char str[])
{
    int startPoint = 0;
    if (x < 0)
    {
        startPoint++;
        x *= -1;
    }

    int pw = 1, cnt = 1;

    // tinh so chu so cua so
    while (x / pw > 9) 
    {
        pw *= 10;
        cnt++;
    }

    for(int i = 0; i < cnt; i++)
    {
        str[startPoint + i] = x / pw + 48;
        x %= pw;
        pw /= 10;
    }
    if (startPoint) str[0] = '-';
    str[cnt + startPoint] = '\0';
    return cnt + startPoint;
}

void filePrintInt(int x, OpenFileID fileID)
{
    char *num = (char*) maloc(sizeof(char) * 20);
    int len = int2char(x, num);
    Write(num, len, fileID);
    free(num);
}

int main()
{
    // mo file mota bang system call Open
    int isCreate = Create("ascii.txt");
    OpenFileID fileID = Open("ascii.txt", 0);


    char asciiCharacter[] = " : \" \"\n";
    char num[5];

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

    Write("Ascii  table:\n", 15, fileID);

    for(i = 32; i < 127; i++)
    {
        filePrintInt(i, fileID);
        Write(asciiCharacter, 7, fileID);
        asciiCharacter[4]++; // cap nhat thanh ki tu tiep theo
    }

    return 0;
}