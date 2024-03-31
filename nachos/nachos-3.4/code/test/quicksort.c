#include "syscall.h"
#include "copyright.h"

int int2char(int x, char str[])
{
    int startPoint = 0;
    if (x < 0)
    {
        startPoint++;
        x *= -1;
    }

    int pw = 1, cnt = 1;

    // tinh so chu so cua so
    while (x / pw > 9) 
    {
        pw *= 10;
        cnt++;
    }

    for(int i = 0; i < cnt; i++)
    {
        str[startPoint + i] = x / pw + 48;
        x %= pw;
        pw /= 10;
    }
    if (startPoint) str[0] = '-';
    str[cnt + startPoint] = '\0';
    return cnt + startPoint;
}


// y tuong:
// chuyen so dang int thanh chuoi char(qua ham int2char)
// ghi vao file bang ham ghi chuoi Write(buffer, size, fileID)
void filePrintInt(int x, OpenFileID fileID)
{
    char *num = (char*) maloc(sizeof(char) * 20);
    int len = int2char(x, num);
    Write(num, len, fileID);
    free(num);
}

int main()
{
    int swapTmp;
    int i, j;  
    int n, arr[100];
    int st[300], stEnd;
    OpenFileID fileID;   


    // quicksort variable 
    int start, end;
    int count;
    int pivot, pivotIndex;



    // ==========================================================
    //                        nhap du lieu
    // ==========================================================
    
    // nhap so phan tu cua mang
    PrintString("Nhap so phan tu cua mang:" );
    n = ReadInt();


    // kiem tra so phan tu co hop le khong
    if (n < 0) 
    {
        PrintString("So phan tu khong hop le!\n");
        return 0;
    }

    // nhap cac phan tu cua mang
    for(i = 0; i < n; i++) 
    {
        // bao hieu nguoi dung nhap phan tu i
        PrintString("Nhap phan tu thu ");
        PrintInt(i);
        PrintChar(':');

        // doc phan tu i
        arr[i] = ReadInt();
    }
    // ==========================================================
    //                        quicksort
    // ==========================================================

    st[0] = 0; st[1] = n - 1;
    stEnd = 2;
    while (stEnd != 0)
    {
        start = st[0];
        end = st[1];
        stEnd = stEnd - 2;

        if (start >= end) continue;

        // partition part
        {
            pivot = arr[start];

            count = 0;
            for (i = start + 1; i <= end; i++) {
                if (arr[i] <= pivot)
                    count++;
            }

            // Giving pivot element its correct position
            pivotIndex = start + count;

            // swap arr[pivotIndex] and arr[start]
            swapTmp = arr[pivotIndex];
            arr[pivotIndex] = arr[start];
            arr[start] = swapTmp;

            // Sorting left and right parts of the pivot element
            i = start, j = end;

            while (i < pivotIndex && j > pivotIndex) {

                while (arr[i] <= pivot) {
                    i++;
                }

                while (arr[j] > pivot) {
                    j--;
                }

                if (i < pivotIndex && j > pivotIndex) {
                    swapTmp = arr[i];
                    arr[i] = arr[j];
                    arr[j] = swapTmp;
                }
            }
        }
    
        // create 2 new block in stack for start->pivotIndex - 1 and pivotIndex + 1 -> end
        st[stEnd] = start;
        st[stEnd + 1] = pivotIndex - 1;
        st[stEnd + 2] = pivotIndex + 1;
        st[stEnd + 3] = end;
        stEnd = stEnd + 4;
    }


    // ==========================================================
    //                  xuat du lieu sang file
    // ==========================================================


    // tao va mo file quicksort.txt
    // Create("quicksort.txt");
    // fileID = Open("quicksort.txt", 0);

    // if (fileID == -1)
    // {
    //     PrintString("Ghi ket qua that bai!\n");
    //     return 0;      
    // }


    // // ghi ket qua sap xep vao quicksort.txt
    // // cau truc:
    // // dong 1: n
    // // dong 2: arr[0] arr[1] ... arr[n - 1]
    // WriteInt(n, fileID);
    // Write("\n", 1, fileID);
    // for(int i = 0; i < n; i++)
    // {
    //     WriteInt(n, fileID);
    //     Write(" ", 1, fileID);
    // }

    return 0;
}