#include "syscall.h"
#include "copyright.h"

void main()
{
	// Khai bao
	int f_Success; // Bien co dung de kiem tra thanh cong
	SpaceId  si_scanner, si_result;	// Bien id cho file
	char c_readFile;	// Bien ki tu luu ki tu doc tu file
	int cam1, cam2, cam3;		
	int cam;			
	int flag_done_result;	// Bien co luu dau hieu doc xong file result

	//-----------------------------------------------------------
	
	cam1 = cam2 = cam3 = 0;
	while(1)
	{
		Down("_scanner");

		// Mo file result.txt de ghi voi nao su dung
		si_result = Open("result.txt", 0);
		if(si_result == -1)
		{	
			Up("passenger");
			return;
		}
		while(1)
		{
			Down("scanner");
			c_readFile = 0;			
			// Mo file voi nuoc .txt de doc dung tich
			si_scanner = Open("scanner.txt", 1);
			if(si_scanner == -1)
			{
				//?
				Close(si_result);
				Up("passenger");
			
				return;
			}
		
			cam = 0;
			flag_done_result = 0;
			while(1)
			{		
				if(Read(&c_readFile, 1, si_scanner)  == -2)
				{	
					Close(si_scanner);		
					// PrintChar('\n'); // test 	
					break;
				}
				// PrintChar(c_readFile);	// test
				if(c_readFile != '*')
				{
					cam = cam * 10 + (c_readFile - 48);
				}
				else
				{
					flag_done_result = 1;				
					Close(si_scanner);
					break;			
				}

			}



			if(cam != 0)
			{
				if(cam1 <= cam2 && cam1 <= cam3)
				{
					cam1 += cam;
					WriteInt(cam, si_result);
					Write("1", 1, si_result);
					Write("  ", 2,si_result); // format "v 1 "
					// Write("1", 1, si_result);
				}
				else if (cam2 <= cam1 && cam2 <= cam3)
				{
					cam2 += cam;
					WriteInt(cam, si_result);
					Write("2", 1, si_result); 
					Write("  ", 2,si_result); // format "v 1 "

				}
				else 
				{
					cam3 += cam;
					WriteInt(cam, si_result);
					Write("3", 1, si_result); 
					Write("  ", 2,si_result); // format "v 1 "
				}
			}
			if(flag_done_result == 1)
			{
				cam1 = cam2 = cam3 = 0;
				Close(si_result);				
				Up("passenger");
				break;				
			}
			Up("passenger");
		}
	}
}
