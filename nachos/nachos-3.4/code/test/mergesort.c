#include "syscall.h"
#include "copyright.h"

int main()
{
    int swapTmp;
    int i;  
    int n;
    float* arr[100];
    int st[400], stEnd;
    OpenFileID fileID; 


    int begin, end, mid;
    int itL, leftSize, itR, rightSize;
    float* leftArr[100], *rightArr[100];
    // ==========================================================
    //                        nhap du lieu
    // ==========================================================

    PrintString("Nhap so phan tu cua mang:" );

    // kiem tra so phan tu co hop le khong
    n = ReadInt();
    if (n < 0) 
    {
        PrintString("So phan tu khong hop le!\n");
        return 0;
    }

    for(i = 0; i < n; i++) 
    {
        // bao hieu nguoi dung nhap phan tu i
        PrintString("Nhap phan tu thu ");
        PrintInt(i);
        PrintChar(':');
        arr[i] = ReadFloat();
    }


    // ==========================================================
    //                        mergesort
    // ==========================================================


    // tam dinh nghia 3 phan tu lien tiep trong mang st duoc xem nhu 1 block
    // 1 block la du lieu de thuc hien cac cong viec
    // [begin, end, -1]  : sap xep hai phan begin->mid va mid + 1->end
    // [begin, end, mid] : tron hai phan begin->mid va mid + 1->end de tao thanh day tang dan
    st[0] = 0;
    st[1] = n - 1;
    st[2] = -1;
    stEnd = 3;
    while (stEnd > 0)
    {
        begin = st[stEnd - 3];
        end = st[stEnd - 2];
        mid = st[stEnd - 1];
        stEnd -= 3;
        if (mid == -1) // truong hop [begin, end, -1]
        {
            if (begin >= end) continue;

            mid = (begin + end) / 2;

            st[stEnd] = begin;
            st[stEnd + 1] = end;
            st[stEnd + 2] = mid;

            st[stEnd + 3] = begin;
            st[stEnd + 4] = mid;
            st[stEnd + 5] = -1;

            st[stEnd + 6] = mid + 1;
            st[stEnd + 7] = end;
            st[stEnd + 8] = -1;
            stEnd += 9;
        }
        else // truong hop [begin, end, mid]
        {
            leftSize = mid - begin + 1;
            rightSize = end - mid;

            for(i = 0; i < leftSize; i++) leftArr[i] = arr[begin + i];
            for(i = 0; i < rightSize; i++) rightArr[i] = arr[mid + 1 + i];

            itL = itR = 0;
            while (itL < leftSize && itR < rightSize)
            {
                if (CompareFPs(leftArr[itL], rightArr[itR]) <= 0) 
                {
                    arr[begin + itL + itR] = leftArr[itL];
                    itL++;
                }
                else 
                {
                    arr[begin + itL + itR] = rightArr[itR];
                    itR++;
                }
            }


            while (itR < rightSize) 
            {
                arr[begin + itL + itR] = rightArr[itR];
                itR++;
            }

            while (itL < leftSize) 
            {
                arr[begin + itL + itR] = leftArr[itL];
                itL++;
            }
        }
    }





    // ==========================================================
    //                  xuat du lieu sang file
    // ==========================================================

    Create("mergesort.txt");
    fileID = Open("mergesort.txt", 0);

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
        WriteFloat(arr[i], fileID);
        Write(" ", 1, fileID);
    }

    for(i = 0; i < n; i++) FreeFloat(arr[i]);

    return 0;
}