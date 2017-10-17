#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>
#include <string.h>
#include <stdio.h>
#include<stdlib.h> 
#include <pthread.h>


#define HELLO_PORT 12345
#define HELLO_GROUP "225.0.0.37"
#define MSGBUFSIZE 10000
#define SERVER "127.0.0.1" //server IP
#define PORT 9999   //server port

char msgbuf[MSGBUFSIZE];
struct sockaddr_in addr,mcst_servinfo;
int fd, nbytes,addrlen,my_sock,slen=sizeof(mcst_servinfo),totalthreads=1;
struct ip_mreq mreq;


void *sendb_msg()
{
     while(1)
     {
        printf("Enter message to send : \n");
        scanf("%[^\n]s",msgbuf);
        fflush(stdin);
        fgetc(stdin);
        if (sendto(my_sock, msgbuf, strlen(msgbuf) , 0 , (struct sockaddr *) &mcst_servinfo, slen)==-1)
         {
            printf("Error in sendto()\n");
         }
    
          memset(msgbuf,'\0', MSGBUFSIZE); 
     }//while
}

int main(int argc, char *argv[])
{

     if ((my_sock=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
      {
        printf("Error creating socket\n");
      }
      memset((char *) &mcst_servinfo, 0, sizeof(mcst_servinfo));
      mcst_servinfo.sin_family = AF_INET;
      mcst_servinfo.sin_port = htons(PORT);
      if (inet_aton(SERVER , &mcst_servinfo.sin_addr) == 0) 
       {
        printf("inet_aton() failed\n");
        exit(1);
       }



     u_int yes=1;           

     /* creates what looks like an ordinary UDP socket */
     if ((fd=socket(AF_INET,SOCK_DGRAM,0)) < 0) {
	  perror("socket");
	  exit(1);
     }



    /* allow multiple sockets to use the same PORT number */
    if (setsockopt(fd,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof(yes)) < 0) {
       perror("Reusing ADDR failed");
       exit(1);
       }


     /* set up destination address */
     memset(&addr,0,sizeof(addr));
     addr.sin_family=AF_INET;
     addr.sin_addr.s_addr=htonl(INADDR_ANY); /* N.B.: differs from sender */
     addr.sin_port=htons(HELLO_PORT);
     
     /* bind to receive address */
     if (bind(fd,(struct sockaddr *) &addr,sizeof(addr)) < 0) {
	  perror("bind");
	  exit(1);
     }
     
     /* use setsockopt() to request that the kernel join a multicast group */
     mreq.imr_multiaddr.s_addr=inet_addr(HELLO_GROUP);
     mreq.imr_interface.s_addr=htonl(INADDR_ANY);
     if (setsockopt(fd,IPPROTO_IP,IP_ADD_MEMBERSHIP,&mreq,sizeof(mreq)) < 0) {
	  perror("setsockopt");
	  exit(1);
     }
   //logic starts here
    int t,rc;
    pthread_t thread[totalthreads];

    for (t=0;t<totalthreads;t++)
    {
     rc=pthread_create(&thread[t],NULL,sendb_msg,NULL);
    }  
   
    while(1)
       {
	  addrlen=sizeof(addr);
	  if ((nbytes=recvfrom(fd,msgbuf,MSGBUFSIZE,0,
			       (struct sockaddr *) &addr,&addrlen)) < 0) {
	       perror("recvfrom");
	       exit(1);
	  }
	  printf("The received Data is: %s\n",msgbuf);
         memset(msgbuf,'\0', MSGBUFSIZE);
       }//while
     
    for(t=0;t<totalthreads;t++)
     {
      rc=pthread_join(thread[t],NULL);
        if(rc)
        {
         printf("\nError");
         exit(-1);
        }
     }
}
