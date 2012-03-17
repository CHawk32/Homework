/* 
 * File:   main.c
 * Author: Cameron Hawkins
 *
 * Created on September 3, 2011, 3:32 PM
 */

#include "fileSystemHeader.h"

// UI Functions ===============================================================

void initializeShell (void) {
    root = malloc((int) sizeof (Node));
        root->parentPtr = root;
        root->siblingPtr = root;
        root->childPtr = NULL;

        strcpy(root->name, "/");
        root->type = 'D';

        cwd = root;
    if (!restoreState(".defaultStateFile.txt")) {
        printf("\tRestore function failed.\n");
    } else {
        printf("\tState successfully restored.\n");
    }
    restartShell();
}

void restartShell (void) {
    strcpy(command, "");
    strcpy(pathname, "");
    strcpy(line, "");
    strcpy(dirname, "");
    strcpy(basename, "");
}




// UNIX commands ==============================================================
void mkdir (void) { 
    createNode('D');   
}

void rmdir (void) {
    deleteNode('D');
}

void cd (void) {
    Node* tmp;
    if (strlen(pathname) == 0) {
        cwd = root;
        return;
    }
    
    tmp = followDirname();
    if (tmp == NULL) {
        printf("\tDirectory name not found.\n");
        return;
    }
    tmp = searchDirFor (tmp , basename);
    if (tmp == NULL) {
        printf("\tBasename not found.\n");
        return;
    }
    if (tmp->type == 'F') {
        printf("\tCan't cd into a file.\n");
        return;
    }
    cwd = tmp; 
}

void ls (void) {
    Node *tmp = followDirname();
    if (strlen(basename) != 0) {
        tmp = searchDirFor(tmp, basename);
    }
    
    if (tmp->type == 'F') {
        printf("\tYou can't ls a FILE.\n");
        return;
    }
    
    tmp = tmp->childPtr;
    
    while (tmp != NULL) {
        printf("%c ", tmp->type);
        printf("%s\n", tmp->name);
        tmp = tmp->siblingPtr;
    }
}

void creat (void) {
    createNode('F');
}

void rm (void) {
    deleteNode('F');
}

void pwd (void) {
    setPathnameToNode (cwd);
    printf("%s\n", pathname);
}

void save (void) {
    if (saveState(pathname)) {
        setPathnameToNode(cwd);
        printf("\tSave to ' %s ' was successful.\n", pathname);
    }
}

void reload (void) {
    if (!restoreState(pathname)) {
        printf("\tRestore from ' %s ' Unsuccessful.\n", pathname);
        return;
    }
    printf("\tRestore Successful.\n");
}

void quit (void) {
    printf("\tSaving state.\n");
    if (!saveState(".defaultStateFile.txt")) {
        printf("\tError saving state.\n");
    }
    
    printf("\tDeallocating Tree.\n");
    cwd = root;
    recursiveDeallocateNode(root);
    free(root);
    
    printf("\tTree Deallocated.\n");
    printf("\tShell Terminated.\n");
}

void clear (void) {
    system("clear");
}




// Utility Functions ==========================================================
int parseInput (void) {
    int inChar = 0, wordChar = 0;
    char currentChar = ' ';
    
    // Skip leading spaces
    while (currentChar == ' ') {
        currentChar = line[inChar++];
    }
   
    // Load up the command
    while ((currentChar != 32) && (currentChar != '\n')) {
        command[wordChar] = currentChar;
        currentChar = line[inChar++];
        wordChar++;
    }
    
    // Make the command a valid C string
    command[wordChar] = '\0';
    wordChar = 0;
    
    // Skip middle spaces
    while (currentChar == ' '){
        currentChar = line[inChar++];
    }
    
    // No Pathname Case
    if (currentChar == '\n') {
        pathname[0] = '\0';
        return 1;
    }
    
    // Load up the pathname
    while ((currentChar != ' ') && (currentChar != '\n')) {
        pathname[wordChar] = currentChar;
        currentChar = line[inChar++];
        wordChar++;
    }
    
    // Make the pathname a valid C string
    pathname[wordChar] = '\0';
    
    // cycle through trailing spaces to anything thats not a space
    while (currentChar == ' '){
        currentChar = line[inChar++];
    }
    // if we have additional chars, there were too many words for input
    if (currentChar != '\n') {
        return -1;
    }
    
    // Success!
    return 1;
}

int getCommandId (void) {
    char *commands[11] = {
        "mkdir", "rmdir", "cd", "ls", "pwd", "creat", "rm","save", "reload", "quit", "clear"
    };
    
    int i = 0;
    
    for (i = 0; i < 11; i++) {
        if (!strcmp(command, commands[i])) {
            return i;
        }
    }
    
    return -1;
}

int validateAndParsePathname (void) {
    int pathnameIndex = 0, bufferIndex = 0;
    char buff[64];
    char current;
    int count = 1;
    
    current = pathname[pathnameIndex];
    strcpy(dirname, "\0");
    
    if (current == '/') {
        strcat(dirname, "/");
        pathnameIndex++;
        current = pathname[pathnameIndex];
    } else if (current == '\0') {
        // case no pathname
        return 0;
    }
    
    while (isValidChar(current)) {
        buff[bufferIndex] = current;
        bufferIndex++;
        pathnameIndex++;
        
        // This is the end of this directory in the path
        if (pathname[pathnameIndex] == '/') {
            count++;
            // No more input after the final '/'
            if (pathname[pathnameIndex + 1] == '\0') {
                buff[bufferIndex] = '\0';
                strcpy(basename, buff);
                return count;
            }
            // There is more input after '/'
            buff[bufferIndex] = '/';
            buff[bufferIndex + 1] = '\0';
            strcat(dirname, buff);
            bufferIndex = 0;
            pathnameIndex++;
        // This is the end of Input
        } else if (pathname[pathnameIndex] == '\0') {
            buff[bufferIndex] = '\0';
            strcpy(basename, buff);
            return count;
        }
        
        current = pathname[pathnameIndex];
    }
    // Current char wasn't valid
    return -1;
}    
    
int isValidChar (char current) {
    // A-Z, a-z, 0-9, or _ or . return true
    if ( ((current >= 'A') && (current <= 'Z')) || 
         ((current >= 'a') && (current <= 'z')) || 
         ((current >= '0') && (current <= '9')) ||
         (current == '_') ||
         (current == '.')   ) {
        return 1;
    } else {
        return 0;
    }
}

Node* followDirname (void) {
    Node* tmp;
    int dirnameSurfer = 0, buffSurfer = 0;
    char buff[64];
    
    if (dirname[dirnameSurfer] == '/') {
        tmp = root;
        dirnameSurfer++;
    } else {
        tmp = cwd;
    }
    
    if (dirname[dirnameSurfer] == '\0') {
        return tmp;
    }
    
    while(1) {
        buffSurfer = 0;
        while ((dirname[dirnameSurfer] != '/') && (dirname[dirnameSurfer] != '\0')) {
            buff[buffSurfer] = dirname[dirnameSurfer];
            dirnameSurfer++;
            buffSurfer++;
        }
        // skip the '/'
        dirnameSurfer++;
        buff[buffSurfer] = '\0';
        buffSurfer++;
        
        tmp = searchDirFor (tmp, buff);
        
        if (tmp == NULL) {
            return NULL;
        }
        if (dirname[dirnameSurfer] == '\0') {
            return tmp;
        }
    }
 
    return tmp;
}

Node* searchDirFor (Node* dir, char* string) {
    Node* tmp = dir->childPtr;
    if (tmp == NULL) {
        return NULL;
    }
    while (1) {
        if (!strcmp(tmp->name, string)) {
            // Found it!
            return tmp;
        } else if (tmp->siblingPtr == NULL) {
            return NULL;
        }
        tmp = tmp->siblingPtr;
    }
}

void createNode (char type) {
    Node* tmp = NULL, *tmp2 = NULL;
    tmp = followDirname();
    if (tmp == NULL) {
        printf("\tDirectory not found.\n");
        return;
    }
    if (tmp->type == 'F') {
        printf("\tYour trying to put a Node into a file.\n");
    }

    // Step3: If the basename is available make a new directory
    if (searchDirFor(tmp, basename) == NULL) {
        // basename is not taken, time to build a new directory
        // Find the next available space.
        tmp2 = tmp->childPtr;
        if (tmp2 == NULL) {
            // tmp had no children
            tmp2 = (Node*) malloc((int) sizeof(Node));
            tmp->childPtr = tmp2;
            
        } else {
            while (tmp2->siblingPtr != NULL) {
                tmp2 = tmp2->siblingPtr;
            }
            tmp2->siblingPtr = (Node*) malloc((int) sizeof(Node));
            tmp2 = tmp2->siblingPtr;
        }
        
        tmp2->childPtr = NULL;
        tmp2->parentPtr = tmp;
        tmp2->siblingPtr = NULL;
        tmp2->type = type;
        strcpy(tmp2->name, basename);
        
        return;
        
    } else {
        printf("\tThat basename is taken.\n");
        return;
    }
}

void deleteNode (char type) {
    Node *tmp, *deathrow;

    tmp = followDirname();
    
    if ((tmp == NULL) || (tmp->childPtr == NULL)) {
        printf("\tDIRECTORY/FILE not found.\n");
        return;
    }


    if (!strcmp(tmp->childPtr->name, basename)) {
        // Directory to destroy is the first child.
        deathrow = tmp->childPtr;
        if (deathrow->type != type) {
            printf("\tDIRECTORY to kill is a FILE or vice versa.\n");
            return;
        }
        if (deathrow->childPtr != NULL) {
            printf("\tDIRECTORY to kill is not Empty.\n");
            return;
        }
        
        // Execution:
        if (deathrow->siblingPtr == NULL) {
            tmp->childPtr = NULL;
        } else {
            tmp->childPtr = deathrow->siblingPtr;
        }
        free(deathrow);
        return;

    } else {
        // Make tmp->sibling pont to the directory to destroy
        tmp = tmp->childPtr;
        while (tmp->siblingPtr != NULL) {
            if (!strcmp(tmp->siblingPtr->name, basename)) {
                // Found it, time to kill it if we can.
                deathrow = tmp->siblingPtr;
                if (deathrow->type != type) {
                    printf("\tDIRECTORY to kill is a FILE or vice versa.\n");
                    return;
                }
                if (deathrow->childPtr != NULL) {
                    printf("\tDIRECTORY to kill is not Empty.\n");
                    return;
                }
                
                // Execution:
                if (deathrow->siblingPtr == NULL) {
                    tmp->siblingPtr = NULL;
                } else {
                    tmp->siblingPtr = deathrow->siblingPtr;
                }
                free(deathrow);
                return;


            } else if (tmp->siblingPtr->siblingPtr == NULL) {
                printf("\tDIRECTORY/FILE to kill could not be found.\n");
                return;
            }
            
            tmp = tmp->siblingPtr;
        }
    }
}

void setPathnameToNode (Node* node) {
    char *whereContentStarts = NULL;
    char buff[128] = {'\0'};
    int n = 127, j = 0, k = 0;
    int nameLength = 0;
    Node* tmp = node;
    
    buff[n] = '\0';
    n--;
  
    // Load buff from right loop
    while (tmp != root) {
        nameLength = strlen(tmp->name);
        n -= nameLength;
        k = 0;
        for (j = 0; j < nameLength; j++) {
            buff[j + n + 1] = tmp->name[k];
            k++;
        }
        buff[n] = '/';
        n--;
        tmp = tmp->parentPtr;
    }
    
    whereContentStarts = &buff[n + 1];
    strcpy(pathname, whereContentStarts);
}

int saveState (char *file) {
    Node* tmp = root;
    FILE *fp;
    
    fp = fopen(file, "w");
    if (fp == NULL) {
        return 0;
    }
    fprintf(fp, "D\t/\n");
    
    tmp = tmp->childPtr;
    
    recursiveFileWrite (fp, tmp);
    
    fclose(fp);
    printf("\tSave State Complete.\n");
    return 1;
}

void recursiveFileWrite (FILE* fp, Node* node) {
    if (node == NULL) {
        return;
    }
    setPathnameToNode(node);
    fprintf(fp,"%c\t%s\n", node->type, pathname);
    
    recursiveFileWrite (fp, node->childPtr);
    recursiveFileWrite (fp, node->siblingPtr);
    return;
}

int restoreState (char *file) {
    char type = 0, whiteSpace = 0;
    char buff[128];
    FILE *fp = fopen(file, "r");
    
    if (fp == NULL) {
        return 0;
    }
    
    fscanf(fp, "%c", &type);
    fscanf(fp, "%s", buff);
    fscanf(fp, "%c", &whiteSpace);
    
    if ((buff[0] != '/') || (type != 'D')) {
        printf("\tReload file not in proper format.\n");
        return 0;
    }
    // Set up tree to be restored
    cwd = root;
    if (root->childPtr != NULL) {
        // Dealocate Previous tree if it exists 
        recursiveDeallocateNode(root);
        root->childPtr = NULL;
    }
    
    while (!feof(fp)) {
        fscanf(fp, "%c", &type);
        fscanf(fp, "%s", buff);
        fscanf(fp, "%c", &whiteSpace);
        if (!feof(fp)) {
            strcpy(pathname, buff);
            //pathname[strlen(pathname) + 1] = '\n';
            validateAndParsePathname();
            printf("Recovery attempt: %c %s\n", type, buff);
            createNode(type);
        } else {
            break;
        }
    }
    
    fclose(fp);
    return 1;
}

void recursiveDeallocateNode (Node* node) {
    if (node == NULL) {
        return;
    }
    recursiveDeallocateNode (node->childPtr);
    
    if (node == root) {
        return;
    }
    
    recursiveDeallocateNode (node->siblingPtr);
    
    free(node);
    
}

