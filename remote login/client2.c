#include<stdio.h> 
#include<string.h> 
#include<stdlib.h> 
#include<unistd.h>
#include<arpa/inet.h>
#include<sys/socket.h>
 
#define SERVER "127.0.0.1" //server IP
#define BUFFERLENGTH 1000  //Max length of buffer
#define PORT 8888   //server port

int main (void)
{
   struct sockaddr_in mysock;
   struct timeval tv;
   tv.tv_sec =2;
   tv.tv_usec =0;
   int sock, slen=sizeof(mysock),exitStatus;
   char buffer[BUFFERLENGTH];
   char message[BUFFERLENGTH];
   if ((sock=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
    {
        printf("Error creating socket\n");
    }
   memset((char *) &mysock, 0, sizeof(mysock));
   mysock.sin_family = AF_INET;
   mysock.sin_port = htons(PORT);
     if (inet_aton(SERVER , &mysock.sin_addr) == 0) 
     {
        printf("inet_aton() failed\n");
        exit(1);
     }
     int attempt=0;
  while(1)
    {
        printf("Enter message to send : \n");
        scanf("%[^\n]s",message);
        fflush(stdin);
        fgetc(stdin);
        //send the message
        if (sendto(sock, message, strlen(message) , 0 , (struct sockaddr *) &mysock, slen)==-1)
        {
            printf("Error in sendto()\n");
        }
        exitStatus=strcmp(message,"FINISH");
         if(exitStatus==0)
           break;
        int len=strlen(message);
        memset(buffer,'\0', BUFFERLENGTH);
        //try to receive some data, this is a blocking call
        setsockopt(sock,SOL_SOCKET,SO_RCVTIMEO,(char*)&tv,sizeof(tv));
        int recvret=recvfrom(sock, buffer, BUFFERLENGTH, 0, (struct sockaddr *) &mysock, &slen);
        if (recvret >= 0)
        {
             printf("The data received: %s\n",buffer);
             attempt=0;
             printf("The value: %d\n",recvret);
        }
     
       else
       {
            printf("Timed out: %d\n",recvret);
            attempt++;
              if(attempt==3)
                break;
            continue;
       }
    }
    printf("Server might be busy, Try again sometimes later\n");
    close(sock);
    return 0;
}

