#include "FTPClient.h"
#include <fstream>
#include <sys/poll.h>
#define BUFSIZE 8192

//-----------------------------------------------------------------------------
FTPClient::FTPClient() {
    clientSD = 0;
    char url[] = "ftp.tripod.com";
    char username[] = "css432";
    char password[] = "UWB0th3ll";
    while(open(url, 21) == 0){
	   std::cout << "Cant connect\n";
    }
    int code = login(username, password);
    if(code == 230)
        std::cout << "FTP>\tClient logged in" << std::endl;
    else
	//std::cout << "Code was: " << code << std::endl;
    exit(1);
}

//-----------------------------------------------------------------------------
FTPClient::~FTPClient() {
}

//-----------------------------------------------------------------------------
int FTPClient::open(char* hostName, int port) {
    //Setup
    char buffer_in[1450];
    // char buf[1450];

    // for(int i = 0; i < 1448; i++)
	// buffer_in[i] = '\0'; 

    bzero(buffer_in,1450);

    // Attempt to connect to server
    sock = new Socket(port);
    clientSD = sock->getClientSocket(hostName);
    //std::cout << "sd is" << clientSD << std::endl;

    // Read Connection acknoledgement from server.
    
    // if ( val > 0 ) {                  // the socket is ready to read
      // char buf[1450];
      // int nread = read( clientSD, buf, 1450 ); // guaranteed to return from read 
                                           // even if nread < BUFLEN
      // int nread = read( sd, buf, BUFLEN ); // guaranteed to return from read
    // }
    //read(clientSD, buffer_in, 1450);
    //std::cout << "FTP>\t" << buffer_in;
    strcpy( buffer_in, recvMessage() );
    std::cout << "FTP>\t" << buffer_in << std::endl;
    while(getReturnCode(buffer_in) != 220) {
	   std::cout << "Incorrect hostname and port: Enter new one" <<  std::endl;
       strcpy( buffer_in, recvMessage() );
    }
 
    return clientSD; //true if > 0
}

// //-----------------------------------------------------------------------------
// int FTPClient::close() {
//     std::cout << "\nConsider yourself STUBBED!! (close)";
//     //Close the connection o.o
//     return true;
// }

//-----------------------------------------------------------------------------
void FTPClient::quit() {
    std::cout << "\nConsider yourself STUBBED!! (quit)";
    //Close the connection, of course
    close(clientSD);
    //Quit the program, bro
    exit(1);
}

//-----------------------------------------------------------------------------
int FTPClient::login(char *username, char *password) { 
    int code = sendUserName(username);
    //std::cout << "Code " << code << std::endl;
    while(code != 331) {
   	    std::cout << "Incorrect username. Reenter:";
	   std::cin >> username;
	   code = sendUserName(username);
	   std::cout << std::endl;
    }
    //std::cout <<"Good username" << std::endl;

    code = sendPassword(password);
    while(code != 230) {
 	   std::cout << "Incorrect password. Reenter:";
	   std::cin >> password;
	   code = sendPassword(password);
    }

    char buffer[BUFSIZE];
    bzero(buffer,BUFSIZE);

    strcpy(buffer,recvMessage() );

    std::cout << buffer << std::endl;

    return code;
}

//-----------------------------------------------------------------------------
//Returns 331 if successful
int FTPClient::sendUserName(char* username) {
    //std::cout << "sendUserName()" << std::endl;
    memset( ctrlBuf, '\0', sizeof( ctrlBuf ) );
    recvBytes = 0;
    int code;
    char buffer[100];
    strcpy(buffer, "USER ");
    strcat(buffer, username);

    //Send the message!
    int i;

    if((i = sendMessage(buffer)) < 0) {
	   perror("Can't send message\n");
	   return 1;
    }
    //std::cout << "sendMessage() returns " << i << std::endl;
    
    strcpy(buffer, recvMessage());
    //std::cout << "Recieved message: " << buffer << std::endl;
    //std::cout <<  std::endl;
    return getReturnCode(buffer);
}

//-----------------------------------------------------------------------------
//get the first 3 chars from the received message and conver to an int
int FTPClient::getReturnCode( char *message) {
    char temp[4];
    if(message == NULL || strlen(message) <= 3)
	   return 0;
    for(int i = 0; i < 3; i++)
   	    temp[i] = message[i];
    temp[3] = '\0';
    return atoi(temp);
}

//-----------------------------------------------------------------------------
//Returns 230 if successful
int FTPClient::sendPassword(char* password) {
    int code;
    char buffer[2048];
    strcpy(buffer, "PASS ");
    strcat(buffer, password);
    if(sendMessage(buffer) < 0) {
	   perror("Can't send message\n");
    	return 1;
    }    
    strcpy(buffer, recvMessage());
    //std::cout << buffer << std::endl;
    return getReturnCode(buffer);
}

//-----------------------------------------------------------------------------
int FTPClient::sendMessage(char* buffer) {

    int send_length = strlen(buffer);
    char message[send_length+2];
    memset( message, '\0', sizeof( message ) );
    strcpy(message, buffer);
    strcat(message,  "\r\n");

    int size = write(clientSD, message, send_length+2);

    return size;
}

//-----------------------------------------------------------------------------
int FTPClient::sendPASV(){

    // std::cout << "in sendPASV" << std::endl;

    char buffer[70];                            //allocate buffer
    bzero(buffer, 70 );                         //zero out buffer

    sendMessage("PASV ");                       //send the message PASV
    strcpy( buffer, recvMessage() );            //receive reply

    int port = getPortFromPASV( buffer );           //get port # from reply

    std::cout << "port: " << port << std::endl;

    char url[] = "ftp.tripod.com";              //set the url to connect to

    serverSock = new Socket(port);              //new socket with new port #
    dataSD = serverSock->getClientSocket(url);  //new data socket

    return 0;
}

//-----------------------------------------------------------------------------
char* FTPClient::recvMessage() {
    // std::cout << "recvMessage()" << std::endl;
    struct pollfd ufds;
    char *buffer;
    buffer = new char[BUFSIZE];
    std::string message;
    bzero(buffer, BUFSIZE);
    char *retMsg;
    int msg_size = 0;
    bool recieve = false; 

    char buffer_in[BUFSIZE];
    for(int i = 0; i < BUFSIZE; i++){
        buffer_in[i] = '\0';
    }

    while(1){
        memset(buffer, '\0', BUFSIZE); //clear out buffer for next read 
        msg_size = read(clientSD, buffer, BUFSIZE);
        if(msg_size > 0) {
           message.append(buffer);
        }

        if(buffer[msg_size-1] == '\n' && buffer[msg_size-2] == '\r'){
            break; 
        }
    }

    char error[] = "Error receiving message";
    if( msg_size > 0) {
  	    retMsg = new char[message.length() + 1];
	    strcpy(retMsg, message.c_str());
	    int msg_size = strlen(retMsg);
	    for(int i = msg_size - 1; i > msg_size - 3; i--)
	        if(retMsg[i] == '\n' || retMsg[i] == '\r')
		        retMsg[i] = '\0';
	    return retMsg;
    }
	return '\0';
}

//-----------------------------------------------------------------------------
int FTPClient::getPortFromPASV( char* buffer ) {

    std::cout << "in getPortFromPASV" << std::endl;

    std::cout << buffer << std::endl;
    int address[6];

    for(int i=0; i<6; i++)
        address[i] = 0;

    char tempBuf[4];
    bzero(tempBuf, 6);

    char tempChar;
    int j = 0,k = 0;

    for(int i=20; i < strlen(buffer) ;i++ ) {
        if(buffer[i] == ',' || buffer[i] == ')') {
            // std::cout << tempBuf << std::endl;
            address[j] = atoi(tempBuf);
            j++;
            k = 0;
            bzero(tempBuf, 6);
            //tempBuf = new std::string;
        }
        else {
            if( isdigit( buffer[i] )) {
                // std::cout << buffer[i] << std::endl;
                tempBuf[k] = buffer[i];
                k++;
            }

        }
    }

    // for( int i = 0; i < 6 ; i++ ) {
    //     std::cout << i << ": " << address[i] << std::endl;
    // }
    int port = address[4] * 256 + address[5];

    return port;
}

//-----------------------------------------------------------------------------
bool FTPClient::changeDir(char* dirName) {
    std::cout << "\nConsider yourself STUBBED!! (changeDir)";
    return true;
}

//-----------------------------------------------------------------------------
//List command
char* FTPClient::getCurrentDirContents() {
    int code;
    char* dataptr;
    char* msgptr; 
    char buffer[BUFSIZE];
    bzero(buffer, BUFSIZE);

    //get PASV connection setup with dataSD
    sendPASV(); 

    //save clientSD temporarily, we will need it                
    int tempSD = clientSD;      

    //add LIST to buffer to be sent
    strcpy(buffer, "LIST");                 

    //send LIST, output error if there was one, message sent on clientSD
    if(sendMessage(buffer) < 0) {
       perror("Can't send message\n");
        return NULL;
    }  

    //Get message from server
    msgptr = recvMessage();
    std::cout << msgptr << std::endl;


    //set clientSD to dataSD
    clientSD = dataSD; 

    //recieve data buffer from server
    dataptr = recvMessage();
    std::cout << dataptr << std::endl;

    //set clientSD to itself again
    clientSD = tempSD;   

    //Recieve end of stream message
    msgptr = recvMessage();  
    std::cout << msgptr << std::endl;

    //close PASV connection
    close( dataSD );                        

    //return buffer with directory contents
    return dataptr;

}


//-----------------------------------------------------------------------------
bool FTPClient::getFile(char* fileName) {
    std::cout << "\nConsider yourself STUBBED!! (getFile)";
    return true;
}
//-----------------------------------------------------------------------------
bool FTPClient::putFile(char* fileName) {
    std::cout << "\nConsider yourself STUBBED!! (putFile)";
    return true;
}
//-----------------------------------------------------------------------------
bool FTPClient::listDir( char* pathname ) {
    std::cout << "\nConsider yourself STUBBED!! (listDir)";
    sendPASV();
    return true;
}
