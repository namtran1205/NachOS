#include "syscall.h"
#include "copyright.h"

void main()
{
	// Khai bao
	int f_Success; // Bien co dung de kiem tra thanh cong
	SpaceId si_sinhvien, si_voinuoc, si_result;	// Bien id cho file
	char c_readFile;	// Bien ki tu luu ki tu doc tu file
	int flag_VN;		// Bien co de nhay den tien trinh voinuoc
	int flag_MAIN;		// Bien co de nhay den tien trinh main
	int lengthFile;		// Luu do dai file
	int i_File;		// Luu con tro file
	//-----------------------------------------------------------
	Up("m_vn");	

	while(1)
	{
		lengthFile = 0;

		Down("sinhvien");
		

		// Tao file result.txt de ghi voi nao su dung
		f_Success = Create("result.txt");
		if(f_Success == -1)
		{
			Up("main"); // tro ve tien trinh chinh
			return;
		}

		// Mo file sinhvien.txt len de doc
		si_sinhvien = Open("sinhvien.txt", 1);
		if(si_sinhvien == -1)
		{
			Up("main"); // tro ve tien trinh chinh
			return;
		}
		
		lengthFile = Seek(-1, si_sinhvien);
		Seek(0, si_sinhvien);
		i_File = 0;
	
		// Tao file voinuoc.txt
		f_Success = Create("voinuoc.txt");
		if(f_Success == -1)
		{
			Close(si_sinhvien);
			Up("main"); // tro ve tien trinh chinh
			return;
		}
		
		// Mở voinuoc.txt để ghi TỪNG dung tích -> sinhvien.txt
		si_voinuoc = Open("voinuoc.txt", 0);
		if(si_voinuoc == -1)
		{
			Close(si_sinhvien);
			Up("main"); // tro ve tien trinh chinh
			return;
		}

		
		si_result = Open("result.txt", 0);
		if(si_result == -1)
		{
			Close(si_result);
			Up("main"); // tro ve tien trinh chinh
			return;
		}



		// Ghi TỪNG dung tich vao file voinuoc.txt tu file sinhvien.txt
		//Ghi TỪNG dung tích vào file result.txt từ file sinhvien.txt
		while(i_File < lengthFile)
		{
			flag_VN = 0;
			Read(&c_readFile, 1, si_sinhvien);
			if(c_readFile != ' ')
			{
				Write(&c_readFile, 1, si_voinuoc);
			}
			else
			{
				flag_VN = 1;
			}
			if(i_File == lengthFile - 1)
			{
				Write("*", 1, si_voinuoc);
				flag_VN = 1;
			}
			
				
			if(flag_VN == 1)
			{
				Close(si_voinuoc);
				Up("voinuoc");
				// Dung chuong trinh sinhvien lai de voinuoc thuc thi
				Down("sinhvien");
				
				// Tao file voinuoc.txt
				f_Success = Create("voinuoc.txt");
				if(f_Success == -1)
				{
					Close(si_sinhvien);
					Up("main"); // tro ve tien trinh chinh
					return;
				}
		

				// Mo file voinuoc.txt de ghi tung dung tich nuoc cua sinhvien
				si_voinuoc = Open("voinuoc.txt", 0);
				if(si_voinuoc == -1)
				{
					Close(si_sinhvien);
					Up("main"); // tro ve tien trinh chinh
					return;
				}
				
			}
			i_File++;			
							
		}				
		// Ket thuc tien trinh sinhvien va voinuoc quay lai ham SvVn
		Up("main");			
	}
	// Quay lai ham Svvn	
}
