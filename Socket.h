#ifndef JON_SOCKET
#define JON_SOCKET

#include <sys/types.h>    // socket, bind
#include <sys/socket.h>   // socket, bind, listen, inet_ntoa
#include <netinet/in.h>   // htonl, htons, inet_ntoa
#include <arpa/inet.h>    // inet_ntoa
#include <netdb.h>        // gethostbyname
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <stdlib.h>
class Socket {

    public:
        Socket(int);
        int getServerSocket( );
        int getClientSocket(char*);

    private:
        int port;
        int SD;
};
#endif
