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
                case SC_Create: 
                {
                    int virtAddr; 
                    char* filename; 
                    DEBUG('a',"\n SC_Create call ..."); 
                    DEBUG('a',"\n Reading virtual address of filename"); 
                    // Lấy tham số tên tập tin từ thanh ghi r4 
                    virtAddr = machine->ReadRegister(4); 
                    DEBUG ('a',"\n Reading filename."); 
                    // MaxFileLength là = 32 
                    int MaxFileLength = 32;
                    filename = User2System(virtAddr, MaxFileLength+1);  
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

                case SC_PrintChar:

                case SC_ReadString:

                case SC_PrintString:

                case SC_Open:

                case SC_Read:

                case SC_Write:

                case SC_Close:

            }
        }

        if ((which == SyscallException) && (type == SC_Halt)) {
	        DEBUG('a', "Shutdown, initiated by user program.\n");
   	        interrupt->Halt();
        } else {
	        printf("Unexpected user mode exception %d %d\n", which, type);
	        ASSERT(FALSE);
        }
    }
}