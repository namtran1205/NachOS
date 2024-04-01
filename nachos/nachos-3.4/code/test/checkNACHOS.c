#include "syscall.h"
#include "copyright.h"
#define maxlen 32
int
main()
{
    OpenFileID id;
    id = Open("mergesort.txt", 0);
    Write("Hello World", 11, id);
    Close(id);

    Halt();
    return 0;    
}