#include "syscall.h"
#include "copyright.h"
#define maxlen 32
int
main()
{
    char* filename;
    ReadString(filename, 255);
    Create(filename);

    Halt();
    return 0;
}