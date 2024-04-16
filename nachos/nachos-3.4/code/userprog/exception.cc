// exception.cc 
//	Entry point into the Nachos kernel from user programs.
//	There are two kinds of things that can cause control to
//	transfer back to here from user code:
//
//	syscall -- The user code explicitly requests to call a procedure
//	in the Nachos kernel.  Right now, the only function we support is
//	"Halt".
//
//	exceptions -- The user code does something that the CPU can't handle.
//	For instance, accessing memory that doesn't exist, arithmetic errors,
//	etc.  
//
//	Interrupts (which can also cause control to transfer from user
//	code into the Nachos kernel) are handled elsewhere.
//
// For now, this only handles the Halt() system call.
// Everything else core dumps.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "system.h"
#include "syscall.h"
#include "machine.h"
//----------------------------------------------------------------------
// ExceptionHandler
// 	Entry point into the Nachos kernel.  Called when a user program
//	is executing, and either does a syscall, or generates an addressing
//	or arithmetic exception.
//
// 	For system calls, the following is the calling convention:
//
// 	system call code -- r2
//		arg1 -- r4
//		arg2 -- r5
//		arg3 -- r6
//		arg4 -- r7
//
//	The result of the system call, if any, must be put back into r2. 
//
// And don't forget to increment the pc before returning. (Or else you'll
// loop making the same system call forever!
//
//	"which" is the kind of exception.  The list of possible exceptions 
//	are in machine.h.
//----------------------------------------------------------------------

// Change the Program Counter register back by 4 bytes to continue fetching instructions
void IncreasePC()
{
    int counter = machine->ReadRegister(PCReg);
    machine->WriteRegister(PrevPCReg, counter);
    counter = machine->ReadRegister(NextPCReg);
    machine->WriteRegister(PCReg, counter);
    machine->WriteRegister(NextPCReg, counter + 4);
}


/* Input: User's memory space (int) - buffer limit (int)
 * Output: Buffer memory (char)
 * Function: Copy the User's memory space to the System's memory space
 */
char* User2System(int virtAddr,int limit) 
{ 
    int i;// index 
    int oneChar; 
    char* kernelBuf = NULL; 
    kernelBuf = new char[limit +1];//need for terminal string 
    if (kernelBuf == NULL) 
    return kernelBuf; 
    memset(kernelBuf,0,limit+1); 
    //printf("\n Filename u2s:"); 
    for (i = 0 ; i < limit ;i++) 
    { 
        machine->ReadMem(virtAddr+i,1,&oneChar); 
        kernelBuf[i] = (char)oneChar; 
        //printf("%c",kernelBuf[i]); 
        if (oneChar == 0) 
            break; 
    } 
    return kernelBuf; 
}

int System2User(int virtAddr,int len,char* buffer)
{
    if (len < 0) return -1;
    if (len == 0)return len;
    int i = 0;
    int oneChar = 0 ;

    do {
        oneChar= (int) buffer[i];
        machine->WriteMem(virtAddr+i,1,oneChar);
        i ++;
    } while(i < len && oneChar != 0);
    
    return i;
}

void ExceptionHandler(ExceptionType which)
{
    int type = machine->ReadRegister(2);
    switch (which)
    {
        case NoException:
            return;
        case PageFaultException:
	    	DEBUG('a', "Page fault is happening.\n");
	    	interrupt->Halt();
            break;
        case ReadOnlyException:
	    	DEBUG('a', "Page fault is happening.\n");
    
            interrupt->Halt();
            break;
        case BusErrorException:
            DEBUG('a', "Bus Error is happening.\n");
            interrupt->Halt();
            break;

        case AddressErrorException:
            DEBUG('a', "Address Error is happening.\n");
            interrupt->Halt();
            break;

        case OverflowException:
            DEBUG('a', "Overflow is happening.\n");
            interrupt->Halt();
            break;

        case IllegalInstrException: 
            DEBUG('a', "Illegal Instruction is happening.\n");
            interrupt->Halt();
            break;

        case NumExceptionTypes:

        case SyscallException:
        {
            switch(type)
            {
                case SC_Halt:
	    			DEBUG('a', "Shutdown, initiated by user program.\n");
	    			interrupt->Halt();
                    break;
                case SC_Create: 
                {
                    int address; // Biến lưu địa chỉ của chuỗi tên tập tin
                    char* filename; // Con trỏ trỏ đến tên tập tin
                    DEBUG('a',"\n SC_Create call ..."); // Ghi log: Gọi hàm SC_Create
                    DEBUG('a',"\n Reading virtual address of filename"); // Ghi log: Đọc địa chỉ ảo của tên tập tin
                    // Lấy tham số tên tập tin từ thanh ghi r4
                    address = machine->ReadRegister(4); // Gán địa chỉ từ thanh ghi r4 vào biến address
                    DEBUG ('a',"\n Reading filename."); // Ghi log: Đọc tên tập tin
                    // MaxFileLength là = 32
                    int MaxFileLength = 32;
                    filename = User2System(address, MaxFileLength+1);  // Lấy tên tập tin từ không gian người dùng và gán vào biến filename
                    if (filename == NULL) // Kiểm tra xem có đủ bộ nhớ hay không
                    { 
                        printf("\n Not enough memory in system"); // In ra thông báo khi không đủ bộ nhớ
                        DEBUG('a',"\n Not enough memory in system"); // Ghi log: Không đủ bộ nhớ trong hệ thống
                        machine->WriteRegister(2,-1); // Trả về lỗi cho chương trình người dùng
                        delete filename; // Giải phóng bộ nhớ đã cấp phát cho biến filename
                        return; // Kết thúc hàm
                    } 
                    DEBUG('a',"\n Finish reading filename."); // Ghi log: Đã đọc xong tên tập tin
                    // Create file with size = 0 
                    // Dùng đối tượng fileSystem của lớp OpenFile để tạo file,  
                    // việc tạo file này là sử dụng các thủ tục tạo file của hệ điều  
                    // hành Linux, chúng ta không quản lý trực tiếp các block trên 
                    // đĩa cứng cấp phát cho file, việc quản lý các block của file  
                    // trên ổ đĩa là một đồ án khác 
                    if (!fileSystem->Create(filename,0)) // Tạo file mới với kích thước = 0, nếu không thành công
                    { 
                        printf("\n Error create file '%s'",filename); // In ra thông báo lỗi khi không tạo được file
                        machine->WriteRegister(2,-1); // Trả về lỗi cho chương trình người dùng
                        delete filename; // Giải phóng bộ nhớ đã cấp phát cho biến filename
                        return; // Kết thúc hàm
                    } 
                    machine->WriteRegister(2,0); // Trả về cho chương trình người dùng thành công
                    printf("Successfully created file\n"); // In ra thông báo tạo file thành công
                    delete filename; // Giải phóng bộ nhớ đã cấp phát cho biến filename
                    IncreasePC(); // Tăng giá trị của thanh ghi PC
                    return; // Kết thúc hàm
                }
                case SC_ReadInt:
                {
                    char* buffer; // Khai báo con trỏ buffer để lưu dữ liệu đầu vào từ người dùng
                    int MAX_BUFFER = 255; // Đặt giới hạn tối đa cho buffer
                    buffer = new char[MAX_BUFFER + 1]; // Cấp phát bộ nhớ động cho buffer với kích thước MAX_BUFFER + 1 để lưu kết thúc chuỗi
                    int numbytes = gSynchConsole->Read(buffer, MAX_BUFFER); // Đọc dữ liệu từ người dùng vào buffer, trả về số lượng ký tự đã đọc
                    int number = 0; // Biến lưu trữ số nguyên cuối cùng

                    // Quá trình chuyển đổi từ buffer sang số nguyên int

                    // Xác định dấu của số
                    bool isNegative = false; // Giả sử số là dương
                    int firstNumIndex = 0;
                    int lastNumIndex = 0;
                    if(buffer[0] == '-') // Nếu ký tự đầu tiên là dấu "-"
                    {
                        isNegative = true; // Đánh dấu số là âm
                        firstNumIndex = 1; // Bắt đầu từ ký tự thứ hai
                        lastNumIndex = 1;                        			   		
                    }
                    
                    // Kiểm tra tính hợp lệ của buffer
                    for(int i = firstNumIndex; i < numbytes; i++)					
                    {
                        if(buffer[i] == '.') // Nếu có dấu chấm thì kiểm tra xem phần thập phân có chứa số khác 0 không
                        {
                            int j = i + 1;
                            for(; j < numbytes; j++)
                            {
                                // Trong trường hợp số không hợp lệ
                                if(buffer[j] != '0')
                                {
                                    printf("\n\n The integer number is not valid");
                                    DEBUG('a', "\n The integer number is not valid");
                                    machine->WriteRegister(2, 0);
                                    IncreasePC();
                                    delete buffer;
                                    return;
                                }
                            }
                        
                            lastNumIndex = i - 1;				
                            break;                           
                        }
                        else if(buffer[i] < '0' && buffer[i] > '9') // Kiểm tra xem có ký tự nào không phải số không
                        {
                            printf("\n\n The integer number is not valid");
                            DEBUG('a', "\n The integer number is not valid");
                            machine->WriteRegister(2, 0);
                            IncreasePC();
                            delete buffer;
                            return;
                        }
                        lastNumIndex = i;    
                    }			
                    
                    // Nếu hợp lệ, chuyển chuỗi về số nguyên
                    for(int i = firstNumIndex; i<= lastNumIndex; i++)
                    {
                        number = number * 10 + (int)(buffer[i] - 48); // Chuyển đổi từ ký tự số ASCII sang giá trị số nguyên
                    }
                    
                    // Trường hợp số âm
                    if(isNegative)
                    {
                        number = number * -1; // Đảo dấu nếu số âm
                    }
                    machine->WriteRegister(2, number); // Ghi số nguyên vào thanh ghi 2
                    IncreasePC(); // Tăng giá trị của thanh ghi PC
                    delete buffer; // Giải phóng bộ nhớ đã cấp phát cho buffer
                    return;	
                }
                case SC_PrintInt:
                {
                    int number = machine->ReadRegister(4); // Đọc số nguyên từ thanh ghi r4
                    if(number == 0) // Kiểm tra nếu số là 0
                    {
                        gSynchConsole->Write("0", 1); // In ra màn hình số 0
                        IncreasePC(); // Tăng giá trị của thanh ghi PC
                        return; // Kết thúc hàm
                    }

                    // Chuyển số thành chuỗi để in ra màn hình
                    bool isNegative = false; // Biến xác định dấu của số
                    int numberOfNum = 0; // Số lượng chữ số
                    int firstNumIndex = 0; // Chỉ số của chữ số đầu tiên

                    if(number < 0) // Nếu số là số âm
                    {
                        isNegative = true; // Đánh dấu là số âm
                        number = number * -1; // Đảo dấu số
                        firstNumIndex = 1; // Bắt đầu từ vị trí thứ 1 trong chuỗi
                    } 

                    int t_number = number;
                    while(t_number) // Đếm số lượng chữ số
                    {
                        numberOfNum++;
                        t_number /= 10;
                    }

                    // Tạo buffer để lưu chuỗi kết quả
                    char* buffer;
                    int MAX_BUFFER = 255;
                    buffer = new char[MAX_BUFFER + 1];
                    for(int i = firstNumIndex + numberOfNum - 1; i >= firstNumIndex; i--) // Bắt đầu từ chữ số cuối cùng
                    {
                        buffer[i] = (char)((number % 10) + 48); // Chuyển từ số nguyên sang ký tự ASCII
                        number /= 10; // Loại bỏ chữ số đã xử lý
                    }

                    // Nếu là số âm
                    if(isNegative)
                    {
                        buffer[0] = '-'; // Gắn dấu trừ vào đầu chuỗi
                        buffer[numberOfNum + 1] = 0; // Kết thúc chuỗi
                        gSynchConsole->Write(buffer, numberOfNum + 1); // In chuỗi ra màn hình
                        delete buffer; // Giải phóng bộ nhớ đã cấp phát cho buffer
                        IncreasePC(); // Tăng giá trị của thanh ghi PC
                        return; // Kết thúc hàm
                    }
                    buffer[numberOfNum] = 0; // Kết thúc chuỗi
                    gSynchConsole->Write(buffer, numberOfNum); // In chuỗi ra màn hình
                    delete buffer; // Giải phóng bộ nhớ đã cấp phát cho buffer
                    IncreasePC(); // Tăng giá trị của thanh ghi PC
                    return; // Kết thúc hàm

                }
                case SC_ReadFloat:
                {
                    float *result;
                    char buffer[255 + 2];
                    memset(buffer, 0, sizeof(buffer));
                    //gSynchConsole->Write("Type a float number: ", strlen("Type a float number: ") + 1);
                    int len = gSynchConsole->Read(buffer, 255 + 1);

                    if (len != 0)
                    {
                        bool isFloat = true;
                        bool isNegative = (buffer[0] == '-');
                        for (int i = isNegative; i < len; ++i)
                        {
                            char c = buffer[i];
                            if (c != '.' && (c < '0' || c > '9'))
                            {
                                DEBUG('a', "Expected float but %s found\n", buffer);
                                gSynchConsole->Write("Expected float but ", strlen("Expected float but ") + 1);
                                gSynchConsole->Write(buffer, strlen(buffer) + 1);
                                gSynchConsole->Write(" found\n", strlen(" found\n") + 1);
                                isFloat = false;
                                break;
                            }
                        }

                        result = new float;
                        if (isFloat)
                        {
                            *result = atof(buffer);
                        }
                        else
                        {
                            *result = 0.0f;
                        }
                    }

                    machine->WriteRegister(2, (int)result);
                    IncreasePC();
                    return;
                }
                case SC_PrintFloat:
                {
                    float* number = (float *)machine->ReadRegister(4); // Read value of number PARAMETER from register 4
                    char* buffer = new char[255];
                    sprintf(buffer, "%f", *number);
                    //gSynchConsole->Write("The float number you typed: ", strlen("The float number you typed: ") + 1);
                    gSynchConsole->Write(buffer, strlen(buffer) + 1); // Print number to console
                    delete buffer;
                    IncreasePC();
                    return;
                }
                case SC_FreeFloat:
                {
                    float *number = (float *)machine->ReadRegister(4);
                    if (number)
                    {
                        delete number;
                    }
                    IncreasePC();
                    return;
                }
                case SC_CompareFPs:
                {
                    // Get the values of two float pointers from registers to compare them
                    float *f1 = (float *)machine->ReadRegister(4);
                    float *f2 = (float *)machine->ReadRegister(5);
                    int result;

                    if      (*f1  < *f2) result = -1;
                    else if (*f1 == *f2) result = 0;
                    else if (*f1  > *f2) result = 1;

                    // Write the comparing result into register 2 and increase program counter
                    machine->WriteRegister(2, result);
                    IncreasePC();
                    return;
                }
                case SC_ReadChar:
                {
                    // Input: Không có
                    // Output: Duy nhất 1 ký tự (char)
                    // Công dụng: Đọc một ký tự từ người dùng nhập vào
                    int maxBytes = 255; // Đặt số lượng ký tự tối đa có thể nhập vào là 255
                    char* buffer = new char[255]; // Cấp phát bộ nhớ động cho buffer để lưu ký tự nhập vào từ người dùng
                    //gSynchConsole->Write("Type a character: ", strlen("Type a character: ") + 1);
                    int numBytes = gSynchConsole->Read(buffer, maxBytes); // Đọc dữ liệu từ người dùng vào buffer, trả về số lượng ký tự đã đọc

                    if(numBytes > 1) // Nếu người dùng nhập nhiều hơn 1 ký tự
                    {
                        printf("Chỉ được nhập duy nhất 1 ký tự!"); // In ra màn hình thông báo lỗi
                        DEBUG('a', "\nERROR: Chỉ được nhập duy nhất 1 ký tự!"); // Ghi log: Lỗi - Chỉ được nhập duy nhất 1 ký tự
                        machine->WriteRegister(2, 0); // Ghi 0 vào thanh ghi R2 để báo lỗi
                    }
                    else if(numBytes == 0) // Nếu ký tự nhập vào rỗng
                    {
                        printf("Ký tự rỗng!"); // In ra màn hình thông báo lỗi
                        DEBUG('a', "\nERROR: Ký tự rỗng!"); // Ghi log: Lỗi - Ký tự rỗng
                        machine->WriteRegister(2, 0); // Ghi 0 vào thanh ghi R2 để báo lỗi
                    }
                    else
                    {
                        // Chuỗi vừa lấy có đúng 1 ký tự, lấy ký tự ở index = 0, ghi vào thanh ghi R2
                        char c = buffer[0];
                        machine->WriteRegister(2, c);
                    }

                    delete buffer; // Giải phóng bộ nhớ đã cấp phát cho buffer
                    IncreasePC(); // Tăng giá trị của thanh ghi PC
                    return; // Kết thúc hàm

                }
                case SC_PrintChar:
                {
                    char temp = (char)machine->ReadRegister(4); // Đọc ký tự từ thanh ghi r4 và gán vào biến temp

                    //gSynchConsole->Write("The character you typed: ", strlen("The character you typed: ") + 1);
                    // Ghi chuỗi "The character you typed: " ra màn hình
                    gSynchConsole->Write(&temp, 1); // Ghi ký tự temp ra màn hình
                    IncreasePC(); // Tăng giá trị của thanh ghi PC
                    return; // Kết thúc hàm

                }
                case SC_ReadString:
                {
                    int address, length; // Khai báo biến address và length để lưu địa chỉ và độ dài của chuỗi
                    char* buffer; // Khai báo con trỏ buffer để lưu chuỗi

                    address = machine->ReadRegister(4); // Đọc địa chỉ tham số buffer từ thanh ghi r4
                    length = machine->ReadRegister(5); // Đọc độ dài tối đa của chuỗi từ thanh ghi r5

                    buffer = User2System(address, length); // Sao chép chuỗi từ không gian người dùng sang không gian hệ thống

                    if (buffer != NULL) { // Nếu việc cấp phát bộ nhớ cho buffer thành công
                        //gSynchConsole->Write("Type a string: ", strlen("Type a string: ") + 1);
                        // Yêu cầu người dùng nhập chuỗi và lưu vào buffer
                        gSynchConsole->Read(buffer, length); // Sử dụng hàm Read của SynchConsole để đọc chuỗi

                        // Sao chép chuỗi từ không gian hệ thống sang không gian người dùng
                        System2User(address, length, buffer);

                        delete[] buffer; // Giải phóng bộ nhớ đã cấp phát cho buffer
                    } else {
                        DEBUG('a', "\nError: Failed to allocate memory for buffer."); // Ghi log: Lỗi - Không thể cấp phát bộ nhớ cho buffer
                    }

                    IncreasePC(); // Tăng giá trị của thanh ghi PC
                    return; // Kết thúc hàm

                }
                case SC_PrintString:
                {
                    int address = machine->ReadRegister(4); // Lấy địa chỉ tham số buffer từ thanh ghi r4
                    const int BUFFER_SIZE = 255; // Đặt kích thước tối đa của buffer là 255 ký tự
                    char* buffer = User2System(address, BUFFER_SIZE); // Sao chép chuỗi từ không gian người dùng sang không gian hệ thống với một buffer có kích thước là 255 ký tự
                    int length = 0; // Khởi tạo biến length để lưu độ dài thực sự của chuỗi
                    while (*(buffer + length) != '\0') // Xác định độ dài thực sự của chuỗi
                        length++;
                    //gSynchConsole->Write("The string you typed: ", strlen("The string you typed: ") + 1);
                    // Ghi chuỗi "The string you typed: " ra màn hình
                    gSynchConsole->Write(buffer, length + 1); // Sử dụng hàm Write của SynchConsole để in chuỗi
                    delete[] buffer; // Giải phóng bộ nhớ đã cấp phát cho buffer
                    IncreasePC(); // Tăng giá trị của thanh ghi PC
                    return; // Kết thúc hàm
                }
                case SC_Open:
                {
                    int address = machine->ReadRegister(4); // Lấy địa chỉ tham số name từ thanh ghi số 4
                    int type = machine->ReadRegister(5); // Lấy tham số type từ thanh ghi số 5
                    char* filename; // Khai báo con trỏ filename để lưu tên tập tin
                    filename = User2System(address, 32); // Sao chép chuỗi từ không gian người dùng sang không gian hệ thống với buffer có kích thước là 32 ký tự

                    // Kiểm tra xem hệ điều hành có thể mở được tập tin không
                    int freeSlot = fileSystem->FindFreeSlot(); // Tìm một slot trống trong bảng fileSystem->openf
                    if (freeSlot != -1) // Chỉ xử lí khi còn slot trống
                    {
                        if (type == 0 || type == 1) // Chỉ xử lí khi type là 0 hoặc 1
                        {
                            // Mở tập tin với tên là filename và type là type
                            if ((fileSystem->CheckopenF[freeSlot] = fileSystem->Open(filename, type)) != NULL) // Mở tập tin thành công
                            {
                                machine->WriteRegister(2, freeSlot); // Trả về OpenFileID
                            }
                        }
                        else if (type == 2) // Xử lí trường hợp stdin với type được quy ước là 2
                        {
                            machine->WriteRegister(2, 0); // Trả về OpenFileID
                        }
                        else // Xử lí trường hợp stdout với type được quy ước là 3
                        {
                            machine->WriteRegister(2, 1); // Trả về OpenFileID
                        }
                        delete[] filename; // Giải phóng bộ nhớ đã cấp phát cho filename
                        IncreasePC(); // Tăng giá trị của thanh ghi PC
                        return; // Kết thúc hàm
                    }

                    machine->WriteRegister(2, -1); // Không mở được tập tin, trả về -1
                    delete[] filename; // Giải phóng bộ nhớ đã cấp phát cho filename
                    IncreasePC(); // Tăng giá trị của thanh ghi PC
                    return; // Kết thúc hàm
                }
                case SC_Read:
                {
                    int address = machine->ReadRegister(4); // Lấy địa chỉ tham số buffer từ thanh ghi số 4
                    int charcount = machine->ReadRegister(5); // Lấy giá trị charcount từ thanh ghi số 5
                    int id = machine->ReadRegister(6); // Lấy id của file từ thanh ghi số 6
                    int OldPos; // Khai báo biến OldPos để lưu vị trí hiện tại của con trỏ đọc file
                    int NewPos; // Khai báo biến NewPos để lưu vị trí mới của con trỏ đọc file
                    char *buf; // Khai báo con trỏ buf để lưu dữ liệu đọc được từ file

                    // Kiểm tra xem id của file có nằm ngoài bảng mô tả file không
                    if (id < 0 || id > 14)
                    {
                        printf("\nKhông thể đọc vì id nằm ngoài bảng mô tả file.");
                        machine->WriteRegister(2, -1); // Trả về -1 để báo lỗi
                        IncreasePC(); // Tăng giá trị của thanh ghi PC
                        return; // Kết thúc hàm
                    }

                    // Kiểm tra xem file có tồn tại không
                    if (fileSystem->CheckopenF[id] == NULL)
                    {
                        printf("\nKhông thể đọc vì file này không tồn tại.");
                        machine->WriteRegister(2, -1); // Trả về -1 để báo lỗi
                        IncreasePC(); // Tăng giá trị của thanh ghi PC
                        return; // Kết thúc hàm
                    }

                    // Xét trường hợp đọc file stdout (type được quy ước là 3) thì trả về -1
                    if (fileSystem->CheckopenF[id]->type == 3)
                    {
                        printf("\nKhông thể đọc file stdout.");
                        machine->WriteRegister(2, -1); // Trả về -1 để báo lỗi
                        IncreasePC(); // Tăng giá trị của thanh ghi PC
                        return; // Kết thúc hàm
                    }

                    OldPos = fileSystem->CheckopenF[id]->GetCurrentPos(); // Lấy vị trí hiện tại của con trỏ đọc file

                    buf = User2System(address, charcount); // Sao chép chuỗi từ không gian người dùng sang không gian hệ thống với buffer có kích thước là charcount

                    // Xét trường hợp đọc file stdin (type được quy ước là 2)
                    if (fileSystem->CheckopenF[id]->type == 2)
                    {
                        // Sử dụng hàm Read của lớp SynchConsole để đọc dữ liệu từ bàn phím và trả về số byte thực sự đọc được
                        int size = gSynchConsole->Read(buf, charcount); 
                        // Sao chép chuỗi từ không gian hệ thống sang không gian người dùng với buffer có kích thước là số byte thực sự đọc được
                        System2User(address, size, buf); 
                        machine->WriteRegister(2, size); // Trả về số byte thực sự đọc được
                        delete buf; // Giải phóng bộ nhớ đã cấp phát cho buf
                        IncreasePC(); // Tăng giá trị của thanh ghi PC
                        return; // Kết thúc hàm
                    }

                    // Xét trường hợp đọc file bình thường
                    if ((fileSystem->CheckopenF[id]->Read(buf, charcount)) > 0)
                    {
                        // Số byte thực sự đọc được là NewPos - OldPos
                        NewPos = fileSystem->CheckopenF[id]->GetCurrentPos();
                        // Sao chép chuỗi từ không gian hệ thống sang không gian người dùng với buffer có kích thước là số byte thực sự đọc được
                        System2User(address, NewPos - OldPos, buf); 
                        machine->WriteRegister(2, NewPos - OldPos); // Trả về số byte thực sự đọc được
                    }
                    else
                    {
                        // Trường hợp còn lại là đọc file có nội dung là NULL, trả về -2
                        //printf("\nĐọc file rỗng.");
                        machine->WriteRegister(2, -2); // Trả về -2 để báo lỗi
                    }

                    delete buf; // Giải phóng bộ nhớ đã cấp phát cho buf
                    IncreasePC(); // Tăng giá trị của thanh ghi PC
                    return; // Kết thúc hàm
                }
                case SC_Write:
                {
                    int address = machine->ReadRegister(4); // Lấy địa chỉ tham số buffer từ thanh ghi số 4
                    int charcount = machine->ReadRegister(5); // Lấy giá trị charcount từ thanh ghi số 5
                    int id = machine->ReadRegister(6); // Lấy id của file từ thanh ghi số 6
                    int OldPos; // Khai báo biến OldPos để lưu vị trí hiện tại của con trỏ đọc file
                    int NewPos; // Khai báo biến NewPos để lưu vị trí mới của con trỏ đọc file
                    char *buf; // Khai báo con trỏ buf để lưu dữ liệu cần ghi vào file

                    // Kiểm tra xem id của file có nằm ngoài phạm vi bảng mô tả file không
                    if (id < 0 || id > 14)
                    {
                        printf("\nCannot writing."); // In thông báo lỗi
                        machine->WriteRegister(2, -1); // Trả về -1 để báo lỗi
                        IncreasePC(); // Tăng giá trị của thanh ghi PC
                        return; // Kết thúc hàm
                    }

                    // Kiểm tra xem file có tồn tại không
                    if (fileSystem->CheckopenF[id] == NULL)
                    {
                        printf("\nCannot write."); // In thông báo lỗi
                        machine->WriteRegister(2, -1); // Trả về -1 để báo lỗi
                        IncreasePC(); // Tăng giá trị của thanh ghi PC
                        return; // Kết thúc hàm
                    }

                    // Xét trường hợp ghi vào file chỉ có quyền đọc (type quy ước là 1) hoặc file stdin (type quy ước là 2), trả về -1
                    if (fileSystem->CheckopenF[id]->type == 1 || fileSystem->CheckopenF[id]->type == 2)
                    {
                        printf("\nCannot write."); // In thông báo lỗi
                        machine->WriteRegister(2, -1); // Trả về -1 để báo lỗi
                        IncreasePC(); // Tăng giá trị của thanh ghi PC
                        return; // Kết thúc hàm
                    }

                    OldPos = fileSystem->CheckopenF[id]->GetCurrentPos(); // Lấy vị trí hiện tại của con trỏ đọc file

                    buf = User2System(address, charcount); // Sao chép chuỗi từ không gian người dùng sang không gian hệ thống với buffer có kích thước là charcount

                    // Xét trường hợp ghi vào file có quyền đọc và ghi (type quy ước là 0), trả về số byte thực sự ghi được
                    if (fileSystem->CheckopenF[id]->type == 0)
                    {
                        if ((fileSystem->CheckopenF[id]->Write(buf, charcount)) > 0)
                        {
                            NewPos = fileSystem->CheckopenF[id]->GetCurrentPos(); // Lấy vị trí mới của con trỏ đọc file
                            machine->WriteRegister(2, NewPos - OldPos); // Trả về số byte thực sự ghi được
                            delete buf; // Giải phóng bộ nhớ đã cấp phát cho buf
                            IncreasePC(); // Tăng giá trị của thanh ghi PC
                            return; // Kết thúc hàm
                        }
                    }

                    // Xét trường hợp ghi vào file stdout (type quy ước là 3)
                    if (fileSystem->CheckopenF[id]->type == 3)
                    {
                        int i = 0;
                        while (buf[i] != 0 && buf[i] != '\n') // Vòng lặp ghi cho đến khi gặp ký tự '\n'
                        {
                            gSynchConsole->Write(buf + i, 1); // Sử dụng hàm Write của lớp SynchConsole để ghi ký tự
                            i++;
                        }
                        
                        buf[i] = '\n'; // Ghi ký tự '\n' vào cuối chuỗi
                        gSynchConsole->Write(buf + i, 1); // Ghi ký tự '\n'
                        machine->WriteRegister(2, i - 1); // Trả về số byte thực sự ghi được
                        delete buf; // Giải phóng bộ nhớ đã cấp phát cho buf
                        IncreasePC(); // Tăng giá trị của thanh ghi PC
                        return; // Kết thúc hàm
                    }

                }
                case SC_Close:
                {
                    int fileID = machine->ReadRegister(4); // Lấy id của file từ thanh ghi số 4

                    if (fileID >= 0 && fileID <= 14) // Chỉ xử lí khi fileID nằm trong khoảng [0, 14]
                    {
                        if (fileSystem->CheckopenF[fileID]) // Nếu file đã mở thành công
                        {
                            delete fileSystem->CheckopenF[fileID]; // Xóa vùng nhớ lưu trữ file
                            fileSystem->CheckopenF[fileID] = NULL; // Gán vùng nhớ là NULL để đánh dấu file đã đóng
                            machine->WriteRegister(2, 0); // Ghi 0 vào thanh ghi 2 để báo hiệu việc đóng file thành công
                            break; // Thoát khỏi vòng lặp
                        }
                    }

                    machine->WriteRegister(2, -1); // Nếu không thực hiện được việc đóng file, ghi -1 vào thanh ghi 2
                    IncreasePC(); // Tăng giá trị của thanh ghi PC
                    return; // Kết thúc hàm
                }
                case SC_WriteInt:
                {
                    int number = machine->ReadRegister(4); // Lấy giá trị cần ghi (tham số number) từ thanh ghi số 4
                    OpenFileID id = machine->ReadRegister(5); // Lấy id của file từ thanh ghi số 5
                    int OldPos;
                    int NewPos;

                    char *buffer;
                    int startPoint = 0; // Vị trí bắt đầu của dãy số
                    int numberOfNum = 1; // Biến để lưu số chữ số của number
                    int pw = 1; // Số lớn nhất mà number chia lấy nguyên > 0
                    int MAX_BUFFER = 255;
                    buffer = new char[MAX_BUFFER + 1];

                    // Kiểm tra xem id của file truyền vào có nằm ngoài bảng mô tả file không
                    if (id < 0 || id > 14)
                    {
                        printf("\nCannot writing."); // In thông báo lỗi
                        machine->WriteRegister(2, -1); // Ghi -1 vào thanh ghi 2 để báo lỗi
                        IncreasePC(); // Tăng giá trị của thanh ghi PC
                        return; // Kết thúc hàm
                    }
                    // Kiểm tra xem file có tồn tại không
                    if (fileSystem->CheckopenF[id] == NULL)
                    {
                        printf("\nCannot write."); // In thông báo lỗi
                        machine->WriteRegister(2, -1); // Ghi -1 vào thanh ghi 2 để báo lỗi
                        IncreasePC(); // Tăng giá trị của thanh ghi PC
                        return; // Kết thúc hàm
                    }
                    // Xét trường hợp ghi vào file chỉ đọc (type quy ước là 1) hoặc file stdin (type quy ước là 2), trả về -1
                    if (fileSystem->CheckopenF[id]->type == 1 || fileSystem->CheckopenF[id]->type == 2)
                    {
                        printf("\nCannot write."); // In thông báo lỗi
                        machine->WriteRegister(2, -1); // Ghi -1 vào thanh ghi 2 để báo lỗi
                        IncreasePC(); // Tăng giá trị của thanh ghi PC
                        return; // Kết thúc hàm
                    }

                    // Xử lý trường hợp số âm
                    if(number < 0)
                    {
                        startPoint = 1;       // Dịch vị trí bắt đầu lên 1 để ghi '-' vào vị trí 0
                        number = number * -1; // Chuyển số âm thành số dương để tính số chữ số
                    } 

                    // Tính số lượng chữ số của số number
                    while(number / pw >= 10)
                    {
                        numberOfNum++;
                        pw *= 10;
                    }

                    // Tạo buffer chuỗi để in ra màn hình
                    for(int offset = 0; offset < numberOfNum; offset++)
                    {
                        buffer[startPoint + offset] = (char)((number / pw) + 48);
                        number %= pw;
                        pw /= 10;
                    }
                    if(startPoint > 0) buffer[0] = '-'; // Nếu số âm, ghi '-' vào vị trí đầu tiên của buffer
                    buffer[startPoint + numberOfNum] = '\0'; // Kết thúc chuỗi buffer

                    OldPos = fileSystem->CheckopenF[id]->GetCurrentPos(); // Kiểm tra thành công thì lấy vị trí OldPos

                    // Xét trường hợp ghi vào file đọc và ghi (type quy ước là 0), trả về số byte thực sự ghi được
                    if (fileSystem->CheckopenF[id]->type == 0)
                    {
                        if ((fileSystem->CheckopenF[id]->Write(buffer, startPoint + numberOfNum)) > 0)
                        {
                            // Lấy vị trí mới của con trỏ đọc file
                            NewPos = fileSystem->CheckopenF[id]->GetCurrentPos();
                            // Kiểm tra xem số lượng byte ghi vào có vượt quá số lượng byte thực sự?
                            if (startPoint + numberOfNum > NewPos - OldPos)
                                machine->WriteRegister(2, -1); // Nếu vượt quá, ghi -1 vào thanh ghi 2
                            else 
                                machine->WriteRegister(2, 0); // Nếu không, ghi 0 vào thanh ghi 2
                            delete buffer; // Giải phóng bộ nhớ đã cấp phát cho buffer
                            IncreasePC(); // Tăng giá trị của thanh ghi PC
                            return; // Kết thúc hàm
                        }
                    }

                    // Xét trường hợp ghi vào file stdout (type quy ước là 3)
                    if (fileSystem->CheckopenF[id]->type == 3)
                    {
                        gSynchConsole->Write(buffer, startPoint + numberOfNum); // Sử dụng hàm Write của lớp SynchConsole để ghi chuỗi buffer
                        machine->WriteRegister(2, 0); // Ghi 0 vào thanh ghi 2
                        delete buffer; // Giải phóng bộ nhớ đã cấp phát cho buffer
                        IncreasePC(); // Tăng giá trị của thanh ghi PC
                        return; // Kết thúc hàm
                    }
                    machine->WriteRegister(2, -1); // Nếu không thực hiện được việc ghi vào file, ghi -1 vào thanh ghi 2
                    delete buffer;
                    IncreasePC();
                    return;
                }
                case SC_WriteFloat:
                {
                    float* number = (float*)machine->ReadRegister(4);
                    OpenFileID id = machine->ReadRegister(5); // Lay id cua file tu thanh ghi so 5
                    int OldPos;
                    int NewPos;


                    char* buffer = new char[255];
                    sprintf(buffer, "%f", *number);


                    // Kiểm tra ID truyền vào
                    if (id < 0 || id > 14)
                    {
                        printf("\nCannot writing.");
                        machine->WriteRegister(2, -1);
                        IncreasePC();
                        return;
                    }
                    //Kiểm tra sự tồn tại của file
                    if (fileSystem->CheckopenF[id] == NULL)
                    {
                        printf("\nCannot write.");
                        machine->WriteRegister(2, -1);
                        IncreasePC();
                        return;
                    }
                    // Xet truong hop ghi file only read (type quy uoc la 1) hoac file stdin (type quy uoc la 2) thi tra ve -1
                    if (fileSystem->CheckopenF[id]->type == 1 || fileSystem->CheckopenF[id]->type == 2)
                    {
                        printf("\nCannot write.");
                        machine->WriteRegister(2, -1);
                        IncreasePC();
                        return;
                    }
                    
                    OldPos = fileSystem->CheckopenF[id]->GetCurrentPos(); // Kiem tra thanh cong thi lay vi tri OldPos
                    // Xet truong hop ghi file read & write (type quy uoc la 0) thi tra ve so byte thuc su
                    if (fileSystem->CheckopenF[id]->type == 0)
                    {
                        if ((fileSystem->CheckopenF[id]->Write(buffer, strlen(buffer))) > 0)
                        {
                            // So byte thuc su = NewPos - OldPos
                            NewPos = fileSystem->CheckopenF[id]->GetCurrentPos();
                            if (strlen(buffer) > NewPos - OldPos)
                                machine->WriteRegister(2, -1);
                            else 
                                machine->WriteRegister(2, 0);
                            delete buffer;
                            IncreasePC();
                            return;
                        }
                    }
                    if (fileSystem->CheckopenF[id]->type == 3) // Xet truong hop con lai ghi file stdout (type quy uoc la 3)
                    {
                        gSynchConsole->Write(buffer, strlen(buffer)); // Su dung ham Write cua lop SynchConsole 
                        machine->WriteRegister(2, 0); 
                        delete buffer;
                        IncreasePC();
                        return;
                    }
                    machine->WriteRegister(2, -1);
                    delete buffer;
                    IncreasePC();
                    return;
                }
                
                case SC_Join:
                {
                    // Đọc ID từ thanh ghi 4 của máy
                    int userId = machine->ReadRegister(4);
                    // Thực hiện hoạt động cập nhật tham gia bằng cách sử dụng ID đã thu được
                    int joinResult = pTab->JoinUpdate(userId);

                    // Ghi kết quả của hoạt động tham gia vào thanh ghi 2
                    machine->WriteRegister(2, joinResult);

                    // Tăng bộ đếm chương trình để di chuyển đến lệnh tiếp theo
                    IncreasePC();
                    return;

                }
                case SC_Exec:
                {
                    // Input: vi tri int
			        // Output: Fail return -1, Success: return id cua thread dang chay
			        // SpaceId Exec(char *name);
			        int virtAddr;
			        virtAddr = machine->ReadRegister(4);	// doc dia chi ten chuong trinh tu thanh ghi r4
			        char* name;
			        name = User2System(virtAddr, 255 + 1); // Lay ten chuong trinh, nap vao kernel
                    gSynchConsole->Write(name, strlen(name) + 1);
                    gSynchConsole->Write("\n", strlen("\n") + 1);
			        if(name == NULL)
			        {
			        	DEBUG('a', "\n Not enough memory in System");
			        	printf("\n Not enough memory in System");
			        	machine->WriteRegister(2, -1);
			        	IncreasePC();
			        	return;
			        }
			        OpenFile *oFile = fileSystem->Open(name);
			        if (oFile == NULL)
			        {
			        	printf("\nExec:: Can't open this file.");
                        printf("exception.cc SC_EXEC open file ???? \n");
			        	machine->WriteRegister(2,-1);
			        	IncreasePC();
			        	return;
			        }

			        delete oFile;

			        // Return child process id
			        int id = pTab->ExecUpdate(name); 
			        machine->WriteRegister(2,id);

			        delete[] name;	
			        IncreasePC();
			        return;
                }
                case SC_Exit:
                {
                    // Đọc trạng thái thoát từ thanh ghi
                    int exitStatus = machine->ReadRegister(4);

                    // Kiểm tra nếu trạng thái thoát khác không
                    if (exitStatus != 0) {
                        IncreasePC();
                        return;
                    }

                    // Cập nhật bảng tiến trình với trạng thái thoát
                    int result = pTab->ExitUpdate(exitStatus);

                    // Giải phóng tài nguyên liên kết với luồng hiện tại
                    currentThread->FreeSpace();

                    // Kết thúc việc thực thi của luồng hiện tại
                    currentThread->Finish();

                    // Di chuyển đến lệnh tiếp theo
                    IncreasePC();
                    return;
                }
                
                
                case SC_CreateSemaphore:
                {
                    // Đọc tham số từ các thanh ghi máy
                    int virtAddr = machine->ReadRegister(4);
                    int semval = machine->ReadRegister(5);

                    // Chuyển địa chỉ ảo thành không gian hệ thống cho tên semaphore
                    char *name = User2System(virtAddr, 255);

                    // Kiểm tra nếu không đủ bộ nhớ để cấp phát cho biến name
                    if (name == NULL) {
                        DEBUG('a', "\n Not enough memory in the system");
                        printf("\n Not enough memory in the system");
                        machine->WriteRegister(2, -1);
                        IncreasePC();
                        return;
                    }

                    // Tạo semaphore với tên và giá trị khởi tạo được chỉ định
                    int result = semTab->Create(name, semval);

                    // Xử lý khi không thể tạo semaphore
                    if (result == -1) {
                        DEBUG('a', "\n Cannot create semaphore");
                        printf("\n Cannot create semaphore");
                        machine->WriteRegister(2, -1);
                        delete[] name;
                        IncreasePC();
                        return;
                    }

                    // Dọn dẹp bộ nhớ đã cấp phát và ghi kết quả vào thanh ghi 2
                    delete[] name;
                    machine->WriteRegister(2, result);
                    IncreasePC();
                    return;

                }
                case SC_Up:
                {
                    // Đọc địa chỉ ảo từ thanh ghi 4 của máy
                    int virtualAddress = machine->ReadRegister(4);

                    int semaphoreValue = machine->ReadRegister(5);

                    // Chuyển đổi địa chỉ ảo thành tên semaphore trong hệ thống
                    char *semaphoreName = User2System(virtualAddress, 255);

                    // Kiểm tra xem việc chuyển đổi có thành công không
                    if (semaphoreName == NULL) {
                        // Báo lỗi nếu không đủ bộ nhớ để cấp phát
                        DEBUG('a', "\n Not enough memory in the system");
                        printf("\n Not enough memory in the system");
                        machine->WriteRegister(2, -1);
                        IncreasePC();
                        return;
                    }

                    // Gọi hàm Signal của SemaphoreTable để thực hiện phát tín hiệu cho semaphore
                    int result = semTab->Signal(semaphoreName);

                    // Xử lý khi không tìm thấy semaphore có tên chỉ định
                    if (result == -1) {
                        DEBUG('a', "\n This semaphore does not exist!");
                        printf("\n This semaphore does not exist!");
                        machine->WriteRegister(2, -1);
                        delete[] semaphoreName;
                        IncreasePC();
                        return;
                    }

                    // Ghi kết quả vào thanh ghi 2 của máy và tăng chương trình tiếp tục thực thi
                    delete[] semaphoreName;
                    machine->WriteRegister(2, result);
                    IncreasePC();
                    return;
                }
                case SC_Down:
                {
                    // Đọc địa chỉ ảo từ thanh ghi 4 của máy
                    int virtualAddress = machine->ReadRegister(4);

                    int semaphoreValue = machine->ReadRegister(5);

                    // Chuyển đổi địa chỉ ảo thành tên semaphore trong hệ thống
                    char *semaphoreName = User2System(virtualAddress, 255);

                    // Kiểm tra xem việc chuyển đổi có thành công không
                    if (semaphoreName == NULL) {
                        // Báo lỗi nếu không đủ bộ nhớ để cấp phát
                        DEBUG('a', "\n Not enough memory in the system");
                        printf("\n Not enough memory in the system");
                        machine->WriteRegister(2, -1);
                        IncreasePC();
                        return;
                    }

                    // Gọi hàm Wait của SemaphoreTable để thực hiện chờ đợi cho semaphore
                    int result = semTab->Wait(semaphoreName);

                    // Xử lý khi không tìm thấy semaphore có tên chỉ định
                    if (result == -1) {
                        DEBUG('a', "\n This semaphore does not exist!");
                        printf("\n This semaphore does not exist!");
                        machine->WriteRegister(2, -1);
                        delete[] semaphoreName;
                        IncreasePC();
                        return;
                    }

                    // Ghi kết quả vào thanh ghi 2 của máy và tăng chương trình tiếp tục thực thi
                    delete[] semaphoreName;
                    machine->WriteRegister(2, result);
                    IncreasePC();
                    return;
                }
                
                default:
                    break;
            }
            IncreasePC();
        }
    }
}