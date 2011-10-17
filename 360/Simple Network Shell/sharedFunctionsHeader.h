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


int ls_func (char* pathname, FILE* outFile);
int cd_func (char* pathname, FILE* outFile);
int pwd_func (char* pathname, FILE* outFile);
int mkdir_func (char* pathname, FILE* outFile);
int rmdir_func (char* pathname, FILE* outFile);
int rm_func (char* pathname, FILE* outFile);
int cat_func (char* pathname, FILE* outFile);

// LS helpers
void printDetails (struct dirent* entry, FILE*  outFile);
void getPermissionsString (struct stat *status, char *buffer);
void getIndPerminssion(char* ret, unsigned short ind);
char getFileTypeChar(unsigned short fileType);

