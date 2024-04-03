#include "syscall.h"
#include "copyright.h"
int arr[100];
int st[300];

int main()
{
    int n;               // so phan tu cua mang
    int stEnd;           // vi tri ket thuc cua stack(hoac kich thuoc hien thoi cua phan stack dang duoc su dung)
    OpenFileID fileID;   // bien luu id cua file quicksort.txt


    // cac bien cho thuat toan quicksort 
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
        // lay cac gia tri start end cho mot lan chay quicksort
        start = st[stEnd - 2];
        end = st[stEnd - 1];
        stEnd = stEnd - 2;  // lui stEnd ve vi tri dau tien khong duoc su dung

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

        if (i < end){  // nap them 1 lan chay quicksort cho doan i->end vao stack
            st[stEnd] = i;
            st[stEnd + 1] = end;
            stEnd += 2;
        }
        if (start < j){ // nap them 1 lan chay quicksort cho doan start->j vao stack
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

    // kiem tra viec mo file co thanh cong khong
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
    
    Close(fileID); // dong file

    return 0;
}