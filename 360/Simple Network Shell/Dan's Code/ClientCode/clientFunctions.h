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

void initializeFunctionList(void);
int isClientCommand(char command[], char pathname[]);

int pwd_func(char pathname[]);
int ls_func(char pathname[]);
int cd_func(char pathname[]);
int mkdir_func(char pathname[]);
int rmdir_func(char pathname[]);
int rm_func(char pathname[]);
int cat_func(char pathname[]);
int get_func(char pathname[]);
int put_func(char pathname[]);
int quit_func(char pathname[]);

// LS helpers
void printDetails (struct dirent* entry);
void getPermissionsString (struct stat *status, char *buffer);
void getIndPerminssion(char* ret, unsigned short ind);
char getFileTypeChar(unsigned short fileType);

#endif
