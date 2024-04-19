#include "syscall.h"
#include "copyright.h"

void main()
{
    int successCheck;                  // Variable to check for success
    SpaceId passengerFileId, scannerFileId, resultFileId; // File IDs
    char currentChar;                 // Character to read from file
    int jumpToMainThread;             // Flag to jump to the main thread
    int fileLength;                   // Length of the file
    int filePointer;                  // File pointer

    // Semaphore initialization
    Up("_scanner");

    while (1)
    {
        fileLength = 0;

        // Wait for passenger thread
        Down("passenger");

        // Create result.txt
        successCheck = Create("result.txt");
        if (successCheck == -1)
        {
            Up("main");
            return;
        }

        // Open passenger.txt for writing
        passengerFileId = Open("passenger.txt", 1);
        if (passengerFileId == -1)
        {
            Up("main");
            return;
        }

        // Get the length of passenger.txt
        fileLength = Seek(-1, passengerFileId);
        Seek(0, passengerFileId);
        filePointer = 0;

        // Create scanner.txt
        successCheck = Create("scanner.txt");
        if (successCheck == -1)
        {
            Close(passengerFileId);
            Up("main");
            return;
        }

        // Open scanner.txt for reading
        scannerFileId = Open("scanner.txt", 0);
        if (scannerFileId == -1)
        {
            Close(scannerFileId);
            Up("main");
            return;
        }

        // Process each character in passenger.txt
        while (filePointer < fileLength - 1)
        {
            jumpToMainThread = 0;
            Read(&currentChar, 1, passengerFileId);
            if (currentChar != ' ')
            {
                Write(&currentChar, 1, scannerFileId);
            }
            else
            {
                jumpToMainThread = 1;
            }
            if (filePointer == fileLength - 2)
            {
                Write("*", 1, scannerFileId);
                jumpToMainThread = 1;
            }

            // Jump to scanner thread if necessary
            if (jumpToMainThread == 1)
            {
                Close(scannerFileId);
                Up("scanner");

                // Wait for passenger thread
                Down("passenger");

                // Create scanner.txt again
                successCheck = Create("scanner.txt");
                if (successCheck == -1)
                {
                    Close(passengerFileId);
                    Up("main");
                    return;
                }

                // Open scanner.txt for reading
                scannerFileId = Open("scanner.txt", 0);
                if (scannerFileId == -1)
                {
                    Close(passengerFileId);
                    Up("main");
                    return;
                }
            }
            filePointer++;
        }

        // Release main thread
        Up("main");
    }
}
