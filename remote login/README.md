# **Synopsis**

#### **About Remote Login** 
 An concurrent server that accept commands (requests) from multiple credible clients, processes it locally and sends back the data to the corresponding clients over UDP sockets. Each time an request arrives, the server spawns a new process and the process runs until the client disconnects from the server. Clients can view or modify server files by sending linux command (ls, cd, exit, etc..,). The server is running on the port 8888 and uses the local host IP address.

#### Building the project

Go to project directory and run the Command

  `make `
  
  To start the server
  
 ` ./server`

  To run the sample client,
  
  `./client`
  
  To clean the executables,
  
  `make clean`
  
####  How to connect to server

In order to connect, a client should send the command 

   ` connect `
   
   To disconnect from the server,
   
   `FINISH`
   
   *Note : If a particular client is idle for 30 seconds then it will be automatically disconnected from the server.*
   