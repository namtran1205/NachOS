#include "syscall.h"
#include "copyright.h"
int arr[10];
int st[30];

int main()
{
    int n;
    int stEnd;
    OpenFileID fileID;   


    // quicksort variable 
    int swapTmp;
    int i, j;  
    int start, end;
    int pivot;



    // ==========================================================
    //                        nhap du lieu
    // ==========================================================
    
    // nhap so phan tu cua mang
    PrintString("Nhap so phan tu:" );
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
        PrintInt(i);
        PrintChar(':');

        // doc phan tu i
        arr[i] = ReadInt();

    }
    // ==========================================================
    //                        quicksort
    // ==========================================================


    st[0] = 0; st[1] = n - 1; // tao du lieu cho lan chay dau tien
    stEnd = 2;
    while (stEnd > 0)
    {
        start = st[stEnd - 2];
        end = st[stEnd - 1];
        stEnd = stEnd - 2;

        // partition part
        pivot = arr[(start + end)/2];
        i = start; j = end;
        while (i < j)
        {
            while (arr[i] < pivot) i++;
            while (arr[j] > pivot) j--;
            
            if (i <= j){
                swapTmp = arr[i];
                arr[i] = arr[j];
                arr[j] = swapTmp;
                i++;
                j--;
            }
        }
        // create 2 new block in stack for start->j and i -> end
        if (i < end){
            st[stEnd] = i;
            st[stEnd + 1] = end;
            stEnd += 2;
        }
        if (start < j){
            st[stEnd] = start;
            st[stEnd + 1] = j;
            stEnd += 2;  
        }


    }


    // ==========================================================
    //                  xuat du lieu sang file
    // ==========================================================


    // tao va mo file quicksort.txt
    Create("quicksort.txt");
    fileID = Open("quicksort.txt", 0);

    if (fileID == -1)
    {
        PrintString("Ghi ket qua that bai!\n");
        return 0;      
    }


    // ghi ket qua sap xep vao quicksort.txt
    // cau truc:
    // dong 1: n
    // dong 2: arr[0] arr[1] ... arr[n - 1]
    WriteInt(n, fileID);
    Write("\n", 1, fileID);
    for(i = 0; i < n; i++)
    {
        WriteInt(arr[i], fileID);
        Write(" ", 1, fileID);
    }
    Close(fileID);

    return 0;
}