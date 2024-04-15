#include "syscall.h"


int main() 
{
	int ping, pong;
	PrintString("Testing Ping-pong programe\n");
	
	ping = Exec("./test/ping");
	pong = Exec("./test/pong");
	Join(ping);
	PrintString("Join ping successfully\n");
	Join(pong);
	PrintString("Join pong successfully\n");
    return 0;
}