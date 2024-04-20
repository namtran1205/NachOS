#ifndef PTABLE_H
#define PTABLE_H

#include "bitmap.h"
#include "pcb.h"
#include "synch.h"

#define MAXPROCESS 10

class PTable {
public:
    // khởi tạo size đối tượng PCB để lưu size process. Gán giá trị ban đầu là null
    // nhớ khởi tạo bm và bmsem để sử dụng
    PTable(int size);               
    ~PTable();                          // hủy các đối tượng đã tạo
    int ExecUpdate(char* name);         // Xử lý cho system call SC_Exec
    int ExitUpdate(int ec);             // Xử lý cho system call SC_Exit
    int JoinUpdate(int id);             // Xử lý cho system call SC_Join
    int GetFreeSlot();                  // tìm free slot để lưu thông tin cho tiến trình mới
    bool IsExist(int pid);              // kiểm tra tồn tại processID này không?
    void Remove(int pid);               // khi tiến trình kết thúc, delete processID ra khỏi mảng quảng lý của nó
    char* GetFileName(int id);          // Trả về tên chương trình

private:
    BitMap* bm;                          // đánh dấu các vị trí đã sử dụng trong pcb
    PCB* pcb[MAXPROCESS];
    int psize;
    Semaphore* bmsem;                   // dùng để ngăn chặn trường hợp nạp 2 tiến trình cùng một lúc
};

#endif // PTABLE_H