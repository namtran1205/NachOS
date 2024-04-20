#include "syscall.h"
#include "copyright.h"

void main()
{
    // Declarations
    int success;                     // bien kiem tra co thanh cong khi mo, tao file
    SpaceId scannerId, resultId;     // File ID
    char currentChar;                // bien tam luu ki tu duoc doc tu file
    int capacity1, capacity2, capacity3;  // so hanh ly da duoc xu li cua cac may quet 1, 2, 3
    int capacity;                    // so hanh ly dang can duoc xu li 
    int isResultDone;                // Flag bao hieu da xu li xong tat ca hanh ly cua mot thoi diem

    //-----------------------------------------------------------
    
    // khoi tao gia tri ban dau cho moi may quet
    capacity1 = capacity2 = capacity3 = 0;
    
    while (1)
    {
        // cho den khi tien trinh Passenger khoi chay
        Down("_scanner");

        // Mo file "result.txt" de ghi ket qua xu li duoc 
        resultId = Open("result.txt", 0);
        if (resultId == -1)
        {    
            Up("passenger");
            return;
        }

        while (1)
        {
            // cho yeu cau xu li cua Passenger
            Down("scanner");
            currentChar = 0;            
            
            // mo file "scanner.txt" doc so luong hanh ly cua hanh khach
            scannerId = Open("scanner.txt", 1);
            if (scannerId == -1)
            {
                Close(resultId);
                Up("passenger");
                return;
            }
        
            capacity = 0;
            isResultDone = 0;
            // doc va chuyen doi so luong hanh ly tu dang string sang int
            while (1)
            {       
                // ngat khi da doc het file 
                if (Read(&currentChar, 1, scannerId)  == -2)
                {    
                    Close(scannerId);        
                    break;
                }


                if (currentChar != '*') 
                {
                    capacity = capacity * 10 + (currentChar - '0');
                }
                else // ngat khi da doc duoc dau hieu ket thuc 1 thoi diem
                {
                    isResultDone = 1;                
                    Close(scannerId);
                    break;            
                }

            }

            // chon may quet cho hanh khach
            // - May quet duoc chon se la may quet co tong so hanh ly da duoc xu li la MIN
            // - Ghi ket qua "soHanhLy tenMayQuet  " vao "result.txt"
            if (capacity != 0)
            {
                if (capacity1 <= capacity2 && capacity1 <= capacity3)
                {
                    capacity1 += capacity;
                    WriteInt(capacity, resultId);
                    Write(" 1  ", 4, resultId);
                }
                else if (capacity2 <= capacity1 && capacity2 <= capacity3)
                {
                    capacity2 += capacity;
                    WriteInt(capacity, resultId);
                    Write(" 2  ", 4, resultId);
                }
                else 
                {
                    capacity3 += capacity;
                    WriteInt(capacity, resultId);
                    Write(" 3  ", 4, resultId);
                }
            }

            if (isResultDone == 1)
            {
                // reset gia tri cua cac may quet khi ket thuc mot thoi diem
                capacity1 = capacity2 = capacity3 = 0;
                Close(resultId);                
                Up("passenger"); // tro ve Passenger va tiep truc tro ve main de ghi ket qua
                break;                
            }
            Up("passenger"); // tro ve Passenger de ghi hanh khach tiep theo
        }
    }
}
