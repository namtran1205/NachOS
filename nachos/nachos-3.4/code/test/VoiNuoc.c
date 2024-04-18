#include "syscall.h"
#include "copyright.h"

void main()
{
	// Khai bao
	int f_Success; // Bien co dung de kiem tra thanh cong
	SpaceId  si_voinuoc, si_result;	// Bien id cho file
	char c_readFile;	// Bien ki tu luu ki tu doc tu file
	int v1, v2, v3;		// Voi 1, voi 2, voi 3
	int v;			// Dung tich nuoc cua sinh vien
	int flag_done_result;	// Bien co luu dau hieu doc xong file result

	//-----------------------------------------------------------
	
	v1 = v2 = v3 = 0;
	// Xu ly voi nuoc
	while(1)
	{
		Down("m_vn");

		// Mo file result.txt de ghi voi nao su dung
		si_result = Open("result.txt", 0);
		if(si_result == -1)
		{		//?
			Up("sinhvien");
			return;
		}
		while(1)
		{
			Down("voinuoc");
			c_readFile = 0;			
			// Mo file voi nuoc .txt de doc dung tich
			si_voinuoc = Open("voinuoc.txt", 1);
			if(si_voinuoc == -1)
			{
				//?
				Close(si_result);
				Up("sinhvien");
			
				return;
			}
		
			v = 0;
			flag_done_result = 0;
			while(1)
			{		
				if(Read(&c_readFile, 1, si_voinuoc)  == -2)
				{	
					Close(si_voinuoc);		
					PrintChar('\n'); // test 	
					break;
				}
				PrintChar(c_readFile);	// test
				if(c_readFile != '*')
				{
					v = v * 10 + (c_readFile - 48);
				}
				else
				{
					flag_done_result = 1;				
					Close(si_voinuoc);
					break;			
				}

			}



			if(v != 0)
			{
				if(v1 <= v2 && v1 <= v3)
				{
					v1 += v;
					WriteInt(v, si_result);
					Write("1", 1, si_result);   // format "v 1 "
					// Write("1", 1, si_result);
				}
				else if (v2 <= v3 && v2 <= v1)
				{
					v2 += v;
					WriteInt(v, si_result);
					Write("2", 1, si_result); 
				}
				else 
				{
					v3 += v;
					WriteInt(v, si_result);
					Write("3", 1, si_result); 
				}


				
		

				// // Dung voi 1
				// if(v1 <= v2)
				// {
				// 	v1 += v;
				// 	Write("1", 1, si_result);
				// }
				// else	// Dung voi 2
				// {					
				// 	v2 += v;
				// 	Write("2", 1, si_result);
					
				// }
			}
		
			if(flag_done_result == 1)
			{
				v1 = v2 = v3 = 0;
				Close(si_result);				
				Up("sinhvien");
				break;				
			}

			Up("sinhvien");
		}
	}
}
