#include "pcb.h"
#include "utility.h"
#include "system.h"
#include "thread.h"
#include "addrspace.h"


char* file_name;

void MyStartProcess(int pID);


PCB::PCB()
{
	
}

PCB::PCB(int id)
{
	joinsem = new Semaphore("JoinSem",0);
	exitsem = new Semaphore("ExitSem",0);
	mutex = new Semaphore("Mutex",1);
	pid= id;
	exitcode= 0;
	numwait= 0;
	if(id)
		parentID= currentThread->processID;
	else
		parentID= 0;
	thread = NULL;
	JoinStatus= -1;

}

PCB::~PCB()
{
	if(joinsem != NULL)
		delete joinsem;
	if(exitsem != NULL)
		delete exitsem;
	if(mutex != NULL)
		delete mutex;
}

//------------------------------------------------------------------
int PCB::GetID()
{
	return pid;
}

int PCB::GetNumWait()
{
	return numwait;
}

int PCB::GetExitCode()
{
	return exitcode;	
}

void PCB::SetExitCode(int ec)
{
	exitcode = ec;
}

void PCB::IncNumWait()
{
	mutex->Acquire();
	numwait++;
	mutex->Release();
}

void PCB::DecNumWait()
{
	mutex->Acquire();
	if(numwait)
		numwait--;
	mutex->Release();
}

char* PCB::GetNameThread()
{
	if (thread == NULL) return NULL;
	return thread->getName();
}

//-------------------------------------------------------------------
void PCB::JoinWait()
{
	joinsem->Acquire();
	printf("pcb.cc - JoinWait line 88 joinsem->Acquire() successfully\n");
}

void PCB::JoinRelease()
{
	joinsem->Release();
}

void PCB::ExitWait()
{
	exitsem->Acquire();
}

void PCB::ExitRelease()
{
	exitsem->Release();
}

//------------------------------------------------------------------
int PCB::Exec(char *filename, int pID)
{
	mutex->Acquire();
	printf("pcb.cc line 111 filename : %s\n", filename);
	thread= new Thread(filename);
	if(thread == NULL)
	{
		printf("\nLoi: Khong tao duoc tien trinh moi !!!\n");
		mutex->Release();
		return -1;
	}
	thread->processID = pID;
	printf("pcb.cc line 120 pID = %d\n", pID);
	parentID = currentThread->processID;

	file_name = new char[255];
	for (int i = 0; i < strlen(filename); i++) {
		file_name[i] = filename[i];
	}
	printf("pcb.cc line 129 file_name: %s\n", file_name);

	thread->Fork(MyStartProcess, pID);

	delete file_name;
	mutex->Release();
	return pID;
}

void PCB::SetFileName(char* fn)
{
	strcpy(processname, fn);
}

char* PCB::GetFileName()
{
	 return this->processname;
}


//*************************************************************************************
void MyStartProcess(int pID)
{
	printf("pcb.cc line 141 pID = %d\n", pID);
	char *filename = pTab->GetFileName(pID);
	printf("pcb.cc line 140 filename : %s\n", file_name);
	AddrSpace *space= new AddrSpace(file_name);
	if(space == NULL)
	{
		printf("\nLoi: Khong du bo nho de cap phat cho tien trinh !!!\n");
		return; 
	}
	currentThread->space = space;

	space->InitRegisters();		// set the initial register values
	space->RestoreState();		// load page table register

	machine->Run();			// jump to the user progam
	ASSERT(FALSE);			// machine->Run never returns;
						// the address space exits
						// by doing the syscall "exit"
}

