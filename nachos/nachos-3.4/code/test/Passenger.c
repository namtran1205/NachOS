#include "syscall.h"
#include "copyright.h"

void main()
{
    int successCheck;                  // bien kiem tra co thanh cong khi mo, tao file
    SpaceId passengerFileId, scannerFileId, resultFileId; // File ID
    char currentChar;                 // bien tam luu ki tu duoc doc tu file
    int jumpToScanner;             // Flag bao hieu can chay tien trinh Scanner
    int fileLength;                   // Chieu dai file
    int filePointer;                  // dai dien con tro doc trong file 

    // khoi dong tien trinh Scanner
    Up("_scanner");

    while (1)
    {
        fileLength = 0;

        // cho yeu cau tu main
        Down("passenger");

        // tao file  "result.txt" de luu ket qua tra ve main
        successCheck = Create("result.txt");
        if (successCheck == -1)
        {
            Up("main");
            return;
        }

        // mo "passenger.txt" de doc yeu cau cua main
        passengerFileId = Open("passenger.txt", 1);
        if (passengerFileId == -1)
        {
            Up("main");
            return;
        }

        // Lay chieu dai cua file "passenger.txt"
        fileLength = Seek(-1, passengerFileId);
        Seek(0, passengerFileId);
        filePointer = 0;

        // tao file "scanner.txt" de gui cong viec cho tien trinh scanner
        successCheck = Create("scanner.txt");
        if (successCheck == -1)
        {
            Close(passengerFileId);
            Up("main");
            return;
        }

        // Mo file "scanner.txt" 
        scannerFileId = Open("scanner.txt", 0);
        if (scannerFileId == -1)
        {
            Close(scannerFileId);
            Up("main");
            return;
        }

        // Doc va ghi tung so hanh ly cua tung hanh khach vao file "scanner.txt" 
        // Yeu cau scanner xu li cho tung hanh khach
        while (filePointer < fileLength - 1)
        {
            
            jumpToScanner = 0;
            // doc va ghi so luong hanh ly cua tung hanh khach
            Read(&currentChar, 1, passengerFileId);
            if (currentChar != ' ')
            {
                Write(&currentChar, 1, scannerFileId);
            }
            else
            {
                // bao hieu da doc va ghi xong so luong hanh ly cua 1 hanh khach
                jumpToScanner = 1;
            }
            
            // truong hop da doc toi hanh khach cuoi cung
            if (filePointer == fileLength - 2)
            {
                Write("*", 1, scannerFileId); // danh dau ket thuc 1 thoi diem
                jumpToScanner = 1;      // bao hieu da doc xong so luong hanh ly cua 1 hanh khach
            }

            // Xu li khi da doc va ghi xong 1 hanh khach
            if (jumpToScanner == 1)
            {
                Close(scannerFileId);   // dong file truoc khi danh thuc tien trinh Scanner
                
                Up("scanner");   // danh thuc tien trinh Scanner de xu li hanh khach vua ghi vao file "scanner.txt"

                
                Down("passenger");   // cho tien trinh Scanner xu li hoat tat

                // tao lai file "scanner.txt"
                successCheck = Create("scanner.txt");
                if (successCheck == -1)
                {
                    Close(passengerFileId);
                    Up("main");
                    return;
                }

                // moi lai file "scanner.txt"
                scannerFileId = Open("scanner.txt", 0);
                if (scannerFileId == -1)
                {
                    Close(passengerFileId);
                    Up("main");
                    return;
                }
            }
            filePointer++;
        }

        // tro ve main de ghi ket qua cho moi hanh khach
        Up("main");
    }
}
