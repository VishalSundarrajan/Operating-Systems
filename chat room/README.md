# **Synopsis**

#### **About Chat Room** 
 An concurrent broadcaster which accepts messages from multiple clients and broadcasts it to all the clients in the chat room over UDP sockets. Everytime a message is received the broadcaster broadcasts except the client that generates the message (similar to L2 switch ARP). Every clients run two threads, one to receive and other to send simultaneously.

#### Building the project

Go to project directory and run the Command

  `make `
  
  To start the server
  
 ` ./broadcaster`

  To run the sample client, (more number of clients can be joined)
  
  `./person1`
  
  To clean the executables,
  
  `make clean`
  
####  How to join the chat room

  To join the chat room, send the message to the broadcaster address
   
  To disconnect from the server, close the client application
   
   *Note : If a particular client is idle for 30 seconds then it will be automatically disconnected from the server.*
   
