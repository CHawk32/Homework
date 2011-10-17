#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/socket.h>
#include <netdb.h>
#include <sys/stat.h>

#include "../systemFunctions.h"

#define  MAX 512

// Define variables:
struct sockaddr_in  server_addr, client_addr, name_addr;
struct hostent *hp;

int  sock, newsock;                  // socket descriptors
int  serverPort;                     // server port number
int  r, length, n;                   // help variables

// Server initialization code:

int server_init(char *name)
{
   printf("==================== server init ======================\n");
   // get DOT name and IP address of this host

   printf("1 : get and show server host info\n");
   hp = gethostbyname(name);
   if (hp == 0){
      printf("unknown host\n");
      exit(1);
   }
   printf("    hostname=%s  IP=%s\n",
               (char*)hp->h_name,  (char*)(inet_ntoa(*(long *)hp->h_addr)));

   //  create a TCP socket by socket() syscall
   printf("2 : create a socket\n");
   sock = socket(AF_INET, SOCK_STREAM, 0);
   if (sock < 0){
      printf("socket call failed\n");
      exit(2);
   }

   printf("3 : fill server_addr with host IP and PORT# info\n");
   // initialize the server_addr structure
   server_addr.sin_family = AF_INET;                  // for TCP/IP
   server_addr.sin_addr.s_addr = htonl(INADDR_ANY);   // THIS HOST IP address
   server_addr.sin_port = 0;   // let kernel assign port

   printf("4 : bind socket to host info\n");
   // bind syscall: bind the socket to server_addr info
   r = bind(sock,(struct sockaddr *)&server_addr, sizeof(server_addr));
   if (r < 0){
       printf("bind failed\n");
       exit(3);
   }

   printf("5 : find out Kernel assigned PORT# and show it\n");
   // find out socket port number (assigned by kernel)
   length = sizeof(name_addr);
   r = getsockname(sock, (struct sockaddr *)&name_addr, &length);
   if (r < 0){
      printf("get socketname error\n");
      exit(4);
   }

   // show port number
   serverPort = ntohs(name_addr.sin_port);   // convert to host ushort
   printf("    Port=%d\n", serverPort);

   // listen at port with a max. queue of 5 (waiting clients)
   printf("5 : server is listening ....\n");
   listen(sock, 5);
   printf("===================== init done =======================\n");
}


main(int argc, char *argv[])
{
    int fileSize = 0, fd = 0, n =0;
   char *hostname;
   char line[MAX], buff[256], functionName = 0, pathName[256] = {0}, fileName[256] = {0};

   if (argc < 2)
      hostname = "localhost";
   else
      hostname = argv[1];

   chdir("../ServerRoot");
   getcwd(buff, 256);
   //chroot(buff);
 
   server_init(hostname);
   initializeFunctionList();

   // Try to accept a client request
   while(1){
     printf("server: accepting new connection ....\n");

     // Try to accept a client connection as descriptor newsock
     length = sizeof(client_addr);
     newsock = accept(sock, (struct sockaddr *)&client_addr, &length);
     if (newsock < 0){
        printf("server: accept error\n");
        exit(1);
     }
     printf("server: accepted a client connection from\n");
     printf("-----------------------------------------------\n");
     printf("        IP=%s  port=%d\n", (char*)inet_ntoa(client_addr.sin_addr.s_addr),
                                        ntohs(client_addr.sin_port));
     printf("-----------------------------------------------\n");

     // Processing loop
     while(1)
     {
       printf("@> ");
       n = read(newsock, line, MAX);
       if (n==0)
       {
           printf("server: client died, server loops\n");
           close(newsock);
           break;
       }

      //grab function number and pathname from the client input
       if(line[0] == 'P') //Server receives a put command from client
       {
           strtok(line, " ");
           fileSize = atoi(strtok(NULL, " "));
           strcpy(fileName, strtok(NULL, " "));
           fd = open(fileName, O_WRONLY | O_CREAT, 0644);
           if(fd == 0){
               write(newsock, "F", 2);
           }
           else{
               write(newsock, "S", 2);
               printf("Reading file from client...\n");
               while(fileSize > 0)
               {
                   n = read(newsock, returnString, 4096);
                   
                   write(fd, returnString, n);
                   fileSize -= n;
               }
               printf("@> File '%s' from client uploaded...\n", fileName);
               close (fd);
           }
           

       }
       else if(line[0] == 'G') //Server receives a get command from client
       {
            int fd = 0, i = 0, n = 0;
            struct stat fileStatus;
            char buff[16];
            char fileName[256] = {0};

            strtok(line, " ");
            strcpy(fileName, strtok(NULL, " "));

            fd = open(fileName, O_RDONLY);

            if(fd == 0){
                printf("Server File Not Found.");
                write(newsock, "F", 2);
            }
            else
            {
                fstat(fd, &fileStatus);

                sprintf(buff, "S %d", 74);
                write(newsock, buff, 16);
                printf("Sending %s to client...\n", fileName);
                i = fileStatus.st_size;
                while(i > 0)
                {
                    n = read(fd, returnString, 4096);
                    write(newsock, returnString, strlen(returnString));
                    i -= n;
                }
                printf("@> Transmission of %d bytes complete...\n", (int) fileStatus.st_size);
            }
       }
       else
       {
          functionName = atoi(&line[0]);
          if(line[1] == ' ')
          {
              strcpy(pathName, &line[2]);
          }
          else
          {
              pathName[0] = '\0';
          }
          printf("Server: client sent func: %d | path: %s\n", functionName, pathName);
          executeServerCommand(functionName, pathName, newsock);
       }
       bzero(line, MAX);
    }
 }
}

