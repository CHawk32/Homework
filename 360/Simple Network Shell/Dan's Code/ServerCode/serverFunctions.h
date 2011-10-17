#ifndef CLIENTFUNCTIONS_H
#define	CLIENTFUNCTIONS_H

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

char formatingString[512];

void initializeFunctionList(void);
int executeServerCommand(int functionIndex, char pathname[], int sock);

int pwd_func(char pathname[], int sock);
int ls_func(char pathname[], int sock);
int cd_func(char pathname[], int sock);
int mkdir_func(char pathname[], int sock);
int rmdir_func(char pathname[], int sock);
int rm_func(char pathname[], int sock);
int cat_func(char pathname[], int sock);
int get_func(char pathname[], int sock);
int put_func(char pathname[], int sock);

// LS helpers
void printDetails (struct dirent* entry, int sock);
void getPermissionsString (struct stat *status, char *buffer);
void getIndPerminssion(char* ret, unsigned short ind);
char getFileTypeChar(unsigned short fileType);

#endif
