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
    FTPClient(char*, char*, char*);
    FTPClient(char* );
    ~FTPClient();
    // char** open_connection(char* hostName, int port); //open a TCP connection to port
    int open_connection(char* hostName, int port);
    void close_connection();
    void quit();
    int login(char *username, char* password);
    int getUserName();
    int getPassword();
    int sendUserName(char* nameToSend);
    int sendMessage(char *buffer);
    void* waitForMessage(void *ptr);
    char* recvMessage();
    int sendPassword(char* passToSend);
    int sendPASV();         // Send PASV to server and obtain a new Server Socket
    int sendSYST();         // send client file system type
    int getPortFromPASV(char* );
    bool changeDir(char* dirName);
    char* getCurrentDirContents(); //returns buffer with directory contents
    int downloadFile(char *filename);
    bool putFile(char* fileName);
    bool listDir(char* pathname);
    char *getFileName(char *filepath);
    int getReturnCode(char *message);
    double time_diff(struct timeval x, struct timeval y);
    int getMessageSize(char *msg);
    bool renameFile(char* oldFilename, char* newFilename);
    bool makeDir(char* dirName);
    bool removeDir(char* dirName);
<<<<<<< HEAD
    bool printWorkingDirectory();
=======
    bool deleteFile(char* fileName);
>>>>>>> e181c10916a423e45102328e643a16b9534cddae

private:
    int clientSD;
    int dataSD;
    int recvBytes; 
    Socket* sock;
    Socket* serverSock;
    char ctrlBuf[BUFSIZE + 1];
    char dataBuf[BUFSIZE + 1];
};

