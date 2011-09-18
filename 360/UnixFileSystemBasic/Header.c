/* 
 * File:   fileSystemHeader.h
 * Author: Cameron Hawkins
 *
 * Created on September 3, 2011, 3:32 PM
 */

#ifndef __FILESYSTEMHEADER_H__
#define	__FILESYSTEMHEADER_H__

#include <stdio.h>
#include <string.h>
#include <malloc.h>

// Magic Numbers
#define COMMAND 0
#define PARAMETER 1

enum {
    MKDIR, RMDIR, CD, LS, PWD, CREAT, RM, SAVE, RELOAD, QUIT, CLEAR
};

// Structure declaration
typedef struct node {
    // Type: D or F, Name
    char type;
    char name[64];
    
    // Pointers
    struct node *parentPtr;
    struct node *siblingPtr;
    struct node *childPtr;
} Node;

// Global Vars
// Operational
Node *root;
Node *cwd;

char line[126];

// Users input
char command[16];
char pathname[64];

// Get parsed from pathname if its valid
char dirname[64];
char basename[64];

// Function Declarations
// UI stuff ====================================================================
void initializeShell (void);
void restartShell (void);

// Unix Commands ===============================================================
void mkdir (void);
void rmdir (void);
void cd (void);
void ls (void);
void rm (void);
void pwd (void);
void save (void);
void reload (void);
void quit (void);
void clear (void) ;


// Utility Functions ===========================================================
// Splits user input into command and pathname. returns 1 if successful
// -1 for bad input
int parseInput (void);
// Returns the enum associated with the command or -1 if invalid
int getCommandId (void);
// Splits pathname and basename. returns number of directors in pathname
// or -1 for incorrect input
int validateAndParsePathname (void);
int isValidChar (char current);
// Follows the dirname, If its valid it returns the specified dir
Node* followDirname (void);
Node* searchDirFor (Node* dir, char* string);
// These are used for mkdir/creat and rmdir/rm
void createNode (char type);
void deleteNode (char type);
// Loads the cwd pathname into the (overused) pathname variable
void setPathnameToNode (Node* node);
// Saves the state of the file system to the deignated file.
int saveState (char *file);
void recursiveFileWrite (FILE* fp, Node* node);
// Restore State Function
int restoreState (char *file);
// Recursively Deallocates the Entire Tree, aside from root
void recursiveDeallocateNode (Node* node);

#endif	/* __FILESYSTEMHEADER_H__ */


