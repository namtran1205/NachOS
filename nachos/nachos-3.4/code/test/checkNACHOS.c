#include "syscall.h"
#include "copyright.h"
#define maxlen 32
int
main()
{
    int len;
    char filename[maxlen +1];
/*Create a file*/
    if (Create(“text.txt”) = = -1)
    {
    // xuất thông báo lỗi tạo tập tin
	printf("\nCan not create file.\n");
    }
    else
    {
    // xuất thông báo tạo tập tin thành công
	printf("\nSuccessfully create file\n");
    }
    Halt();
}