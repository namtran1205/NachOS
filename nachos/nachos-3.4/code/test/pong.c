#include "syscall.h"
#include "copyright.h"


void 
main()
{
    int i;
    for(i = 0; i < 1000; ++i)
    {
        Down("sem_ping");
        PrintChar('B');
        Up("sem_pong");
    }
}