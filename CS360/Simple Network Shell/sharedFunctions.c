#include "sharedFunctionsHeader.h"

// Main operational functions ==========================================

int ls_func (char* pathname, FILE* outFile) {
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

int cd_func (char* pathname, FILE* outFile) {
    int success = chdir(pathname);
    if (success < 0) {
        fprintf(outFile, "CD: Pathname could not be found.\n");
        return 0;
    }
    return 1;
}


int pwd_func (char* pathname, FILE* outFile) {
    char buffer[256];
    getcwd(buffer, 256);
    fprintf(outFile, "%s\n", buffer);
}

int mkdir_func (char* pathname, FILE* outFile) {
    int success = 0;
    //struct stat myStat;
    //myStat.st_mode = 
    success = mkdir(pathname, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    if (success < 0) {
        fprintf(outFile, "Error in creating directory %s\n", pathname);
    }
}

int rmdir_func (char* pathname, FILE* outFile) {
    int success = rmdir(pathname);
    if (success < 0) {
        fprintf(outFile, "Error in removing directory %s.\n", pathname);
        return 0;
    }
    return 1;
}

int rm_func (char* pathname, FILE* outFile) {
    int success = unlink(pathname);
    if (success < 0) {
        fprintf(outFile, "Error in removing file %s.\n", pathname);
        return 0;
    }
    return 1;
}

int cat_func (char* pathname, FILE* outFile) {
    char buffer[BLOCK_SIZE + 1];
    int readBytes = 0;
    int fd = open(pathname, O_RDONLY);
    
    if (fd <= 0) {
        fprintf(outFile, "File %s could not be open for read mode.\n", pathname);
        return 0;
    } else {
        while (readBytes = read(fd, buffer, BLOCK_SIZE)) {
            buffer[readBytes] = '\0';
            fprintf(outFile, "%s", buffer);
        }
    }
    return 1;
}




// Helper Functions =======================================================
// LS helper functions
void printDetails (struct dirent* entry, FILE* outFile) {
    struct stat entryStat;
    struct passwd userPwdStruct;
    struct group userGroup;
    char permissions[INPUT_LENGTH] = {0};
    char userName[64] = {0};
    char groupName[64] = {0};
    char dateStr[128] = {0};
    int fd = open(entry->d_name, O_RDONLY);
    
    if (fd == 0) {
        fprintf(outFile, "file not found\n");
    }

    fstat(fd, &entryStat);
    
    getPermissionsString(&entryStat, permissions);
    
    userPwdStruct = *getpwuid(entryStat.st_uid);
    strcpy(userName, userPwdStruct.pw_name);
    
    //userPwdStruct = *getpwuid(entryStat.st_gid);
    userGroup = *getgrgid(entryStat.st_gid);
    strcpy(groupName, userGroup.gr_name);
    strftime(dateStr, 128, "%F %T", gmtime((const time_t*) &entryStat.st_atim));
    fprintf(outFile, "%s %u %s %s %5u %s %s\n",
            permissions, 
            (unsigned int)entryStat.st_nlink, 
            userName, 
            groupName, 
            (unsigned int)entryStat.st_size,
            dateStr,
            entry->d_name
            );
    close(fd);
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



// REMOVE THIS
int main () {
    return 0;
}

