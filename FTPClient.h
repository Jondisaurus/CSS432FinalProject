#include "Socket.h"
#include <string.h>
#include <iostream>
#include <sys/poll.h>
#include <string>
#include "Exception.h"
#include <fstream>

#define BUFSIZE 8192
class FTPClient {
public:
    FTPClient();
    ~FTPClient();

    int open(char* hostName, int port); //open a TCP connection to port
    int close();
    void quit();
    int login(char *username, char* password);
    int sendUserName(char* nameToSend);
    //int sendMessage(); 
    int sendMessage(char *buffer);
    void* waitForMessage(void *ptr);
    char* recvMessage();
    //int sendCommand();
    //int recvCommand();
    int sendPassword(char* passToSend);
    int sendPASV();
    int getPortFromPASV(char* );
    bool changeDir(char* dirName);
    char* getCurrentDirContents(); //returns buffer with directory contents
    bool getFile(char* fileName);
    bool putFile(char* fileName);
    bool listDir(char*  pathname);
    int getReturnCode(char *message);
private:
    char* userName;
    char* password;
    int clientSD;
    int dataSD;
    int recvBytes; 
    Socket* sock;
    char ctrlBuf[BUFSIZE + 1];
    char dataBuf[BUFSIZE + 1];
};

