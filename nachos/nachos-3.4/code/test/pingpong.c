#include "syscall.h"


int main() 
{
	int ping, pong;
	int create_success;
	create_success = CreateSemaphore("sem_ping", 4);
	if (create_success == 0)
		PrintString("Create sem_ping successfully\n");
	else
		return;

	
	create_success = CreateSemaphore("sem_pong", 0);
	if (create_success == 0)
		PrintString("Create sem_pong successfully\n");
	else
		return;
	PrintString("Testing Ping-pong programe\n");
	
	ping = Exec("./test/ping");
	pong = Exec("./test/pong");
	Join(ping);
	Join(pong);
    return 0;
}