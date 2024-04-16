#include "syscall.h"


int main() 
{
	int ping, pong;
	PrintString("Testing Ping-pong programe\n");
	
	ping = Exec("./test/ping");
	pong = Exec("./test/pong");
	Join(ping);
	Join(pong);
    return 0;
}