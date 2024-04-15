#ifndef SEM_H
#define SEM_H

#include "synch.h"

class Sem {
public:
    // khởi tạo đối tượng Sem. Gán giá trị ban đầu là null
    // nhớ khởi tạo bm sử dụng
    Sem(char* na, int i){
        strcpy(this->name,na);
        sem = new Semaphore(name,i);
    }
    ~Sem(){
        if(sem != NULL)
            delete sem;     // hủy các đối tượng đã tạo
    }

    void wait(){
        sem->Acquire();       // thực hiện thao tác chờ
    }
    void signal(){
        sem->Release();       // thực hiện thao tác giải phóng Semaphore
    }
    char* GetName(){    // Trả về tên của Semaphore
        return name;
    }
private:
    char name[50];      
    Semaphore* sem;     // Tạo semaphore để quản lý
};

#endif // SEM_H