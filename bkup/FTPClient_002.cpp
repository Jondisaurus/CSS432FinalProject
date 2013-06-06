#include "FTPClient.h"
#include <fstream>
#include <string.h>
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
        std::cout << "Logged in\n" << std::endl;
    else
	std::cout << "Code was: " << code << std::endl;
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
    std::cout << "sd is" << clientSD << std::endl;

    // Read Connection acknoledgement from server.
    recvCommand(); 
    std::cout << "From server" << std::endl << ctrlBuf << std::endl;
    while(getReturnCode(ctrlBuf) != 220) {
		std::cout << "Incorrect hostname and port: Enter new one" <<
						std::endl;

						//some implementation here would be nice
    }
 
    return true; //true if > 0
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

bool FTPClient::login(char *username, char* password){
	int code = sendUserName(username);

	std::cout << "User name sent, return code: " << code << std::endl; 

	return true; 

}

int FTPClient::sendUserName(char* nameToSend){
    bzero(ctrlBuf, sizeof(ctrlBuf));
	//memset( ctrlBuf, '\0', BUFSIZE+1);
	strcpy(ctrlBuf, "USER ");
	strcat(ctrlBuf, nameToSend);

	int bytesSent = sendCommand();

	std::cout << "Bytes sent: " << bytesSent << std::endl; 

	if(bytesSent < 0){
		return -1; 
	}
	return recvCommand(); 
}

int FTPClient::recvCommand(){
	do{
        std::cout << " ClientSD: " << clientSD << std::endl; 
		recvBytes = read(clientSD,ctrlBuf,BUFSIZE);
	}while(recvBytes == 0);
	ctrlBuf[recvBytes] = 0;

	std::cout << "RECV -- ctrlBuf: " << ctrlBuf << std::endl; 

	return getReturnCode(ctrlBuf); 

}

int FTPClient::sendCommand(){
	int messageSize = strlen(ctrlBuf) + 3;
	char message[messageSize];
	memset( message, '\0', messageSize);
	strcpy(message, ctrlBuf);
	strcat(message, "\r\n");
	message[messageSize] = 0; 
	std::cout << "SEND -- ctrlBuf: " << ctrlBuf << std::endl; 

	return write(clientSD,message,messageSize);
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
};
