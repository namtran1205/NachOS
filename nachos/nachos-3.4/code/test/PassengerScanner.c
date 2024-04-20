#include "syscall.h"
#include "copyright.h"

int main()
{
    int success;                                    // bien kiem tra co thanh cong khi mo, tao file
    SpaceId inputId, outputId, passengerId, resultId;// File ID
    int numTimeMoments;                             // so thoi diem can xu li
    char currentChar;                               // bien tam luu ki tu duoc doc tu file
    // Tao cac semaphore can thiet
    success = CreateSemaphore("main", 0);
    if (success == -1)
        return 1;
    success = CreateSemaphore("passenger", 0);
    if (success == -1)
        return 1;
    success = CreateSemaphore("scanner", 0);
    if (success == -1)
        return 1;
    success = CreateSemaphore("_scanner", 0);
    if (success == -1)
        return 1;
    
    // tao file "output.txt"
    success = Create("output.txt");
    if (success == -1)
        return 1;
    
    // mo file "input.txt" de doc du lieu can xu li
    inputId = Open("input.txt", 1);
    if (inputId == -1)
        return 1;
    
    //  mo file "output.txt" de xuat ket qua 
    outputId = Open("output.txt", 0);
    if (outputId == -1)
    {
        Close(inputId);
        return 1;
    }

    // doc so thoi diem can xu li
    numTimeMoments = 0;
    while (1)
    {
        Read(&currentChar, 1, inputId);
        if (currentChar != '\n')
        {
            if (currentChar >= '0' && currentChar <= '9')
                numTimeMoments = numTimeMoments * 10 + (currentChar - '0');
        }
        else
            break;
    }

    // Thuc thi tien trinh Passenger
    success = Exec("./test/Passenger");
    if (success == -1)
    {
        Close(inputId);
        Close(outputId);
        return 1;
    }

    // Thuc thi tien trinh Scanner
    success = Exec("./test/Scanner");
    if (success == -1)
    {
        Close(inputId);
        Close(outputId);
        return 1;
    }

    // Xu li tung thoi diem
    while (numTimeMoments--)
    {
        // tao file "passenger.txt" de giao tiep giua main va tien trinh passenger
        success = Create("passenger.txt");
        if (success == -1)
        {
            Close(inputId);
            Close(outputId);
            return 1;
        }
        
        // mo file "passenger.txt" de ghi day hanh ly trong thoi diem hien tai
        passengerId = Open("passenger.txt", 0);
        if (passengerId == -1)
        {
            Close(inputId);
            Close(outputId);
            return 1;
        }

        // doc chuoi hanh ly cua thoi diem hien tai tu "input.txt" va ghi vao "passenger.txt"
        while (1)
        {
            // truong hop ket thuc file
            if (Read(&currentChar, 1, inputId) < 1)
            {
                break;
            }

            // viet lien tiep cac ki tu da doc duoc vao "passenger.txt"
            if (currentChar != '\n')
            {
                Write(&currentChar, 1, passengerId);                
            }
            else //truong hop da het 1 thoi diem
                break;                        
        }
        Close(passengerId); 


        // cho chay tiep tien trinh Passenger de xu li chuoi hanh khach
        Up("passenger");
        // dung tien trinh main lai de cho tien trinh Passenger thuc hien xong cong viec
        Down("main");    
        // mo file "result.txt" de doc ket qua da duoc xu li tu tien trinh Passenger va Scanner
        resultId = Open("result.txt", 1);
        if (resultId == -1)
        {
            Close(inputId);
            Close(outputId);
            return 1;
        }

        // tien hanh doc va ghi ket qua tu file "result.txt" qua file "output.txt"
        while (1)
        {
            if (Read(&currentChar, 1, resultId)  < 1)
            {
                Write("\r\n", 2, outputId);
                Close(resultId);
                Up("_scanner");
                break;
            }
            Write(&currentChar, 1, outputId);
        }
    }
    Close(inputId);
    Close(outputId);
    return 0;    
}
