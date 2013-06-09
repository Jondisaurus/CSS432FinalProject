#include "Socket.h"
#include <string.h>
#include <iostream>
#include <sys/poll.h>
#include <string>
#include "Exception.h"
#include <fstream>
#include <unistd.h>
#include <stdio.h>

#define BUFSIZE 8192
class FTPClient {
public:
    FTPClient();
    FTPClient(char*, char*);
    ~FTPClient();

    int open_connection(char* hostName, int port); //open a TCP connection to port
    // int close();
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
    int sendSYST();
    int getPortFromPASV(char* );
    bool changeDir(char* dirName);
    char* getCurrentDirContents(); //returns buffer with directory contents
    //int getFile(char* filename, char* localpath);
    int downloadFile(char *filename);
    bool putFile(char* fileName);
    bool listDir(char* pathname);
    char *getFileName(char *filepath);
    int getReturnCode(char *message);
double time_diff(struct timeval x, struct timeval y);
    int getMessageSize(char *msg);
private:
    char* userName;
    char* password;
    int clientSD;
    int dataSD;
    int recvBytes; 
    Socket* sock;
    Socket* serverSock;
    char ctrlBuf[BUFSIZE + 1];
    char dataBuf[BUFSIZE + 1];
};

