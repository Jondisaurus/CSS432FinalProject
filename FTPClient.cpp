#include "FTPClient.h"
#include <fstream>
#define BUFSIZE 8192
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
        std::cout << "FTP>\tClient logged in\n" << std::endl;
    else
	//std::cout << "Code was: " << code << std::endl;
    exit(1);
}

FTPClient::~FTPClient() {
}

int FTPClient::open(char* hostName, int port) {
    //Setup
    char buffer_in[1450];

    for(int i = 0; i < 1448; i++)
	buffer_in[i] = '\0'; 

    // Attempt to connect to server
    sock = new Socket(port);
    clientSD = sock->getClientSocket(hostName);
    //std::cout << "sd is" << clientSD << std::endl;

    // Read Connection acknoledgement from server.
    read(clientSD, buffer_in, 1450);
    std::cout << "FTP>\t" << buffer_in;
    while(getReturnCode(buffer_in) != 220) {
	   std::cout << "Incorrect hostname and port: Enter new one" <<  std::endl;
    }
 
    return clientSD; //true if > 0
}

int FTPClient::close() {
    std::cout << "\nConsider yourself STUBBED!!";
    //Close the connection o.o
    return true;
}

void FTPClient::quit() {
    //Close the connection, of course
    close();
    //Quit the program, bro
    exit(1);
}

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

    return code;
}

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

int FTPClient::getReturnCode( char *message) {
    char temp[4];
    if(message == NULL || strlen(message) <= 3)
	   return 0;
    for(int i = 0; i < 3; i++)
   	    temp[i] = message[i];
    temp[3] = '\0';
    return atoi(temp);
}

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
/*
int FTPClient::sendMessage(){
    memset( ctrlBuf, '\0', sizeof( ctrlBuf ) );
    ctrlBuf[send_bytes] = 0
}
*/
int FTPClient::sendMessage(char* buffer) {

    int send_length = strlen(buffer);
    char message[send_length+2];
    memset( message, '\0', sizeof( message ) );
    strcpy(message, buffer);
    strcat(message,  "\r\n");

    //message[send_length] = 0; 
    //strcat(message, "\0");
    //std::cout << "Sending Message: " << strlen(message) << " " << std::endl;
    //std::cout << message << std::endl;
    int size = write(clientSD, message, send_length+2);
    //std::cout << "Size: " << size << std::endl;
    return size;
}



#if 0
void* FTPClient::waitForMessage(void *ptr) {
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
    try {
	while(true) {
	    std::cout << "Received in thread: ";
	    std::cout << recvMessage();
	}
    }
    catch(Exception ex) {
  	std::cout << "killing thread";
    }
}
#endif
char* FTPClient::recvMessage() {
    //std::cout << "recvMessage()" << std::endl;
    char *buffer;
    buffer = new char[BUFSIZE];
    std::string message;
    bzero(buffer, BUFSIZE);
    char *retMsg;
    int size = 0;

    char buffer_in[BUFSIZE];
    for(int i = 0; i < BUFSIZE; i++){
        buffer_in[i] = '\0';
    }
    do {

	    size = recv(clientSD, buffer_in, BUFSIZE -1,0);
	    //std::cout << "Size: " << size << std::endl;
 	    //std::cout << buffer_in << std::endl;

//	}
	   if(size > 0) {
	       message.append(buffer_in);
           //std::cout << message << std::endl; 
	   }
	   else {
	       break;
	   }
    } while(size < 0);

    //std::cout << "Just recieved " << message << std::endl;

    char error[] = "Error receiving message";
    if( size > 0) {
  	    retMsg = new char[message.length() + 1];
	    strcpy(retMsg, message.c_str());
	    int size = strlen(retMsg);
	    for(int i = size - 1; i > size - 3; i--)
	        if(retMsg[i] == '\n' || retMsg[i] == '\r')
		        retMsg[i] = '\0';
    //std::cout << "retMsg:  " << retMsg << std::endl; 
	    return retMsg;
    }
    //else if (size == 0)
	return '\0';
    //else throw (new Exception (error, RECV_EXCEPTION));
}


#if 0
char* FTPClient::recvMessage() {
    struct pollfd ufds;
    bool receive = false; 
    char recvBuf[2048];

    ufds.fd = clientSD;
    ufds.events = POLLIN;
    ufds.revents = 0; 
    int val = poll(&ufds,1,1000);

    while(val < 0){
        val = poll(&ufds,1,1000);
        std::cout << "val: " << val << std::endl; 
    }

    std::cout << "recieving msg" << std::endl; 
    //if(receive){
        recvBytes = read(clientSD, recvBuf, 2047);
    //}
    //for(int i = 0; i == BUFSIZE; i++){
        //cntlBuf[i] = 0;
    //}
    std::cout << "msg recieved: " << recvBuf << std::endl;

    /*
    do{/// isnt this about the same as polling?
        recvBytes = read(clientSD, ctrlBuf, BUFSIZE);
    }while(recvBytes == 0);
    */
    recvBuf[recvBytes] = 0;
    std::cout << recvBuf << std::endl; 
    return ctrlBuf;
}
#endif
bool FTPClient::changeDir(char* dirName) {
    std::cout << "\nConsider yourself STUBBED!!";
    return true;
}
char* FTPClient::getCurrentDirContents() {
    std::cout << "\nConsider yourself STUBBED!!";
    return NULL;
} //returns buffer with directory contents
bool FTPClient::getFile(char* fileName) {
    std::cout << "\nConsider yourself STUBBED!!";
    return true;
}
bool FTPClient::putFile(char* fileName) {
    std::cout << "\nConsider yourself STUBBED!!";
    return true;
}
