#include "syscall.h"
#include "copyright.h"

void main()
{
    // Declarations
    int success;                     // Variable to check for success
    SpaceId scannerId, resultId;     // File IDs
    char currentChar;                // Current character read from file
    int capacity1, capacity2, capacity3;  // Capacities of three scanners
    int capacity;                    // Current scanner capacity
    int isResultDone;                // Flag to indicate if result file is done

    //-----------------------------------------------------------
    
    // Initialize capacities
    capacity1 = capacity2 = capacity3 = 0;
    
    while (1)
    {
        Down("_scanner");

        // Open result file "result.txt" for writing
        resultId = Open("result.txt", 0);
        if (resultId == -1)
        {    
            Up("passenger");
            return;
        }

        while (1)
        {
            Down("scanner");
            currentChar = 0;            
            
            // Open scanner file "scanner.txt" for reading
            scannerId = Open("scanner.txt", 1);
            if (scannerId == -1)
            {
                Close(resultId);
                Up("passenger");
                return;
            }
        
            capacity = 0;
            isResultDone = 0;
            while (1)
            {       
                if (Read(&currentChar, 1, scannerId)  == -2)
                {    
                    Close(scannerId);        
                    break;
                }
                if (currentChar != '*')
                {
                    capacity = capacity * 10 + (currentChar - '0');
                }
                else
                {
                    isResultDone = 1;                
                    Close(scannerId);
                    break;            
                }

            }

            // Assign capacity to the appropriate scanner
            if (capacity != 0)
            {
                if (capacity1 <= capacity2 && capacity1 <= capacity3)
                {
                    capacity1 += capacity;
                    WriteInt(capacity, resultId);
                    Write("1 ", 2, resultId);
                }
                else if (capacity2 <= capacity1 && capacity2 <= capacity3)
                {
                    capacity2 += capacity;
                    WriteInt(capacity, resultId);
                    Write("2 ", 2, resultId);
                }
                else 
                {
                    capacity3 += capacity;
                    WriteInt(capacity, resultId);
                    Write("3 ", 2, resultId);
                }
            }
            if (isResultDone == 1)
            {
                // Reset scanner capacities
                capacity1 = capacity2 = capacity3 = 0;
                Close(resultId);                
                Up("passenger");
                break;                
            }
            Up("passenger");
        }
    }
}
