/* 
 * File:   main.c
 * Author: Cameron Hawkins
 *
 * Created on September 3, 2011, 3:32 PM
 */

#include "fileSystemHeader.h"

int main (int argc, char* argv[]) {
    int commandId;
    initializeShell();
    
    while (1) {
        // Get next command
        printf("%s : user$ ", cwd->name);
        fgets(line, 128, stdin);
        
        // Input Sanitizing
        if (line[0] == '\n') {
            // No input
            goto RESTART;
        }
                
        if (parseInput() == -1) {
            // More than 2 words written
            printf("\tError with input: %s \tToo Many words.\n", line);
            goto RESTART;
        }
        
        if (validateAndParsePathname() == -1) {
            // Error with pathname
            printf("\tError with pathname format.\n");
            goto RESTART;
        }
        
        // Run the command
        commandId = getCommandId();
        switch (commandId) {
            case(MKDIR): mkdir(); break;
            case(RMDIR): rmdir(); break;
            case(CD): cd(); break;
            case(LS): ls(); break;
            case(PWD): pwd(); break;
            case(CREAT): creat(); break;
            case(RM): rm(); break;
            case(SAVE): save(); break;
            case(RELOAD): reload(); break;
            case(QUIT): goto QUIT; break;
            case(CLEAR): clear(); break;
            default:
                printf("\tCommand: ' %s ' not found.\n", command);
                goto RESTART;
        }
        
        RESTART:
        restartShell();
    }
    
    QUIT:
    quit();
    return 0;
}


