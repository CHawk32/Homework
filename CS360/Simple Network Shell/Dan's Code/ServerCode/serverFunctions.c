#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/socket.h>

#include "serverFunctions.h"

char *remoteFunctionList[6] = {"pwd", "ls", "cd", "mkdir", "rmdir", "rm"};
int (*functionPtr[10])(char pathname[], int sock);

void initializeFunctionList()
{
    functionPtr[0] = pwd_func;
    functionPtr[1] = ls_func;
    functionPtr[2] = cd_func;
    functionPtr[3] = mkdir_func;
    functionPtr[4] = rmdir_func;
    functionPtr[5] = rm_func;
    functionPtr[6] = cat_func;
    functionPtr[7] = get_func;
    functionPtr[8] = put_func;
}

int executeServerCommand(int functionIndex, char pathname[], int sock)
{
    return functionPtr[functionIndex](pathname, sock);
}

//=================================================
//BEGIN SERVER FUNCTIONS
//=================================================
int pwd_func(char pathname[], int sock)
{
    char buffer[256];
    getcwd(buffer, 256);
    sprintf(formatingString, "%s\n", buffer);
    printf("@> SERVER > CLIENT: %s", formatingString);
    write(sock, formatingString, 512);
    return 1;
}
int ls_func(char pathname[], int sock)
{
    formatingString[0] = '\0'; 
    char outputSize[512] = {0};
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
        printDetails(dirEntry, sock);       
    }

    printf("@> SERVER > CLIENT: %s", formatingString);
    sprintf(outputSize, "%d", strlen(formatingString));
    write(sock, outputSize, 512); //send client the size
    write(sock, formatingString, strlen(formatingString)); //send client the output
    printf("%d", strlen(formatingString));

    return 1;
}
int cd_func(char pathname[], int sock)
{
    int success = chdir(pathname);
    if (success < 0) {
        sprintf(formatingString, "CD: Pathname could not be found.\n");
	printf("@> SERVER > CLIENT: %s", formatingString);
	write(sock, formatingString, 512);
        return 0;
    }
    return 1;
}
int mkdir_func(char pathname[], int sock)
{
    int success = 0;
    success = mkdir(pathname, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    if (success < 0) {
        sprintf(formatingString, "Error in creating directory %s\n", pathname);
	printf("@> SERVER > CLIENT: %s", formatingString);
	write(sock, formatingString, 512);
	return 0;
    }
    return 1;
}
int rmdir_func(char pathname[], int sock)
{
    int success = rmdir(pathname);
    if (success < 0) {
        sprintf(formatingString, "Error in removing directory %s.\n", pathname);
	printf("@> SERVER > CLIENT: %s", formatingString);
	write(sock, formatingString, 512);
        return 0;
    }
    return 1;
}
int rm_func(char pathname[], int sock)
{
    int success = unlink(pathname);
    if (success < 0) {
        sprintf(formatingString, "Error in removing file %s.\n", pathname);
	printf("@> SERVER > CLIENT: %s", formatingString);
	write(sock, formatingString, 512);
        return 0;
    }
    return 1;
}
int cat_func(char pathname[], int sock)
{
    char buffer[BLOCK_SIZE + 1];
    int readBytes = 0;
    int fd = open(pathname, O_RDONLY);
    
    if (fd <= 0) {
        sprintf(formatingString, "File %s could not be open for read mode.\n", pathname);
	printf("@> SERVER > CLIENT: %s", formatingString);
	write(sock, formatingString, 512);
        return 0;
    } else {
        while (readBytes = read(fd, buffer, BLOCK_SIZE)) {
            buffer[readBytes] = '\0';
            sprintf(formatingString, "%s", buffer);
	    printf("@> SERVER > CLIENT: %s", formatingString);
	    write(sock, formatingString, 512);
        }
    }
    return 1;
}
int get_func(char pathname[], int sock)
{
}
int put_func(char pathname[], int sock)
{
}

//=================================================
// Helper Functions
//=================================================

// LS helper functions
void printDetails (struct dirent* entry, int sock) {
    struct stat entryStat;
    struct passwd userPwdStruct;
    struct group userGroup;
    char permissions[INPUT_LENGTH] = {0};
    char userName[64] = {0};
    char groupName[64] = {0};
    char dateStr[128] = {0};
    char tmp[128];
    char tmpString[512];
    int fd = open(entry->d_name, O_RDONLY);
    
    if (fd == 0) {
        sprintf(tmpString, "file not found\n");
	printf("@> SERVER > CLIENT: %s", tmpString);
	write(sock, "512", 4);
	write(sock, tmpString, 512);
    }

    fstat(fd, &entryStat);
    
    getPermissionsString(&entryStat, permissions);
    
    userPwdStruct = *getpwuid(entryStat.st_uid);
    strcpy(userName, userPwdStruct.pw_name);
    
    userGroup = *getgrgid(entryStat.st_gid);
    strcpy(groupName, userGroup.gr_name);
    strftime(dateStr, 128, "%F %T", gmtime((const time_t*) &entryStat.st_atim));
    sprintf(tmpString, "%s %u %s %s %5u %s %s\n",
            permissions, 
            (unsigned int)entryStat.st_nlink, 
            userName, 
            groupName, 
            (unsigned int)entryStat.st_size,
            dateStr,
            entry->d_name
            );
    close(fd);
    strcat(formatingString, tmpString);
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

char getFileTypeChar(unsigned short fileType) {  
    switch(fileType) {
        case (10):
            //TODO:: Something is messed up here.
            return 'l';
        case (4):
            return 'd';
        default:
            return '-';
    }
}
