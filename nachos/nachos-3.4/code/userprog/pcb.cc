#include "pcb.h"
#include "utility.h"
#include "system.h"
#include "thread.h"
#include "addrspace.h"


char* file_name = NULL;

extern void MyStartProcess(int pID);


PCB::PCB()
{
	
}

PCB::PCB(int id)
{
	pid= id;
	exitcode= 0;
	numwait= 0;
    if (id == 0)
        this->parentID = -1;
    else
        this->parentID = currentThread->processID;
	
	numwait = exitcode = boolBG = 0;

	thread = NULL;
	JoinStatus= -1;

	joinsem = new Semaphore("JoinSem",0);
	exitsem = new Semaphore("ExitSem",0);
	mutex = new Semaphore("Mutex",1);
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
	thread= new Thread(filename);
	if(thread == NULL)
	{
		printf("\nLoi: Khong tao duoc tien trinh moi !!!\n");
		mutex->Release();
		return -1;
	}
	thread->processID = pID;
	parentID = currentThread->processID;

	file_name = new char[255];
	for (int i = 0; i < strlen(filename); i++) {
		file_name[i] = filename[i];
	}
	file_name[strlen(file_name)] = '\0';

	thread->Fork(MyStartProcess, pID);

	delete file_name;
	file_name = NULL;
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

