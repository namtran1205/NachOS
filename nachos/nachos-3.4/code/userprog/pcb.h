#ifndef PCB_H
#define PCB_H

#include "thread.h"
#include "synch.h"

class PCB {
public:
    int parentID;
    char boolBG;

    PCB();
    PCB(int id = 0);
    ~PCB();

    // nạp chương trình có tên lưu trong biến filename và processID là pid
    int Exec(char* filename, int pid);      // Tạo 1 thread mới có tên là filename và process là pid
    int GetID();                            // Trả về ProcessID của tiến trình gọi thực hiện
    int GetNumWait();                       // Trả về số lượng tiến trình chờ

    void JoinWait();                        // 1. Tiến trình cha đợi tiến trình con kết thúc
    void ExitWait();                        // 4. Tiến trình con kết thúc
    void JoinRelease();                     // 2. Báo cho tiến trình cha thực thi tiếp
    void ExitRelease();                     // 3. Cho phép tiến trình con kết thúc

    void IncNumWait();                      // Tăng số tiến trình chờ
    void DecNumWait();                      // Giảm số tiến trình chờ

    void SetExitCode(int ec);               // Đặt exitcode của tiến trình
    int GetExitCode();                      // Trả về exitcode

    void SetFileName(char* fn);             // Đặt tên của tiến trình
    char* GetFileName();                    // Trả về tên của tiến trình

    char* GetNameThread();
    
private:
    Semaphore* joinsem;                     // semaphore cho quá trình join
    Semaphore* exitsem;                     // semaphore cho quá trình exit
    Semaphore* mutex;                      // semaphore cho quá trình truy xuất đọc quyền

    int exitcode;
    int numwait;                            // số tiến trình đã join
    int pid;
    int	JoinStatus;
    char processname[32];                   // Tên tiến trình

    Thread* thread;
};

void MyStartProcess(int pID);

#endif