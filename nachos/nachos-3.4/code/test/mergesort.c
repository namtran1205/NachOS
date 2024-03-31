#include "syscall.h"
#include "copyright.h"


void merge(int arr[], int begin, int mid, int end)
{
    int leftSize = mid - begin + 1;
    int rightSize = end - mid;
    int *leftArr = (int*) maloc(sizeof(int) * leftSize);
    int *rightArr = (int*) maloc(sizeof(int) * rightSize);

    for(int i = 0; i < leftSize; i++) leftArr[i] = arr[begin + i];
    for(int i = 0; i < rightSize; i++) rightArr[i] = arr[mid + 1 + i];

    int itL = 0, itR = 0;
    while (itL < leftSize && itR < rightSize)
    {
        if (leftArr[itL] <= rightArr[itR]) 
        {
            arr[begin + itL + itR] = leftArr[itL];
            itL++;
        }
        else 
        {
            arr[begin + itL + itR] = leftArr[itR];
            itR++;
        }
    }

    while (itR < rightSize) arr[begin + itL + itR] = leftArr[itR++];
    while (itL < leftSize) arr[begin + itL + itR] = leftArr[itL++];
}

void mergeSort(int arr[], int begin, int end)
{
    if (begin >= end) return;
    int mid = (begin + end) >> 1;
    mergeSort(arr, begin, mid);
    mergeSort(arr, mid + 1, end);
    merge(arr, begin, mid, end);
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
    int n, arr[100];

    PrintString("Nhap so phan tu cua mang:" );


    // kiem tra so phan tu co hop le khong
    n = ReadInt();
    if (n < 0) 
    {
        PrintString("So phan tu khong hop le!\n");
        return 0;
    }

    for(int i = 0; i < n; i++) 
    {
        PrintString("Nhap phan tu thu ");
        PrintInt(i);
        PrintChar(':');
        arr[i] = ReadInt();
    }

    mergeSort(arr, 0, n - 1);

    Create("mergesort.txt");
    int fileID = Open("mergesort.txt", 0);

    if (fileID == -1)
    {
        PrintString("Ghi ket qua that bai!\n");
        return 0;      
    }
    // ghi ket qua sap xep vao quicksort.txt
    // cau truc:
    // dong 1: n
    // dong 2: arr[0] arr[1] ... arr[n - 1]
    filePrintInt(n, fileID);
    Write("\n", 1, fileID);
    for(int i = 0; i < n; i++)
    {
        filePrintInt(arr[i], fileID);
        Write(" ", 1, fileID);
    }

    return 0;
}