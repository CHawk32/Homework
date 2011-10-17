#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>


#include "systemFunctions.h"

extern int sock;

char *clientFunctionList[10] = {"lpwd", "lls", "lcd", "lmkdir", "lrmdir", "lrm", "lcat", "get", "put", "quit"};
char *remoteFunctionList[6] = {"pwd", "ls", "cd", "mkdir", "rmdir", "rm"};
int (*functionPtr[10])(char pathname[], int outFile);

void initializeFunctionList () {
    functionPtr[0] = pwd_func;
    functionPtr[1] = ls_func;
    functionPtr[2] = cd_func;
    functionPtr[3] = mkdir_func;
    functionPtr[4] = rmdir_func;
    functionPtr[5] = rm_func;
    functionPtr[6] = cat_func;
    functionPtr[7] = get_func;
    functionPtr[8] = put_func;
    functionPtr[9] = quit_func;
}

int executeServerCommand (int functionIndex, char pathname[], int outFile) {
    int success = 0;
    bzero(returnString, 4096);
    success = functionPtr[functionIndex](pathname, outFile);
    write(outFile, returnString, strlen(returnString));
    bzero(returnString, 4096);
    return success;
}

int checkClientCommand (char command[], char pathname[], int outFile) {
    int commandKnown = 0, i = 0;
    //Check client function list for known var
    for (i = 0; i < 10; i++) {
        if (strcmp(command, clientFunctionList[i]) == 0) {
            commandKnown = 1;
            break;
        }
    }
    
    if (commandKnown) {
        bzero(returnString, 4096);
        functionPtr[i](pathname, outFile);
        // CAMEDIT
        write(outFile, returnString, 4096);
        //Return -1 on succesful client exectution
        return -1;
    } else {
        //Check server function list for known var
        for (i = 0; i < 6; i++) {
            if (strcmp(command, remoteFunctionList[i]) == 0) {
                commandKnown = 1;
                break;
            }

        }
        if (commandKnown) {
            //return the index of the server command to be executed
            return i;
        }
        else {
            sprintf(formatingString, "Command \"%s\" not recognized!\n", command);
            write(outFile, formatingString, strlen(formatingString));
            //return -2 if this command is not recognized
            return -2;
        }
    }
}

//=================================================
//BEGIN CLIENT FUNCTIONS
//=================================================

int pwd_func (char pathname[], int outFile) {
    char buffer[256];
    getcwd(buffer, 256);
    sprintf(formatingString, "%s\n", buffer);
    strcat(returnString, formatingString);
    //write(outFile, formatingString, 512);
    return 1;
}

int ls_func (char pathname[], int outFile) {
    struct dirent* dirEntry = 0;
    char originalDirectory[INPUT_LENGTH];
    DIR* thisDir = 0;

    getcwd(originalDirectory, INPUT_LENGTH);

    // Open the pathname specified Directory
    if ((pathname == NULL) || (pathname[0] == '\0')) {
        // No pathname -> open Current Directory.
        thisDir = opendir(originalDirectory);
    } else {
        // Open the specified directory 
        thisDir = opendir(pathname);
        if (thisDir == NULL) {
            return 0;
        }
    }

    // Print each of the contents in the directory
    while (dirEntry = readdir(thisDir)) {
        printDetails(dirEntry, outFile);
    }
    return 1;
}

int cd_func (char pathname[], int outFile) {
    int success = chdir(pathname);
    if (success < 0) {
        sprintf(formatingString, "CD: Pathname could not be found.\n");
        //write(outFile, formatingString, 512);
        strcat(returnString, formatingString);
        return 0;
    }
    strcat(returnString, "CD Successful\n");
    return 1;
}

int mkdir_func (char pathname[], int outFile) {
    int success = 0;
    //printf("Made it to mkdir.\n");
    success = mkdir(pathname, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    if (success < 0) {
        sprintf(formatingString, "Error in creating directory %s\n", pathname);
        strcat(returnString, formatingString);
        //write(outFile, formatingString, 512);
        return 0;
    }
    strcat(returnString, "MKDIR Successful\n");
    return 1;
}

int rmdir_func (char pathname[], int outFile) {
    int success = rmdir(pathname);
    if (success < 0) {
        sprintf(formatingString, "Error in removing directory %s.\n", pathname);
        //write(outFile, formatingString, 512);
        strcat(returnString, formatingString);
        return 0;
    }
    strcat(returnString, "RMDIR Successful\n");
    return 1;
}

int rm_func (char pathname[], int outFile) {
    int success = unlink(pathname);
    if (success < 0) {
        sprintf(formatingString, "Error in removing file %s.\n", pathname);
        //write(outFile, formatingString, 512);
        strcat(returnString, formatingString);
        return 0;
    }
    strcat(returnString, "RM Successful\n");
    return 1;
}

int cat_func (char pathname[], int outFile) {
    char buffer[BLOCK_SIZE + 1];
    int readBytes = 0;
    int fd = open(pathname, O_RDONLY);

    if (fd <= 0) {
        sprintf(formatingString, "File %s could not be open for read mode.\n", pathname);
        strcat(returnString, formatingString);
        return 0;
    } else {
        while (readBytes = read(fd, buffer, BLOCK_SIZE)) {
            buffer[readBytes] = '\0';
            sprintf(formatingString, "%s", buffer);
            strcat(returnString, formatingString);
        }
    }
    return 1;
}

int get_func (char pathname[], int outFile) {
    int fd = 0, i = 0, n = 0, fileSize = 0;
    char buff[16];

    sprintf(returnString, "G %s", pathname);
    write(sock, returnString, strlen(returnString));
    read(sock, buff, 16);
    
    if(buff[0] != 'S')
    {
        sprintf(returnString, "File on server could not be found...\n");
        return 0;
    }

    fd = open(pathname,  O_WRONLY | O_CREAT, 0644);
    strtok(buff, " ");
    fileSize = atoi(strtok(NULL, " "));

    while(fileSize > 0)
    {
        n = read(sock, returnString, 4096);
        write(fd, returnString, n);
        fileSize -= n;
    }
    bzero(returnString, 4096);
    close(fd);
    return 1;
}

int put_func (char pathname[], int outFile) {
    int fd = 0, i = 0, n = 0;
    struct stat fileStatus;
    char buff[16];
    fd = open(pathname, O_RDONLY);

    if(fd == 0){
        strcat(returnString, "File could not be found...\n");
        return 0;
    } else {
        fstat(fd, &fileStatus);
        i = fileNameIndex(pathname);
        sprintf(returnString, "P %d %s", (int) fileStatus.st_size, &pathname[i+1]);
        write(sock, returnString, strlen(returnString));
        read(sock, buff, 16);
        if(strcmp(buff, "F") == 0){
            sprintf(returnString, "File could not be open for write on the server...\n");
            return 0;
        }
        
        i = fileStatus.st_size;
        while(i > 0)
        {
            n = read(fd, returnString, 4096);
            write(sock, returnString, strlen(returnString));
            i -= n;
        }
        bzero(returnString, 4096);
        sprintf(returnString, "Transmission of %d bytes complete...\n", (int) fileStatus.st_size);
        close(fd);
        return 1;
    }
}

int quit_func (char pathname[], int outFile) {
    printf("\nConnection to server has been disconnected...\nKilling client.\n\n");
    exit(0);
}

//=================================================
// Helper Functions
//=================================================

// LS helper functions

void printDetails (struct dirent* entry, int outFile) {
    struct stat entryStat;
    struct passwd userPwdStruct;
    struct group userGroup;
    char permissions[INPUT_LENGTH] = {0};
    char userName[64] = {0};
    char groupName[64] = {0};
    char dateStr[128] = {0};
    char tmp[128];
    int fd = open(entry->d_name, O_RDONLY);

    if (fd == 0) {
        sprintf(formatingString, "file not found\n");
        strcat(returnString, formatingString);
        //write(outFile, formatingString, 512);
        return;
    }

    fstat(fd, &entryStat);

    getPermissionsString(&entryStat, permissions);

    userPwdStruct = *getpwuid(entryStat.st_uid);
    strcpy(userName, userPwdStruct.pw_name);

    //userPwdStruct = *getpwuid(entryStat.st_gid);
    userGroup = *getgrgid(entryStat.st_gid);
    strcpy(groupName, userGroup.gr_name);
    strftime(dateStr, 128, "%F %T", gmtime((const time_t*) &entryStat.st_atim));
    sprintf(formatingString, "%s %u %s %s %5u %s %s\n",
            permissions,
            (unsigned int) entryStat.st_nlink,
            userName,
            groupName,
            (unsigned int) entryStat.st_size,
            dateStr,
            entry->d_name
            );
    strcat(returnString, formatingString);
    close(fd);
    //write(outFile, formatingString, strlen(formatingString));
    return;
}

void getPermissionsString (struct stat *status, char *buffer) {
    unsigned short bytes = 0, fileType = 0, usrPerm = 0, grpPerm = 0, othPerm = 0;
    char usrString[4];
    char grpString[4];
    char othString[4];

    // Get the 2 st_mode bytes
    bytes = status->st_mode;

    // Bit masking to parse the permissions out:
    // first 4 bits = file type
    // middle 3 bits = irrelevant for this assignment
    // last 9 bits = permissions: usr grp oth (3 bits a peice)
    fileType = bytes >> 12;
    bytes = bytes << 7;
    bytes = bytes >> 7;
    usrPerm = bytes >> 6;
    bytes = bytes << 10;
    bytes = bytes >> 10;
    grpPerm = bytes >> 3;
    bytes = bytes << 13;
    bytes = bytes >> 13;
    othPerm = bytes;

    // Get the "rwx" string format
    getIndPerminssion(usrString, usrPerm);
    getIndPerminssion(grpString, grpPerm);
    getIndPerminssion(othString, othPerm);

    // Set the first bit to file type
    buffer[0] = getFileTypeChar(fileType);
    buffer[1] = '\0';

    // Catenate the "rwx" strings to the return buffer
    strcat(buffer, usrString);
    strcat(buffer, grpString);
    strcat(buffer, othString);
}

void getIndPerminssion (char* ret, unsigned short ind) {
    switch (ind) {
        case (7):
            strcpy(ret, "rwx");
            break;
        case(6):
            strcpy(ret, "rw-");
            break;
        case(5):
            strcpy(ret, "r-x");
            break;
        case(4):
            strcpy(ret, "r--");
            break;
        case(3):
            strcpy(ret, "-wx");
            break;
        case(2):
            strcpy(ret, "-w-");
            break;
        case(1):
            strcpy(ret, "--x");
            break;
        case(0):
            strcpy(ret, "---");
            break;
    }
}

char getFileTypeChar (unsigned short fileType) {
    switch (fileType) {
        case (10):
            //TODO:: Something is messed up here.
            return 'l';
        case (4):
            return 'd';
        default:
            return '-';
    }
}

int fileNameIndex(char * pathname)
{
    int i = 0;
    for(i = strlen(pathname)-1; i >= 0; i--)
    {
        if(pathname[i] == '/')
            return i;
    }

    return -1;
}