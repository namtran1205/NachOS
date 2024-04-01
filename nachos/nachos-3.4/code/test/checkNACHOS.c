#include "syscall.h"
#include "copyright.h"
#define maxlen 32
int
main()
{
    PrintString("Ascii  table:\n");
    for(i = 32; i < 127; i++)
    {
        PrintInt(i);
        PrintString(" : \"");
        PrintChar(i);
        PrintChar('\"\n');
    }
    Halt();
    return 0;

    
}