#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netdb.h>

#include "clientFunctions.h"

#define MAX 512

// Define variables
struct hostent *hp;
struct sockaddr_in  server_addr;

int sock, r;
int SERVER_IP, SERVER_PORT;

// clinet initialization code

int client_init(char *argv[])
{
  //get server info
  hp = gethostbyname(argv[1]);
  if (hp==0){
     printf("unknown host %s\n", argv[1]);
     exit(1);
  }

  SERVER_IP   = *(long *)hp->h_addr;
  SERVER_PORT = atoi(argv[2]);

  //create a TCP socket
  sock = socket(AF_INET, SOCK_STREAM, 0);
  if (sock<0){
     printf("socket call failed\n");
     exit(2);
  }

  //fill server_addr with server's IP and PORT
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = SERVER_IP;
  server_addr.sin_port = htons(SERVER_PORT);

  //connect to server
  r = connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr));
  if (r < 0){
     printf("connect failed\n");
     exit(1);
  }

  printf("---------------------------------------------------------\n");
  printf("Connected to hostname=%s  IP=%s  PORT=%d\n",
          (char*)hp->h_name, (char*)inet_ntoa(SERVER_IP), SERVER_PORT);
  printf("---------------------------------------------------------\n");

  //Initialize the list of function pointers
  initializeFunctionList();

}

main(int argc, char *argv[ ])
{
  int n, i = 0, commandKnown = 0, remoteFunctionNum = -1;
  char tmp = 0;
  char line[MAX], output[MAX], command[16], pathname[256], outputSize[MAX];
  char result[256] = {0};

  if (argc < 3)
  {
     printf("Usage : client ServerName SeverPort\n");
     exit(1);
  }

  chdir("../ClientRoot");
  getcwd(pathname, 256);
  //chroot(pathname);
  
  client_init(argv);

  //processing loop
  while (1) 
  {
	
	printf("I AM RIGHT HERE:");
        bzero(line, MAX); // zero out line[ ]
	//Print shell identifier
        printf("#> ");
        fgets(line, MAX, stdin); // get a line (end with \n) from stdin
	

        line[strlen(line) - 1] = '\0'; // kill \n at end
        if (line[0] != 0) // exit if NULL line
        {
	    i = 0;
            //Get command and pathname from user input
            while (1)
            {
                tmp = line[i];
                if(tmp == ' ')
                {
                    // There is a pathname
                    line[i] = '\0';
                    strcpy(command, line);
                    strcpy(pathname, &line[i+1]);
                    break;
                }
                else if (tmp == '\0') {
                    // No pathname
                    pathname[0] = '\0';
                    strcpy(command, line);
                    break;
                }
                i++;
            }
	    remoteFunctionNum = 0;
	    remoteFunctionNum = isClientCommand(command, pathname); //returns > -1 if remote function call
            if(remoteFunctionNum > -1)	
            //If remoteFunctionNum is greater than -1 we know this is a server function call
	    {
		//send server the function numnber and name
		sprintf(line, "%d %s", remoteFunctionNum, pathname); 
		n = write(sock, line, MAX);

		if(remoteFunctionNum == 1 || remoteFunctionNum == 7 || remoteFunctionNum == 8) //these server functions require a size sent to client first
		{
			outputSize[0] = '\0';
			output[0] = '\0';
			read(sock, outputSize, MAX); //get size of output from server
			read(sock, output, atoi(outputSize)); //read from sock up to outPut size into output
			printf("%s", output);
			
		}
		else
		{
			//Wait for response from server
	       		n = read(sock, output, MAX); //read a line from sock
			printf("%s", output);
		}	
	    }
        }

  }
}


