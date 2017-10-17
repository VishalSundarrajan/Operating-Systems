#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>
#include <string.h>
#include <stdio.h>
#include<stdio.h>  
#include<stdlib.h> 



#define HELLO_PORT 12345
#define HELLO_GROUP "225.0.0.37"
#define PORT 9999 

int main(int argc, char *argv[])
{
     struct sockaddr_in m_addr,localsock,remotesock;
     int mcast_sock,mysock, cnt,BUFFERLENGTH=10000,slen=sizeof(remotesock);
     struct ip_mreq mreq;
     char message[BUFFERLENGTH];

     /* create what looks like an ordinary UDP socket */
     if ((mcast_sock=socket(AF_INET,SOCK_DGRAM,0)) < 0) {
	  perror("socket");
	  exit(1);
     }
    
      if ((mysock=socket(AF_INET,SOCK_DGRAM,0)) < 0) {
	  perror("socket");
	  exit(1);
     }

    localsock.sin_family = AF_INET;
    localsock.sin_port = htons(PORT);
    localsock.sin_addr.s_addr = htonl(INADDR_ANY);

    if(bind(mysock,(struct sockaddr*)&localsock, sizeof(localsock) ) == -1)
      {
        printf("Cannot bind socket to the port\n");
        exit(1);
      }

    while(1)
    {
     memset(message,'\0',BUFFERLENGTH);
     if (recvfrom(mysock,message, BUFFERLENGTH, 0, (struct sockaddr*) &remotesock, &slen) == -1)
        {
          printf("problem receiving data from remote host\n");
          exit(1);  
        }

     /* set up destination address */
     memset(&m_addr,0,sizeof(m_addr));
     m_addr.sin_family=AF_INET;
     m_addr.sin_addr.s_addr=inet_addr(HELLO_GROUP);
     m_addr.sin_port=htons(HELLO_PORT);
     
     
	  if (sendto(mcast_sock,message,BUFFERLENGTH,0,(struct sockaddr *) &m_addr,
		     sizeof(m_addr)) < 0) {
	       perror("sendto");
	       exit(1);
	  }
    }//while  
    
}
