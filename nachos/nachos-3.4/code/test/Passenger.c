#include "syscall.h"
#include "copyright.h"

void main()
{
	// Khai bao
	int f_Success; // Bien co dung de kiem tra thanh cong
	SpaceId si_passenger, si_scanner, si_result;	// Bien id cho file
	char c_readFile;	// Bien ki tu luu ki tu doc tu file
	int flag_VN;		// Bien co de nhay den tien trinh voinuoc
	int flag_MAIN;		// Bien co de nhay den tien trinh main
	int lengthFile;		// Luu do dai file
	int i_File;		// Luu con tro file
	//-----------------------------------------------------------
	Up("_scanner");	

	while(1)
	{
		lengthFile = 0;

		Down("passenger");
		
		f_Success = Create("result.txt");
		if(f_Success == -1)
		{
			Up("main");
			return;
		}

		si_passenger = Open("passenger.txt", 1);
		if(si_passenger == -1)
		{
			Up("main"); 
			return;
		}
		
		lengthFile = Seek(-1, si_passenger);
		Seek(0, si_passenger);
		i_File = 0;
	
		f_Success = Create("scanner.txt");
		if(f_Success == -1)
		{
			Close(si_passenger);
			Up("main"); 
			return;
		}
		
		si_scanner = Open("scanner.txt", 0);
		if(si_scanner == -1)
		{
			Close(si_scanner);
			Up("main"); 
			return;
		}



		while(i_File < lengthFile - 1)
		{
			flag_VN = 0;
			Read(&c_readFile, 1, si_passenger);
			if(c_readFile != ' ')
			{
				Write(&c_readFile, 1, si_scanner);
			}
			else
			{
				flag_VN = 1;
			}
			if(i_File == lengthFile - 2)
			{
				Write("*", 1, si_scanner);
				flag_VN = 1;
			}
			
				
			if(flag_VN == 1)
			{
				Close(si_scanner);
				Up("scanner");

				Down("passenger");
				
				f_Success = Create("scanner.txt");
				if(f_Success == -1)
				{
					Close(si_passenger);
					Up("main"); 
					return;
				}
		

				si_scanner = Open("scanner.txt", 0);
				if(si_scanner == -1)
				{
					Close(si_passenger);
					Up("main"); 
					return;
				}
				
			}
			i_File++;			
							
		}				
		Up("main");			
	}
}
