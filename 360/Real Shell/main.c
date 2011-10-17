/* 
 * File:   main.c
 * Author: chawk
 *
 * Created on October 12, 2011, 10:56 AM
 */

#include "shellHeader.h"


/*
 * The CASH (Cam Shell) main function 
 */
int main (int argc, char* argv[], char* env[]) {
    int fd;
    
    initializeGlobals(env);
    welcome();
    
    while (1) {
        //printf("numbArgs: %d\n", numberArgs);
        resetGlobalsForNextInput();
        
        runPrompt();
        parseInput();
        
        if (inputArgumentsList[0] == NULL) {
            continue;
        }
        
        //printParsedInput();
        strcpy(command, inputArgumentsList[0]);
        
        if ((strcmp(command, "quit") == 0) || (strcmp(command, "exit") == 0)) {
            exitFunc(0);
        } else if (strcmp(command, "cd") == 0) {
            if (inputArgumentsList[1] == NULL) {
                //printf("CASH: Please specify a directory name\n");
                chdir(envHome);
            } else if (chdir(inputArgumentsList[1])) {
                printf("\tCASH: Directory %s could not be found\n", 
                        inputArgumentsList[1]);
            }
        } else {
            // Here we go, the hard stuff!
            pid = fork();
            if (pid) {
                // pid indicates parent
                pid = wait(&deathStatus);
                if (deathStatus != 0) {
                    printf("Child died with a status of %d\n", deathStatus);
                }
            } else {
                // Child, time to find/execute command
                // IORedirection Time:
                switch (IOType) {
                    case (NO_IO):
                        //printf("CASH: No IO redirection.\n");
                        break;
                    case (INPUT):
                        //printf("CASH: Input IO redirection to %s.\n", redirectFileName);
                        close(0);
                        fd = open(redirectFileName, O_RDONLY);
                        if (fd == -1) {
                            printf("CASH: Cannot open file %s for input. \nExiting...\n", redirectFileName);
                            return 1;
                        } 
                        break;
                    case (REPLACE):
                        //printf("CASH: Replace IO redirection to %s.\n", redirectFileName);
                        close(1);
                        fd = open (redirectFileName, O_WRONLY | O_CREAT, 0644);
                        
                        break;
                    case(APPEND):
                        //printf("CASH: Append IO redirection to %s.\n", redirectFileName);
                        close(1);
                        fd = open (redirectFileName, O_WRONLY | O_APPEND | O_CREAT, 0644);
                        
                        break;
                    default:
                        break;
                }
                // Execution attempt loop:
                strcpy(executionString, strtok(envPaths, ":"));
                while (executionString[0] != '\0') {
                    strcat(executionString, "/");
                    strcat(executionString, command);
                    //printf("Attempting: %s\n", executionString);
                    execve(executionString, inputArgumentsList, env);
                    strcpy(executionString, strtok(NULL, ":"));
                }
                printf("Command %s could not be found.\n", command);
                exitFunc(1);
            }
        }
    }
    return 0;
}

