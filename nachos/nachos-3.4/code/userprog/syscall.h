/* syscalls.h 
 * 	Nachos system call interface.  These are Nachos kernel operations
 * 	that can be invoked from user programs, by trapping to the kernel
 *	via the "syscall" instruction.
 *
 *	This file is included by user programs and by the Nachos kernel. 
 *
 * Copyright (c) 1992-1993 The Regents of the University of California.
 * All rights reserved.  See copyright.h for copyright notice and limitation 
 * of liability and disclaimer of warranty provisions.
 */

#ifndef SYSCALLS_H
#define SYSCALLS_H

#include "copyright.h"

/* system call codes -- used by the stubs to tell the kernel which system call
 * is being asked for
 */

#define SC_Halt		        0
#define SC_Exit		        1
#define SC_Exec		        2
#define SC_Join		        3
#define SC_Create   	    4
#define SC_Open		        5
#define SC_Read		        6
#define SC_Write	        7
#define SC_Close	        8
#define SC_Fork		        9
#define SC_Yield	        10
#define SC_ReadInt	        11
#define SC_PrintInt	        12
#define SC_ReadFloat        13
#define SC_PrintFloat       14
#define SC_FreeFloat        15
#define SC_CompareFPs       16
#define SC_ReadChar 	    17
#define SC_PrintChar        18
#define SC_ReadString       19
#define SC_PrintString      20
#define SC_WriteInt         21
#define SC_WriteFloat       22
#define SC_CreateSemaphore  23
#define SC_Up               24    
#define SC_Down             25
#define SC_Seek             26
#ifndef IN_ASM

/* The system call interface.  These are the operations the Nachos
 * kernel needs to support, to be able to run user programs.
 *
 * Each of these is invoked by a user program by simply calling the 
 * procedure; an assembly language stub stuffs the system call code
 * into a register, and traps to the kernel.  The kernel procedures
 * are then invoked in the Nachos kernel, after appropriate error checking, 
 * from the system call entry point in exception.cc.
 */

/* Stop Nachos, and print out performance stats */
void Halt();		
 

/* Address space control operations: Exit, Exec, and Join */

/* This user program is done (status = 0 means exited normally). */
void Exit(int status);	

/* A unique identifier for an executing user program (address space) */
typedef int SpaceId;	
 
/* Run the executable, stored in the Nachos file "name", and return the 
 * address space identifier
 */
SpaceId Exec(char *name);
 
/* Only return once the the user program "id" has finished.  
 * Return the exit status.
 */
int Join(SpaceId id); 	
 

/* File system operations: Create, Open, Read, Write, Close
 * These functions are patterned after UNIX -- files represent
 * both files *and* hardware I/O devices.
 *
 * If this assignment is done before doing the file system assignment,
 * note that the Nachos file system has a stub implementation, which
 * will work for the purposes of testing out these routines.
 */
 
/* A unique identifier for an open Nachos file. */
typedef int OpenFileID;	

/* when an address space starts up, it has two open files, representing 
 * keyboard input and display output (in UNIX terms, stdin and stdout).
 * Read and Write can be used directly on these, without first opening
 * the console device.
 */

#define ConsoleInput	0  
#define ConsoleOutput	1  
 
/* Create a Nachos file, with "name" */
int Create(char *name);

/* Open the Nachos file "name", and return an "OpenFileId" that can 
 * be used to read and write to the file.
 */
OpenFileID Open(char *name, int type);

/* Write "size" bytes from "buffer" to the open file. */
void Write(char *buffer, int size, OpenFileID id);

/* Read "size" bytes from the open file into "buffer".  
 * Return the number of bytes actually read -- if the open file isn't
 * long enough, or if it is an I/O device, and there aren't enough 
 * characters to read, return whatever is available (for I/O devices, 
 * you should always wait until you can return at least one character).
 */
int Read(char *buffer, int size, OpenFileID id);

/* Close the file, we're done reading and writing to it. */
void Close(OpenFileID id);



/* User-level thread operations: Fork and Yield.  To allow multiple
 * threads to run within a user program. 
 */

/* Fork a thread to run a procedure ("func") in the *same* address space 
 * as the current thread.
 */
void Fork(void (*func)());

/* Yield the CPU to another runnable thread, whether in this address space 
 * or not. 
 */
void Yield();


/* Read an integer number input by user from console. */
int ReadInt();

/* Write an integer number console. */
void PrintInt(int number);

/* Read a float number input by user from console 
 * If not a float number, return zero (0.0).
 */
float* ReadFloat();

/* Write a float number console */
void PrintFloat(float* number);

/* Free allocated memory for float pointer*/
void FreeFloat(float* value);

/* Compare the values of two float pointers f1, f2
 * -1: f1 < f2
 *  0: f1 = f2
 *  1: f1 > f2
 */
int CompareFPs(float* f1, float* f2);

/* Read a character input by user from console */
char ReadChar();

/* Write a character to console */
void PrintChar(char character);

/* Read a string input by user from console into the buffer.
 * The string will end when the user presses enter
 * or when its length is greater than or equal to the specified length
 */
void ReadString (char* buffer, int length);

/* Write a string to console*/
void PrintString (char* buffer);


/* Write a int number into the file 
 * return value is -1 for fail 
 *                  0 for success */
int WriteInt(int number, OpenFileID id);

/* Write a float number into the file 
 * return value is -1 for fail 
 *                  0 for success */
int WriteFloat(float* number, OpenFileID id);

/* Create a new Semaphore */
int CreateSemaphore(char* name, int semval);

/* Use class STable to free wating process */
int Up(char* name);

/* Use class Stable to make a process wait */
int Down(char* name);

int Seek(int pos, OpenFileID id);


#endif /* IN_ASM */

#endif /* SYSCALL_H */

