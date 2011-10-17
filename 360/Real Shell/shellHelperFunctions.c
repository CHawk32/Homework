/* 
 * File:   shellHelperFunctions.c
 * Author: chawk
 *
 * Created on October 12, 2011, 10:57 AM
 */

#include "shellHeader.h"

// Print Welcome message
void welcome (void) {
    printf("Welcome to the CASH (Cam Shell) Version 0\n");
}

// Set up the arguments list to send as argv
void initializeGlobals(char** env) {
    int i = 0, pathDone = 0, homeDone = 0;
    char buffer[MAX_INPUT_LENGTH];
    //for (i = 0; i < MAX_NUMBER_ARGUMENTS; i++) {
        //inputArgumentsList[i] = malloc(TOKEN_LENGTH);
    //}
    
    i = 0;
    while (strcpy(buffer, env[i])) {
        if ((buffer[0] == 'P') 
         && (buffer[1] == 'A')
         && (buffer[2] == 'T')
         && (buffer[3] == 'H')
         && (buffer[4] == '=')) {
            strcpy(envPaths, &buffer[5]);
            pathDone = 1;
        } else if ((buffer[0] == 'H')
                && (buffer[1] == 'O')
                && (buffer[2] == 'M')
                && (buffer[3] == 'E')
                && (buffer[4] == '=')) {
            strcpy(envHome, &buffer[5]);
            homeDone = 1;
        }
        
        if (pathDone && homeDone) {
            return;
        }
                
        i++;
    }
    
    printf("CASH: Error in env, PATH field not found.\n");
    exitFunc(1);
}

// Free the above list
void freeGlobals(void) {
    int i = 0;
    for (i = 0; i < MAX_NUMBER_ARGUMENTS; i++) {
        if (inputArgumentsList[i] != NULL) {
            //printf("CASH: Freeing inputList[%d]: %s\n", i,inputArgumentsList[i]);
            free(inputArgumentsList[i]);
            inputArgumentsList[i] = NULL;
        }
    }
}

// Exit Function, free's dynamic stuff
void exitFunc (int status) {
    freeGlobals();
    exit(status);
}

// This function 0's the global variables to prevent spill over from
// previous iterations
void resetGlobalsForNextInput (void) {
    freeGlobals();
    IOType = NO_IO;
    numberArgs = 0;
    pid = 0;
    deathStatus = 0;
    bzero(executionString, MAX_INPUT_LENGTH);
    bzero(command, TOKEN_LENGTH);
    bzero(userInput, MAX_INPUT_LENGTH);
    bzero(redirectFileName, TOKEN_LENGTH);
}

// This function prints the prompt and gathers the input, populates the 
// global input string.
void runPrompt (void) {
    int i = 0;
    char buff[64] = {0};
    
    getwd(buff);
    
    // Get only this directories name
    i = 64;
    while (buff[i] != '/') {
        i--;
    }
    strcpy (buff, &buff[i]);
    
    // Print a nice user prompt
    printf("CASH: %s> ", buff);
    fgets(userInput, MAX_INPUT_LENGTH, stdin);
    userInput[strlen(userInput) - 1] = '\0';
}

// This monster function parses the input string into the global variables
int parseInput (void) {
    int success = 1;
    int thisArgument = 0;
    int argIndex = 0;
    int inputIndex = 0;
    
    // No input, get out
    if ((userInput == NULL) || (userInput[0] == '\0')) {
        return success;
    }
    
    inputArgumentsList[thisArgument] = malloc(TOKEN_LENGTH);
    
    // Finite Automata time!
    while (1) {
        if (userInput[inputIndex] == '\0') {
            inputArgumentsList[thisArgument][argIndex] = '\0';
            return success;
        } else if (charIsIORedirection(userInput[inputIndex])) {
            if (charIsIORedirection(userInput[inputIndex + 1])) {
                IOType = APPEND;
                inputIndex++; 
            }
            inputIndex++;
            break;
        } else if (charIsWhiteSpace(userInput[inputIndex])) {
            inputIndex = skipWhiteSpace(inputIndex);
            if (userInput[inputIndex] == '\0') {
                return success;
            }
            inputIndex--;
            
            inputArgumentsList[thisArgument][argIndex] = '\0';
            thisArgument++; argIndex = 0;
            // Check to make sure the next token isn't an IORedirect or whitespace
            if (!charIsIORedirection(userInput[inputIndex + 1])) {
                inputArgumentsList[thisArgument] = malloc(TOKEN_LENGTH);
                numberArgs++;
            }
        } else {
            inputArgumentsList[thisArgument][argIndex] = userInput[inputIndex];
            argIndex++;
        }
        inputIndex++;
    }
    
    // Handle IORedirection
    if (IOType) {
        inputIndex = skipWhiteSpace (inputIndex);
        strcpy(redirectFileName, &userInput[inputIndex]);
    }
    
    return success;
} 

// This function is a test function to show me that my parser parsed the 
// input string correctly
void printParsedInput (void) {
    int i = 0;
    printf("\nCASH:\n =========== FOR DEBUGGING =============\n");
    printf("Original Input: %s\n", userInput);
    printf("Parsed Command: %s\n", command);
    
    printf("Argument(s) List (%d):\n", numberArgs);
    while (inputArgumentsList[i] != NULL) {
        printf("\tArg[%d]: %s\n", i, inputArgumentsList[i]);
        i++;
    }
    
    printf("IO Redirection Type: %d\n", IOType);
    printf("IO Redirection File: %s\n", redirectFileName);
    printf("\n");
}
















// Subroutines =================================================================

// Sees if a given character is whitespace or not
int charIsWhiteSpace (char c) {
    if ((c == ' ') || (c == '\t')) {
        return 1;
    } else {
        return 0;
    }
}

// Sees if a IO redirect is found
// Reason it takes an int* : It needs to increment the outside 
// functions index by 2 if the IO redirect character is 2 chars long
int charIsIORedirection (char c) {
    if (c == '<') {
        IOType = INPUT;
        return 1;
    } else if (c == '>') {
        IOType = REPLACE;
        return 1;
    } else {
        return 0;
    }
        
}

int skipWhiteSpace (int i) {
    while (charIsWhiteSpace(userInput[i])) {
        i++;
    }
    return i;
}

