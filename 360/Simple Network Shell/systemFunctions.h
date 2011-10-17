#ifndef __SYSTEMFUNCTIONS_H__
#define	__SYSTEMFUNCTIONS_H__

#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <string.h>
#include <dirent.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>

#define INPUT_LENGTH 256
#define BLOCK_SIZE 4096

char formatingString[256];
char returnString[4096];

void initializeFunctionList(void);
int checkClientCommand(char command[], char pathname[], int outFile);
int executeServerCommand(int functionIndex, char pathname[], int outFile);

int pwd_func(char pathname[], int outFile);
int ls_func(char pathname[], int outFile);
int cd_func(char pathname[], int outFile);
int mkdir_func(char pathname[], int outFile);
int rmdir_func(char pathname[], int outFile);
int rm_func(char pathname[], int outFile);
int cat_func(char pathname[], int outFile);
int get_func(char pathname[], int outFile);
int put_func(char pathname[], int outFile);
int quit_func(char pathname[], int outFile);

// LS helpers
void printDetails (struct dirent* entry, int outFile);
void getPermissionsString (struct stat *status, char *buffer);
void getIndPerminssion(char* ret, unsigned short ind);
char getFileTypeChar(unsigned short fileType);

int fileNameIndex(char * pathname);

#endif //__SYSTEMFUNCTIONS_H__
