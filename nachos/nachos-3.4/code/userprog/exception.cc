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
void AdvanceProgramCounter()
{
    int pcAfter = registers[NextPCReg] + 4;
     registers[PrevPCReg] = registers[PCReg];	// for debugging, in case we
						// are jumping into lala-land
    registers[PCReg] = registers[NextPCReg];
    registers[NextPCReg] = pcAfter;
}

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
                case SC_CreateFile: 
                {
                    int address; 
                    char* filename; 
                    DEBUG('a',"\n SC_Create call ..."); 
                    DEBUG('a',"\n Reading virtual address of filename"); 
                    // Lấy tham số tên tập tin từ thanh ghi r4 
                    address = machine->ReadRegister(4); 
                    DEBUG ('a',"\n Reading filename."); 
                    // MaxFileLength là = 32 
                    int MaxFileLength = 32;
                    filename = User2System(address, MaxFileLength+1);  
                    if (filename == NULL) 
                    { 
                        printf("\n Not enough memory in system"); 
                        DEBUG('a',"\n Not enough memory in system"); 
                        machine->WriteRegister(2,-1); // trả về lỗi cho chương          
                    // trình người dùng 
                        delete filename; 
                        return; 
                    } 
                    DEBUG('a',"\n Finish reading filename."); 
                    //DEBUG(‘a’,"\n File name : '"<<filename<<"'"); 
                    // Create file with size = 0 
                    // Dùng đối tượng fileSystem của lớp OpenFile để tạo file,  
                    // việc tạo file này là sử dụng các  thủ tục tạo file của hệ điều  
                    // hành Linux, chúng ta không quản ly trực tiếp các block trên 
                    // đĩa cứng cấp phát cho file, việc quản ly các block của file  
                    // trên ổ đĩa là một đồ án khác 

                    if (!fileSystem->Create(filename,0)) 
                    { 
                        printf("\n Error create file '%s'",filename); 
                        machine->WriteRegister(2,-1); 
                        delete filename; 
                        return; 
                    } 
                    AdvanceProgramCounter();
                    machine->WriteRegister(2,0); // trả về cho chương trình  
                        // người dùng thành công 
                    delete filename; 

                    interrupt->Halt();
                    break; 
    
                }
                /* 
                case SC_Sub:
                {                
                    AdvanceProgramCounter();
                    interrupt->Halt();
                        break;
                }*/
                case SC_ReadInt:
                {
                    const int MAX_BUFFER = 255;
                    char* buffer = new char[MAX_BUFFER]; // Allocate memory for input buffer
                
                    int numBytes = gSynchConsole->Read(buffer, MAX_BUFFER); // Read input from console
                
                    int number = 0; // Initialize variable to store the integer
                    bool isNegative = false; // Flag to indicate if the number is negative
                    int firstNumIndex = 0; // Index to start parsing the number
                
                    // Check if the number is negative
                    if (buffer[0] == '-') {
                        isNegative = true;
                        firstNumIndex = 1;
                    }
                
                    // Parse each character in the buffer to extract the integer
                    for (int i = firstNumIndex; i < numBytes; i++) {
                        // Check if the character is a digit
                        if (buffer[i] < '0' || buffer[i] > '9') {
                            // If the character is not a digit, print error message and return
                            printf("\n\n The integer number is not valid.");
                            DEBUG('a', "\n The integer number is not valid.");
                            machine->WriteRegister(2, 0); // Return 0 in register 2
                            IncreasePC();
                            delete[] buffer; // Free allocated memory
                            return;
                        }
                        // Convert character to integer and calculate the final number
                        number = number * 10 + (int)(buffer[i] - '0');
                    }
                
                    // Adjust the sign of the number if it is negative
                    if (isNegative) {
                        number *= -1;
                    }
                
                    // Write the final number to register 2
                    machine->WriteRegister(2, number);
                    IncreasePC();
                    delete[] buffer; // Free allocated memory
                }
                case SC_PrintInt:
                {
                    // Read the integer to print from register 4
                    int number = machine->ReadRegister(4);
                
                    // If the number is 0, print "0" and return
                    if (number == 0) {
                        gSynchConsole->Write("0", 1);
                        IncreasePC();
                        return;
                    }
                
                    bool isNegative = false; // Flag to indicate if the number is negative
                    int numberOfDigits = 0; // Variable to store the number of digits in the number
                    int firstNumIndex = 0; // Index to start printing the number from
                
                    // Check if the number is negative
                    if (number < 0) {
                        isNegative = true;
                        number *= -1; // Make the number positive
                        firstNumIndex = 1; // Skip the negative sign while printing
                    }
                
                    // Count the number of digits in the number
                    int tempNumber = number;
                    while (tempNumber) {
                        numberOfDigits++;
                        tempNumber /= 10;
                    }
                
                    char* buffer; // Buffer to store the string representation of the number
                    const int MAX_BUFFER = 255;
                    buffer = new char[MAX_BUFFER + 1]; // Allocate memory for buffer
                
                    // Convert the number to a string
                    for (int i = firstNumIndex + numberOfDigits - 1; i >= firstNumIndex; i--) {
                        buffer[i] = (char)((number % 10) + '0'); // Convert digit to character
                        number /= 10; // Move to the next digit
                    }
                
                    // Add negative sign if necessary
                    if (isNegative) {
                        buffer[0] = '-'; // Add negative sign at the beginning
                        buffer[numberOfDigits + 1] = '\0'; // Null-terminate the string
                        gSynchConsole->Write(buffer, numberOfDigits + 1); // Write the string to console
                        delete[] buffer; // Free allocated memory
                        IncreasePC();
                        return;
                    }
                
                    buffer[numberOfDigits] = '\0'; // Null-terminate the string
                    gSynchConsole->Write(buffer, numberOfDigits); // Write the string to console
                    delete[] buffer; // Free allocated memory
                    IncreasePC();
                    return;
                }
                case SC_ReadChar:
                {
                    const int MAX_BYTES = 255;
                    char buffer[MAX_BYTES];
                    int numBytes = gSynchConsole->Read(buffer, MAX_BYTES);

                    if (numBytes > 1) {
                        DEBUG('a', "\nError: Please enter no more than 1 character");
                        machine->WriteRegister(2, 0);
                    } else if (numBytes == 0) {
                        DEBUG('a', "\nError: No character is founded");
                        machine->WriteRegister(2, 0);
                    } else {
                        // Take the character at index 0 from the retrieved string, and return it to register R2
                        char c = buffer[0];
                        machine->WriteRegister(2, c);
                    }

                    // Release dynamically allocated memory
                    //delete[] buffer; // Use delete[] instead of delete for arrays
                    delete[] buffer;
                    //IncreasePC(); // Error system
                    //return;
                    break;
                }
                case SC_PrintChar:
                {
                    char temp[] = (char)machine->ReadRegister(4);
                    gSynchConsole->Write(&temp, 1);
                    break;
                }
                case SC_ReadString:
                {
                    int address, length;
                    char* buffer;
                    address = machine->ReadRegister(4); // Read the buffer parameter address from register 4
                    length = machine->ReadRegister(5); // Read the maximum length of the input string from register 5
                    buffer = User2System(address, length); // Copy string from User Space to System Space
                    if (buffer != nullptr) {
                        gSynchConsole->Read(buffer, length); // Use SynchConsole's Read function to read the string
                        System2User(address, length, buffer); // Copy string from System Space to User Space
                        delete[] buffer; // Free allocated memory
                    } else {
                        DEBUG('a', "\nError: Failed to allocate memory for buffer.");
                    }
                    break;
                }
                case SC_PrintString:
                {
                    int address = machine->ReadRegister(4); // Get the buffer parameter address from register 4
                    const int BUFFER_SIZE = 255;
                    char* buffer = User2System(address, BUFFER_SIZE); // Copy string from User Space to System Space with a buffer of 255 characters
                    int length = 0;
                    while (*(buffer + length) != '\0') // Determine the actual length of the string
                        length++;
                    gSynchConsole->Write(buffer, length + 1); // Use SynchConsole's Write function to print the string
                    delete[] buffer; // Free allocated memory
                    //IncreasePC(); // Increment Program Counter
                    //return;
                    break;
                }
                case SC_Open:
                {
                    int address = machine->ReadRegister(4); // Lay dia chi cua tham so name tu thanh ghi so 4
                    int type = machine->ReadRegister(5); // Lay tham so type tu thanh ghi so 5
                    char* filename;
                    filename = User2System(address, 32); // Copy chuoi tu vung nho User Space sang System Space voi bo dem name dai MaxFileLength
                    //Kiem tra xem OS con mo dc file khong
                    
                    // update 4/1/2018
                    int freeSlot = fileSystem->FindFreeSlot();
                    if (freeSlot != -1) //Chi xu li khi con slot trong
                    {
                        if (type == 0 || type == 1) //chi xu li khi type = 0 hoac 1
                        {
                            
                            if ((fileSystem->openf[freeSlot] = fileSystem->Open(filename, type)) != NULL) //Mo file thanh cong
                            {
                                machine->WriteRegister(2, freeSlot); //tra ve OpenFileID
                            }
                        }
                        else if (type == 2) // xu li stdin voi type quy uoc la 2
                        {
                            machine->WriteRegister(2, 0); //tra ve OpenFileID
                        }
                        else // xu li stdout voi type quy uoc la 3
                        {
                            machine->WriteRegister(2, 1); //tra ve OpenFileID
                        }
                        delete[] filename;
                        break;
                    }
                    machine->WriteRegister(2, -1); //Khong mo duoc file return -1
                    
                    delete[] filename;
                    break;
                }
                case SC_Read:
                {
                    int address = machine->ReadRegister(4); // Lay dia chi cua tham so buffer tu thanh ghi so 4
                    int charcount = machine->ReadRegister(5); // Lay charcount tu thanh ghi so 5
                    int id = machine->ReadRegister(6); // Lay id cua file tu thanh ghi so 6 
                    int OldPos;
                    int NewPos;
                    char *buf;
                    // Kiem tra id cua file truyen vao co nam ngoai bang mo ta file khong
                    if (id < 0 || id > 14)
                    {
                        printf("\nKhong the read vi id nam ngoai bang mo ta file.");
                        machine->WriteRegister(2, -1);
                        IncreasePC();
                        return;
                    }
                    // Kiem tra file co ton tai khong
                    if (fileSystem->openf[id] == NULL)
                    {
                        printf("\nKhong the read vi file nay khong ton tai.");
                        machine->WriteRegister(2, -1);
                        IncreasePC();
                        return;
                    }
                    if (fileSystem->openf[id]->type == 3) // Xet truong hop doc file stdout (type quy uoc la 3) thi tra ve -1
                    {
                        printf("\nKhong the read file stdout.");
                        machine->WriteRegister(2, -1);
                        IncreasePC();
                        return;
                    }
                    OldPos = fileSystem->openf[id]->GetCurrentPos(); // Kiem tra thanh cong thi lay vi tri OldPos
                    buf = User2System(address, charcount); // Copy chuoi tu vung nho User Space sang System Space voi bo dem buffer dai charcount
                    // Xet truong hop doc file stdin (type quy uoc la 2)
                    if (fileSystem->openf[id]->type == 2)
                    {
                        // Su dung ham Read cua lop SynchConsole de tra ve so byte thuc su doc duoc
                        int size = gSynchConsole->Read(buf, charcount); 
                        System2User(address, size, buf); // Copy chuoi tu vung nho System Space sang User Space voi bo dem buffer co do dai la so byte thuc su
                        machine->WriteRegister(2, size); // Tra ve so byte thuc su doc duoc
                        delete buf;
                        IncreasePC();
                        return;
                    }
                    // Xet truong hop doc file binh thuong thi tra ve so byte thuc su
                    if ((fileSystem->openf[id]->Read(buf, charcount)) > 0)
                    {
                        // So byte thuc su = NewPos - OldPos
                        NewPos = fileSystem->openf[id]->GetCurrentPos();
                        // Copy chuoi tu vung nho System Space sang User Space voi bo dem buffer co do dai la so byte thuc su 
                        System2User(address, NewPos - OldPos, buf); 
                        machine->WriteRegister(2, NewPos - OldPos);
                    }
                    else
                    {
                        // Truong hop con lai la doc file co noi dung la NULL tra ve -2
                        //printf("\nDoc file rong.");
                        machine->WriteRegister(2, -2);
                    }
                    delete buf;
                    IncreasePC();
                    return;
                }
                case SC_Write:
                {
                    int address = machine->ReadRegister(4); // Lay dia chi cua tham so buffer tu thanh ghi so 4
                    int charcount = machine->ReadRegister(5); // Lay charcount tu thanh ghi so 5
                    int id = machine->ReadRegister(6); // Lay id cua file tu thanh ghi so 6
                    int OldPos;
                    int NewPos;
                    char *buf;
                    // Kiem tra id cua file truyen vao co nam ngoai bang mo ta file khong
                    if (id < 0 || id > 14)
                    {
                        printf("\nCannot writing.");
                        machine->WriteRegister(2, -1);
                        IncreasePC();
                        return;
                    }
                    // Kiem tra file co ton tai khong
                    if (fileSystem->openf[id] == NULL)
                    {
                        printf("\nCannot write.");
                        machine->WriteRegister(2, -1);
                        IncreasePC();
                        return;
                    }
                    // Xet truong hop ghi file only read (type quy uoc la 1) hoac file stdin (type quy uoc la 2) thi tra ve -1
                    if (fileSystem->openf[id]->type == 1 || fileSystem->openf[id]->type == 2)
                    {
                        printf("\nCannot write.");
                        machine->WriteRegister(2, -1);
                        IncreasePC();
                        return;
                    }
                    OldPos = fileSystem->openf[id]->GetCurrentPos(); // Kiem tra thanh cong thi lay vi tri OldPos
                    buf = User2System(address, charcount);  // Copy chuoi tu vung nho User Space sang System Space voi bo dem buffer dai charcount
                    // Xet truong hop ghi file read & write (type quy uoc la 0) thi tra ve so byte thuc su
                    if (fileSystem->openf[id]->type == 0)
                    {
                        if ((fileSystem->openf[id]->Write(buf, charcount)) > 0)
                        {
                            // So byte thuc su = NewPos - OldPos
                            NewPos = fileSystem->openf[id]->GetCurrentPos();
                            machine->WriteRegister(2, NewPos - OldPos);
                            delete buf;
                            IncreasePC();
                            return;
                        }
                    }
                    if (fileSystem->openf[id]->type == 3) // Xet truong hop con lai ghi file stdout (type quy uoc la 3)
                    {
                        int i = 0;
                        while (buf[i] != 0 && buf[i] != '\n') // Vong lap de write den khi gap ky tu '\n'
                        {
                            gSynchConsole->Write(buf + i, 1); // Su dung ham Write cua lop SynchConsole 
                            i++;
                        }
                        buf[i] = '\n';
                        gSynchConsole->Write(buf + i, 1); // Write ky tu '\n'
                        machine->WriteRegister(2, i - 1); // Tra ve so byte thuc su write duoc
                        delete buf;
                        IncreasePC();
                        return;
                    }
                }
                case SC_Close:
                {
                    int fileID = machine->ReadRegister(4); // Lay id cua file tu thanh ghi so 4
                    if (fileID >= 0 && fileID <= 14) //Chi xu li khi fid nam trong [0, 14]
                    {
                        if (fileSystem->openf[fileID]) //neu mo file thanh cong
                        {
                            delete fileSystem->openf[fileID]; //Xoa vung nho luu tru file
                            fileSystem->openf[fileID] = NULL; //Gan vung nho NULL
                            machine->WriteRegister(2, 0);
                            break;
                        }
                    }
                    machine->WriteRegister(2, -1);
                    break;
                }
                default:
                    break;
            }
            IncreasePC();
        }

        // if ((which == SyscallException) && (type == SC_Halt)) {
	    //     DEBUG('a', "Shutdown, initiated by user program.\n");
   	    //     interrupt->Halt();
        // } else {
	    //     printf("Unexpected user mode exception %d %d\n", which, type);
	    //     ASSERT(FALSE);
        // }
    }
}