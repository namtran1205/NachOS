#include "syscall.h"
#include "copyright.h"

int main()
{
    // Declaration
    int success;                                    // Variable to check for success
    SpaceId inputId, outputId, passengerId, resultId;// File IDs
    int numTimeMoments;                             // Number of time moments to process
    char currentChar;                               // Current character read from file

    // Create semaphores
    success = CreateSemaphore("main", 0);
    if (success == -1)
        return 1;
    success = CreateSemaphore("passenger", 0);
    if (success == -1)
        return 1;
    success = CreateSemaphore("scanner", 0);
    if (success == -1)
        return 1;
    success = CreateSemaphore("_scanner", 0);
    if (success == -1)
        return 1;
    
    // Create output file "output.txt"
    success = Create("output.txt");
    if (success == -1)
        return 1;
    
    // Open input file "input.txt" for reading
    inputId = Open("input.txt", 1);
    if (inputId == -1)
        return 1;
    
    // Open output file "output.txt" for reading and writing
    outputId = Open("output.txt", 0);
    if (outputId == -1)
    {
        Close(inputId);
        return 1;
    }

    // Read the number of time moments from input file
    numTimeMoments = 0;
    while (1)
    {
        Read(&currentChar, 1, inputId);
        if (currentChar != '\n')
        {
            if (currentChar >= '0' && currentChar <= '9')
                numTimeMoments = numTimeMoments * 10 + (currentChar - '0');
        }
        else
            break;
    }

    // Execute Passenger.c process
    success = Exec("./test/Passenger");
    if (success == -1)
    {
        Close(inputId);
        Close(outputId);
        return 1;
    }

    // Execute Scanner.c process
    success = Exec("./test/Scanner");
    if (success == -1)
    {
        Close(inputId);
        Close(outputId);
        return 1;
    }

    // Process each time moment
    while (numTimeMoments--)
    {
        // Create passenger file "passenger.txt"
        success = Create("passenger.txt");
        if (success == -1)
        {
            Close(inputId);
            Close(outputId);
            return 1;
        }
        
        // Open passenger file "passenger.txt" for writing
        passengerId = Open("passenger.txt", 0);
        if (passengerId == -1)
        {
            Close(inputId);
            Close(outputId);
            return 1;
        }

        // Read from input file and write to passenger file
        while (1)
        {
            if (Read(&currentChar, 1, inputId) < 1)
            {
                break;
            }

            // Continue reading 1 character until the end of a line (processing each batch of passengers)
            if (currentChar != '\n')
            {
                Write(&currentChar, 1, passengerId);                
            }
            else
                break;                        
        }
        Close(passengerId); 
        // Start the passenger process
        Up("passenger");
        // Main thread must wait
        Down("main");    
        // Read from result file and write the result to output file "output.txt"
        resultId = Open("result.txt", 1);
        if (resultId == -1)
        {
            Close(inputId);
            Close(outputId);
            return 1;
        }
        while (1)
        {
            if (Read(&currentChar, 1, resultId)  < 1)
            {
                Write("\r\n", 2, outputId);
                Close(resultId);
                Up("_scanner");
                break;
            }
            Write(&currentChar, 1, outputId);
            Write(" ", 1, outputId);
        }
    }
    Close(inputId);
    Close(outputId);
    return 0;    
}
