#ifndef STABLE_H
#define STABLE_H

#include "synch.h"
#include "bitmap.h"
#include "sem.h"

#define MAX_SEMAPHORE 10

class Stable {
public:
    // khởi tạo size đối tượng Sem để quản lý 10 Semaphore. Gán giá trị ban đầu là null
    // nhớ khởi tạo bm để sử dụng
    Stable();
    ~Stable();                              // hủy các đối tượng đã tạo
    int Create(char* name, int init);       // Kiểm tra Semaphore “name” chưa tồn tại thì tạo Semaphore mới. Ngược lại, báo lỗi.       

    int Wait(char* name);                   // Nếu tồn tại Semaphore “name” thì gọi this->P()để thực thi. Ngược lại, báo lỗi.

    int Signal(char* name);                 // Nếu tồn tại Semaphore “name” thì gọi this->V()để thực thi. Ngược lại, báo lỗi.

    int FindFreeSlot();               // Tìm slot trống.    

private:
    BitMap* bm;                             // quản lý slot trống
    Sem* semTab[MAX_SEMAPHORE];       // quản lý tối đa 10 đối tượng Sem
};

#endif // STABLE_H