#include "syscall.h"
#include "copyright.h"
#define maxlen 32
int
main()
{
    float* f1;
    float* f2;
    int compare;

    f1 = ReadFloat();
    f2 = ReadFloat();

    compare = CompareFPs(f1, f2);
    if (compare == -1)
    {
        PrintString("f1 < f2\n");
    }
    else if (compare == 0)
    {
        PrintString("f1 = f2\n");
    }
    else
    {
        PrintString("f1 > f2\n");
    }

    FreeFloat(f1);
    FreeFloat(f2);

    Halt();
    return 0;
}