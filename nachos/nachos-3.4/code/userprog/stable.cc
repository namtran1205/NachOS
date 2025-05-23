#include "stable.h"
#include "system.h"

Stable::Stable()
{	
	this->bm = new BitMap(MAX_SEMAPHORE);
	
	for(int i =0; i < MAX_SEMAPHORE; i++)
	{
		this->semTab[i] = NULL;
	}
	bm->Mark(0);
}

Stable::~Stable()
{
	if(this->bm != NULL)
	{
		delete this->bm;
		this->bm = NULL;
	}
	for(int i = 0; i < MAX_SEMAPHORE; i++)
	{
		if(this->semTab[i] != NULL)
		{
			delete this->semTab[i];
			this->semTab[i] = NULL;
		}
	}
	
}
int Stable::Create(char *name, int init)
{

	// Check da ton tai semaphore nay chua?
	for(int i = 0; i < MAX_SEMAPHORE; i++)
	{
		if(bm->Test(i))
		{


			if (semTab[i]->GetName() == NULL)
				continue;				
			
			if(strcmp(name, semTab[i]->GetName()) == 0)
				return -1;
		}
		
	}
	// Tim slot tren bang semTab trong
	int id = this->FindFreeSlot();
	
	// Neu k tim thay thi tra ve -1
	if(id < 0)
	{
		return -1;
	}

	// Neu tim thay slot trong thi nap Semaphore vao semTab[id]
	this->semTab[id] = new Sem(name, init);
	return 0;
}

int Stable::Wait(char *name)
{
	for(int i =0; i < MAX_SEMAPHORE; i++)
	{
		// Kiem tra o thu i da duoc nap semaphore chua
		if(bm->Test(i))
		{
			// Neu co thi tien hanh so sanh name voi name cua semaphore trong semTab
			if (semTab[i]->GetName() == NULL)
				continue;

			if(strcmp(name, semTab[i]->GetName()) == 0)
			{
				// Neu ton tai thi cho semaphore down(); 
				semTab[i]->wait();
				return 0;
			}
		}
	}
	printf("Khong ton tai semaphore");
	return -1;
}

int Stable::Signal(char *name)
{
	for(int i =0; i < MAX_SEMAPHORE; i++)
	{
		// Kiem tra o thu i da duoc nap semaphore chua
		if(bm->Test(i))
		{
			// Neu co thi tien hanh so sanh name voi name cua semaphore trong semTab
			if (semTab[i]->GetName() == NULL)
				continue;

			if(strcmp(name, semTab[i]->GetName()) == 0)
			{
				// Neu ton tai thi cho semaphore up(); 
				semTab[i]->signal();
				return 0;
			}
		}
	}
	printf("Khong ton tai semaphore");
	return -1;
}


int Stable::FindFreeSlot()
{
	return this->bm->Find();
}