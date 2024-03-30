#include "syscall.h"
#include "copyright.h"
#define maxlen 32
int
main()
{
    char* a;
    ReadString(a, 255);
    PrintString(a);

    Halt();
    return 0;
}