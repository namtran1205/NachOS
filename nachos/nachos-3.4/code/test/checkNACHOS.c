#include "syscall.h"
#include "copyright.h"
#define maxlen 32
int
main()
{
    char a;
    a = ReadChar();
    PrintChar(a);

    Halt();
    return 0;
}