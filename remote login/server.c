#include<stdio.h> 
#include<string.h> 
#include<stdlib.h> 
#include <unistd.h>
#include <sys/wait.h>
#include<arpa/inet.h>
#include<sys/socket.h>
 
#define BufferLength 10000  //Max length of buffer
#define PORT 8888   //The port on which to listen for incoming data

char* concat(const char *s1, const char *s2)
{
    char *result = malloc(strlen(s1)+strlen(s2)+1);
    strcpy(result, s1);
    strcat(result, s2);
    return result;
}

int main(void)
{
  struct sockaddr_in localsock, remotesock;
  int psock, sendlen=sizeof(remotesock),client=0,total_clients=5, validate,status,recvret;
  char buffer[BufferLength];
  char *send_back;
  //create a UDP socket
    if ((psock=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
      {
        printf("Problem in creating socket\n");
        exit(1);
      }
  memset((char *) &localsock, 0, sizeof(localsock));
  localsock.sin_family = AF_INET;
  localsock.sin_port = htons(PORT);
  localsock.sin_addr.s_addr = htonl(INADDR_ANY);
  //bind the server socket to a port
    if(bind(psock,(struct sockaddr*)&localsock, sizeof(localsock) ) == -1)
      {
        printf("Cannot bind socket to the port\n");
        exit(1);
      }
   //give client a connection and listen forever
   int pid;
   while(1)
   {
    memset(buffer,'\0', BufferLength);
    printf("Waiting for connection\n");
     if ((recvret = recvfrom(psock, buffer, BufferLength, 0, (struct sockaddr *) &remotesock, &sendlen)) == -1)
        {
          printf("problem receiving data from remote host\n");
          exit(1);  
        }
         printf("Received packet from %s:%d\n", inet_ntoa(remotesock.sin_addr), ntohs(remotesock.sin_port));
         printf("Data Received: %s\n", buffer);
         validate=strcmp(buffer,"connect");
         memset(buffer,'\0', BufferLength);
         if(validate==0)
          {
           client++;
           printf("Connection approved\n");
           send_back="OK!";
           strcpy(buffer,send_back);
         
              pid=fork();
              if(pid<0)
              {
               printf("\n Error ");
               exit(-1);
               }
              //child connection
              else if(pid==0)
               {
                 //recv data from remote system
                 close(psock);
                 int csock;
                 if ((csock=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
                  {
                     printf("Problem in creating socket\n");
                     exit(1);
                  } 
                  
                 struct timeval tv;
                 tv.tv_sec =10;
                 tv.tv_usec =0;   
                int link[2];
                char *rec="/bin/"; 
                int attempt=0;
                while(1) 
                {
                   if (pipe(link)==-1)
                    {
                      printf("Its error in pipe\n");
                      exit(1);
                    }
                 int cstatus,cpid,nbytes=0;
                 if (sendto(csock, buffer, BufferLength, 0, (struct sockaddr*) &remotesock, sendlen) == -1)
                  {
                    printf("Problem sending data to remote host\n");
                  }
                  memset(buffer,'\0', BufferLength);
                while(1)
                 {
                  setsockopt(csock,SOL_SOCKET,SO_RCVTIMEO,(char*)&tv,sizeof(tv));
                  recvret = recvfrom(csock, buffer, BufferLength, 0, (struct sockaddr *) &remotesock, &sendlen);
                 if (recvret < 0)
                  {
                   attempt++;
                   if(attempt==3)
                    { 
                     strcpy(buffer,"FINISH");
                     break;
                    } 
                  }
                 else
                  {
                   attempt=0;
                   break;
                  }
                 }//while time-out
                  validate=strcmp(buffer,"FINISH");
                  if(validate==0)
                   {
                     break;
                   }
                  else
                   {
                   char *str,*str1,*str2;
                   str=malloc(strlen(buffer)+1);
                   str=strtok(buffer," ");
                   str1=str;
                   str=strtok(NULL," ");
                   str2=str;
                   char* firstarg=concat(rec,str1);
                   if(strcmp(str1,"cd")==0)
                     {
                       printf("value chdir: %d\n",chdir(str2));
                       continue;
                     }
                   cpid=fork();
                   if(cpid<0)
                    {
                     printf("\n Error ");
                     exit(-1);
                    }
                  //child to execute the command
                   else if(cpid==0)
                    {
                     dup2 (link[1], STDOUT_FILENO);
                     close(link[0]);
                     close(link[1]);
                    if(execl (firstarg, str1, str2, (char *)0)==-1);
                      {
                       printf("Invalid command\n");
                       strcpy(buffer,"Invalid Command");
                       exit(0);
                      }
                    }
                    close(link[1]);
                    nbytes = read(link[0], buffer, sizeof(buffer));
                    printf("Output: (%.*s)\n", nbytes, buffer);
                    waitpid(pid,&status,WCONTINUED|WUNTRACED);
                   }//else                 
                  }//Latest-while
                   printf("Client %s:%d disconnected\n", inet_ntoa(remotesock.sin_addr), ntohs(remotesock.sin_port));
                  close(csock);
                 exit(0);
               }//outer else if
             }//if-validate
        //parent goes here
         else
          {
           printf("Connection rejected\n");
           send_back="NOT_OK!";
           strcpy(buffer,send_back);
            printf("Data Sent: %s\n", buffer);
           if (sendto(psock, buffer,BufferLength, 0, (struct sockaddr*) &remotesock, sendlen) == -1)
              {
                printf("Problem sending data to remote host\n");
              }
           continue;
          }//latest else
    waitpid(pid, &status, WUNTRACED | WCONTINUED | WNOHANG);
  }//parent while
  close(psock);
  return 0;
}
