#ifndef __SHELL_HEADER_H__
#define __SHELL_HEADER_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>

#define MAX_INPUT_LENGTH 512
#define TOKEN_LENGTH 128
#define MAX_NUMBER_ARGUMENTS 16

// Global Variable Declarations ================================================
// User input string
char userInput [MAX_INPUT_LENGTH];
// Store the command
char command [TOKEN_LENGTH];
// This is used as the argv. It uses dynamic memory allocation
char* inputArgumentsList[MAX_NUMBER_ARGUMENTS];
// These 2 get the necessary information out of the env**
char envPaths[MAX_INPUT_LENGTH];
char envHome[TOKEN_LENGTH];
// This is used as the file destination for IO Redirection
char redirectFileName[TOKEN_LENGTH];
// This is used in execv
char executionString[MAX_INPUT_LENGTH];
int IOType;
int numberArgs;
int pid;
int deathStatus;

typedef enum ioRedirectionType {
    NO_IO = 0, INPUT, REPLACE, APPEND
} IOTYPE;

// Function Declarations =======================================================
void welcome (void);
void initializeGlobals(char** env);
void freeGlobals(void);
void exitFunc (int status);
void resetGlobalsForNextInput (void);
void runPrompt (void);
int parseInput (void);

// Subroutines =================================================================
int charIsWhiteSpace (char character);
int charIsIORedirection (char c);
int skipWhiteSpace (int i);

#endif //__SHELL_HEADER_H__