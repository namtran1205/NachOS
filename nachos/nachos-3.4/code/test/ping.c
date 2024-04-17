#include "syscall.h"
#include "copyright.h"


void 
main()
{
    int i;
    for(i = 0; i < 1000; ++i)
    {
        Down("sem_pong");
        PrintChar('A');
        Up("sem_ping");
    }
}