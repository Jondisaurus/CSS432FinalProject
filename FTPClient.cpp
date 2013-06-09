#include "FTPClient.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <fstream>
#include <vector>
#include <sys/poll.h>
#include <sys/time.h>
#define BUFSIZE 8192

using namespace std;
//-----------------------------------------------------------------------------
FTPClient::FTPClient() {
    clientSD = 0;
    char url[] = "ftp.tripod.com";
    char username[] = "css432";
    char password[] = "UWB0th3ll";
    while(open_connection(url, 21) == 0){
       std::cout << "Cant connect\n";
    }
    int code = login(username, password);
    // if(code == 230)
        // std::cout << "FTP>\tClient logged in" << std::endl;
    // else
    //std::cout << "Code was: " << code << std::endl;
    // exit(1);
}

//-----------------------------------------------------------------------------
FTPClient::FTPClient(char* user, char* pass) {
    clientSD = 0;
    char url[] = "ftp.tripod.com";
    char* username = user;
    char* password = pass;
    while(open_connection(url, 21) == 0){
       std::cout << "Cant connect\n";
    }
    int code = login(username, password);
    // if(code == 230)
        // std::cout << "FTP>\tClient logged in" << std::endl;
    // else
    //std::cout << "Code was: " << code << std::endl;
    // exit(1);
}

//-----------------------------------------------------------------------------
FTPClient::~FTPClient() {
}

//-----------------------------------------------------------------------------
int FTPClient::open_connection(char* hostName, int port) {
    //Setup
    char buffer_in[1450];
    bzero(buffer_in,1450);

    // Attempt to connect to server
    sock = new Socket(port);
    clientSD = sock->getClientSocket(hostName);
    strcpy( buffer_in, recvMessage() );
    std::cout << buffer_in << std::endl;
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
    std::cout << "Consider yourself STUBBED!! (quit)"<< std::endl;
    close(clientSD);
    exit(1);
}

//-----------------------------------------------------------------------------
// This funncction will take in a username and password, and will send them
// to the ftp server, checking to make sure the return code works. If it 
// doesn't, it asks the user to reenter
//----------------------------------------------------------------------------
int FTPClient::login(char *username, char *password) { 
    char buffer[BUFSIZE];
    bzero(buffer, BUFSIZE);
    int code;
    //If anything is wrong, have the user reenter their information
    do {
        code = sendUserName(username);
        if(code != 331) {
        cout << "Incorrect username. Reenter:";
        cin >> username;
        }

        if((code = sendPassword(password)) == 230)
        break;
    
    cout << "Incorrect password. Reenter:";
    cin >> password;
    } while (true);

    strcpy(buffer, recvMessage());
    cout << buffer << endl;
    return code;
}

//-----------------------------------------------------------------------------
//Returns 331 if successful
//Sends the mmessage "USER " and the username
//-----------------------------------------------------------------------------
int FTPClient::sendUserName(char* username) {
    memset( ctrlBuf, '\0', sizeof( ctrlBuf ) );
    int code;
    char buffer[100];
    strcpy(buffer, "USER ");
    strcat(buffer, username);

    if(sendMessage(buffer) < 0) {
    perror("Can't send message\n");
    return 1;
    }
    
    strcpy(buffer, recvMessage());
    cout << buffer << endl;

    return getReturnCode(buffer);
}

//-----------------------------------------------------------------------------
//get the first 3 chars from the received message and conver to an int to 
//get a returnable error code to check
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
//Sends a password to the server, and then system information 
//----------------------------------------------------------------------------
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
    std::cout << buffer << std::endl;

    sendSYST();
    return getReturnCode(buffer);
}



//-----------------------------------------------------------------------------
int FTPClient::sendPASV(){

    // std::cout << "in sendPASV" << std::endl;

    char buffer[70];                            //allocate buffer
    bzero(buffer, 70 );                         //zero out buffer

    sendMessage("PASV ");                       //send the message PASV
    strcpy( buffer, recvMessage() );            //receive reply

    int port = getPortFromPASV( buffer );           //get port # from reply

    // std::cout << "port: " << port << std::endl;

    char url[] = "ftp.tripod.com";              //set the url to connect to

    serverSock = new Socket(port);              //new socket with new port #
    dataSD = serverSock->getClientSocket(url);  //new data socket

    return 0;
}

//-----------------------------------------------------------------------------
int FTPClient::sendSYST(){
    int code;
    char buffer[2048];
    strcpy(buffer, "SYST ");
    // strcat(buffer, password);
    if(sendMessage(buffer) < 0) {
       perror("Can't send message\n");
        return 1;
    }    
    strcpy(buffer, recvMessage());
    std::cout << buffer << std::endl;
    return getReturnCode(buffer);
}

//-----------------------------------------------------------------------------
//Sends a message to the server with \r\n at the end to dictate the end of
//the message
//----------------------------------------------------------------------------
int FTPClient::sendMessage(char* buffer) {
    int message_length = strlen(buffer) + 2;
    char message[message_length];
    bzero(message, sizeof(message));
    strcpy(message, buffer);
    strcat(message,  "\r\n");
    return write(clientSD, message, message_length);
}


//-----------------------------------------------------------------------------
char* FTPClient::recvMessage() {
    //Configure polling
    struct pollfd ufds;
    ufds.fd = clientSD;
    ufds.events = POLLIN;
    ufds.revents = 0;

    char *buffer;
    buffer = new char[BUFSIZE];
    std::string message;
    bzero(buffer, BUFSIZE);
    char *retMsg;
    int msg_size = 0, val;
    bool recieve = false; 
    char error[] = "Error receiving message";
    char buffer_in[BUFSIZE];
    for(int i = 0; i < BUFSIZE; i++) {
        buffer_in[i] = '\0';
    }

    //----------------
    // if( poll(&ufds, 1, 1000) > 0 ) {
    //     while(1){
    //     //clean out buffer for next read
    //         memset(buffer, '\0', BUFSIZE); 
    //         msg_size = read(clientSD, buffer, BUFSIZE);
    //     //got a message? save it
    //         if(msg_size > 0) {
    //             message.append(buffer);
    //         //is this the end of your message?
    //             if(buffer[msg_size-1] == '\n' && buffer[msg_size-2] == '\r')
    //                 break; 
    //         //Is your message empty?
    //             if(buffer[0] == '\0')
    //                 break;
    //         }
    //     }
    // }
    //-------------------
    val = poll(&ufds, 1, 1000);
    // cout << "ABOUT TO READ!! " << endl;
    int counter = 0;

    while(val > 0){
    //clean out buffer for next read
        memset(buffer, '\0', BUFSIZE); 
        msg_size = read(clientSD, buffer, BUFSIZE);
        // cout << "READ CHECK val: " << val  << " msgSize: " << msg_size << endl;
    //got a message? save it
        if(msg_size > 0) {
            message.append(buffer);
        //is this the end of your message?
            if(buffer[msg_size-1] == '\n' && buffer[msg_size-2] == '\r')
                break; 
        //Is your message empty?
            if(buffer[0] == '\0')
                break;
        }
        if(counter > 100 )
            break;

        val = poll(&ufds, 1, 1000);
        counter++;
    }
  
    // cout << "GOT HERE!" << endl;

    if( msg_size > 0) {
        retMsg = new char[message.length() + 1];
        strcpy(retMsg, message.c_str());
        int msg_size = strlen(retMsg);
        for(int i = msg_size - 1; i > msg_size - 3; i--)
        if(retMsg[i] == '\n' || retMsg[i] == '\r')
        retMsg[i] = '\0';
    return retMsg;
    }
    return NULL;
}

//-----------------------------------------------------------------------------
int FTPClient::getPortFromPASV( char* buffer ) {

    // std::cout << "in getPortFromPASV" << std::endl;

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
    // std::cout << "Consider yourself STUBBED!! (changeDir)"<< std::endl;

    // std::cout << dirName << std::endl;

    int code;
    char* msgptr; 
    char buffer[BUFSIZE];

    //add CWD to buffer to be sent
    strcpy(buffer, "CWD ");  
    if(dirName != NULL) 
        strcat(buffer, dirName);

    //send CWD, output error if there was one, message sent on clientSD
    if(sendMessage(buffer) < 0) {
       perror("Can't send message\n");
        return false;
    }  

    //Get message from server
    msgptr = recvMessage();
    std::cout << msgptr << std::endl;

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
    // usleep(100);
    dataptr = recvMessage();
    // usleep(100);
    if(dataptr == NULL) {
        // cout << "is it NULL???" << endl;
        clientSD = tempSD; 
        cout << recvMessage() << endl;
        close(dataSD);
        return dataptr;
    }
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
// get command
int FTPClient::downloadFile(char *filepath) {
    mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
    int code;
    char* dataptr;
    char* msgptr;
    char buffer[BUFSIZE];
    char typeBuffer[BUFSIZE];
    bzero(buffer, BUFSIZE);
    //XXX: This should read from the filepath itself!!!!
    string filename = string(filepath);

    //save clientSD temporarily, we will need it
    int tempSD = clientSD;

    cout << "local: " << filename << " remote: " << filename << endl;


    sendMessage("Type I");
    strcpy(typeBuffer, recvMessage());
    cout << typeBuffer << endl;

    // check if we set the mode to Binary successfully(code 200)
    code = getReturnCode(typeBuffer);
    if(code != 200) {
        cout << "downloadFile() error" << endl;
        return code;
    }   
    
    sendPASV();                     //set up passive connection
    char msg[2048];
    strcpy(msg, "RETR ");
    strcat(msg, filename.c_str());
    sendMessage(msg);               //send RETR with fileName



    strcpy(buffer, recvMessage());
    cout << buffer << endl;  //get 150 message or 550(no such file)
    code = getReturnCode(buffer);
    if(code == 550) {
        // cout << "550 ERROR!!!!!!" << endl;
        // clientSD = dataSD;
        // cout << "??1: " << recvMessage() << endl;
        close(dataSD);
        clientSD = tempSD;          //set clientSD back to itself
        // cout << "??2: " << recvMessage() << endl;
        return code;
    }

    // cout << typeBuffer << endl;

    // bzero(buffer, BUFSIZE);
    // sendMessage("Type I");
    // strcpy(buffer, recvMessage());
    // cout << "SB TYPEI: " << buffer << endl;

    // //check if we set the mode to Binary successfully(code 200)
    // code = getReturnCode(buffer);
    // if(code != 200) {
    //     cout << "downloadFile() error" << endl;
    //     return code;
    // }   

    int size = 0, filesize = 0;
    //setup file stream
    std::ofstream file(filename.c_str(), ios_base::out | ios::binary);

    // cout << "get opened file" << endl;

    //set up time structure, and get start time
    struct timeval start;
    gettimeofday(&start, NULL);

    //set up polling stuff
    struct pollfd ufds;
    ufds.fd = dataSD;
    ufds.events = POLLIN;
    ufds.revents = 0;

    
    int val = poll(&ufds, 1, 1000);
    // cout << "poll val: " << val << endl;
    while(val > 0) {
        // cout << "in get while! " << endl;
        memset(buffer, '\0', BUFSIZE);
        size = read(dataSD, buffer, BUFSIZE);
        if(size > 0) {
            filesize += size;
            file.write(buffer, size);
        }
        else
            break;
        if(buffer[size-1] == '\n' && buffer[size-2]=='\r')
            break;
        if(buffer[0] == '\0')
            break;
        val = poll(&ufds, 1, 1000);
    }

    if(filesize == 0) {
        cout << "get file size was 0!" << endl;
        file.close();
        close(dataSD);
        return 0;
    }
    struct timeval end, diff;
    gettimeofday(&end, NULL); 

    clientSD = tempSD;          //set clienSD back to itself
    file.close();               //close file stream
    close(dataSD);              //close data connection
    // cout << recvMessage() << endl;      //get 150 message
    char *mess = recvMessage();         //get 226 message
    //XXX: FIX THIS OBVIOUSLY WHAT THE FUCK IS WRONG WITH MEEEEEEE
    int total_bytes = getMessageSize(mess);     //get 226 message
    cout << mess << endl;               

    cout << total_bytes << " bytes received in " << time_diff(start, end);
    cout << " milliseconds (" << (total_bytes * 1000000) / time_diff(start, end);
    cout << " Kbytes/s)" << endl;   

    return filesize;
}

int FTPClient::getMessageSize(char *msg) {
    int start = 0, end = 0;
    char array[1000];
    //Get the index of (
    //JUST ASSUME THE TEXT WILL BE IN THE PROPER FORMAT
    for(int i = 0; i < 5000; i++) {
        if(msg[i] == '(' ) {
            start = i;
            start++;
            end = start;
            while(msg[end] != ' ') {
                //cout << msg[end] << endl;
                end++;
            }
        //cout << "SIZE" << start-end << endl;     
        }
        // if(end != 0 ){

        // }
    }

//    cout << start << endl;
//    cout << end << endl;
//    array = (char*)calloc(sizeof(char), end-start);

//    fora(int i = 0; i < start-end; i++)
//  array[i] = msg[start+i];
    memcpy( array, msg+start, end-start );

    // cout << "array: " << array << endl;
    // cout << "atoi: " << atoi(array) << endl;
    return atoi(array);
}
        
//      cout << "HERE IS THE FILE SIZE" << endl;}
//      cout << vector << endl;
//char temp[4];
//if(message == NULL || strlen(message) <= 3)
//      return 0;
//for(int i = 0; i < 3; i++)
//        temp[i] = message[i];
//temp[3] = '\0';
//return atoi(temp);

//    for(; start <= end; start++) {
//      cout << msg[start];
//      }
//      cout << endl;
//      break;
//  }
//    }
//    return 1;//atoi(temp);
//}

double FTPClient::time_diff(struct timeval x, struct timeval y) {
    double x_ms, y_ms, diff;
    x_ms = (double)x.tv_sec*1000000 + (double)x.tv_usec;
    y_ms = (double)y.tv_sec*1000000 + (double)y.tv_usec;
    diff = (double)y_ms - (double)x_ms;
    return diff;
}

//-----------------------------------------------------------------------------
bool FTPClient::putFile(char* fileName) {

    struct pollfd ufds;
    ufds.fd = clientSD;
    ufds.events = POLLIN;
    ufds.revents = 0;

    mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
    int code;
    char* dataptr;
    char* msgptr;
    char buffer[BUFSIZE];
    char fileBuffer[BUFSIZE];
    bzero(buffer, BUFSIZE);
    string filename = string(fileName);

    //save clientSD temporarily, we will need it
    int tempSD = clientSD;

    cout << "local: " << filename << " remote: " << filename << endl;
    // sendMessage("Type I");
    // strcpy(buffer, recvMessage());
    // cout << buffer << endl;
    // code = getReturnCode(buffer);
    // if(code != 200) {
    //     cout << "put() error" << endl;
    //     return code;
    // }   
    
    // sendPASV();                     //setup passive connection
    // char msg[2048];                 //
    // strcpy(msg, "STOR ");           //
    // strcat(msg, filename.c_str());  //
    // sendMessage(msg);               //send message STOR and fileName

    // cout << recvMessage() << endl;  //get 150 message or 550(no such file)

    FILE *file;
    int file_size;
    // cout << "opening " << filename.c_str() << endl;
    file = fopen(filename.c_str(), "rb");

    // bzero(fileBuffer, BUFSIZE);         //clear out the buffer

    if(file) {
        // cout << "file opened in put!! " << endl;
        file_size = fread(fileBuffer, BUFSIZE, 1, file);
    }
    else {
        cout << "local: " << filename.c_str() << ": No such file or directory\n";
        clientSD = tempSD;
        // recvMessage();
        // close(dataSD);
        // recvMessage();
        // fclose(file);
        return 0;
    }

    // cout << "fileSize: " << file_size << endl;

    sendMessage("Type I");
    strcpy(buffer, recvMessage());
    cout << buffer << endl;
    code = getReturnCode(buffer);
    if(code != 200) {
        cout << "put() error" << endl;
        return code;
    }   

    sendPASV();                     //setup passive connection
    char msg[2048];                 //
    strcpy(msg, "STOR ");           //
    strcat(msg, filename.c_str());  //
    sendMessage(msg);               //send message STOR and fileName

    cout << recvMessage() << endl;  //get 150 message or 550(no such file)

    // cout << "sending the following data"<< endl << fileBuffer << endl;
    clientSD = dataSD;
    struct timeval start, end, diff;
    gettimeofday(&start, NULL);
    sendMessage(fileBuffer);
    gettimeofday(&end, NULL); 

    clientSD = tempSD;
    fclose(file);
    close(dataSD);
    // cout << recvMessage() << endl;
 

    char *mess = recvMessage();             //get 226 message with (xx bytes sent)
    int total_bytes = getMessageSize(mess);
    cout << mess << endl;
    cout << total_bytes << " bytes received in " << time_diff(start, end);
    cout << " ms (" << (total_bytes * 1000000) / time_diff(start, end) << "Kbytes/s)" << endl;   

    return file_size;
}
