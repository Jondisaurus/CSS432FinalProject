#include "FTPClient.h"
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <unistd.h>
#include <sstream>
#include <string>
#define CHAR_SIZE 1000
#define MAX_FTP_ARGS 10

char** userInput;
int inputSize = 0; 
char* serverIP;
FTPClient* client;
// char username[1000];
// char password[1000];
std::stringstream prompt;
//-----------------------------------------------------------------------------
///This function presents the user with a command line then stores the
// results in char** userInput as indiviual tokens. the number of tokens is
// given by the global variable inputSize.
char** getUserInput() {
    if(userInput){
        delete userInput; 
    }
    userInput = new char*;
    char* temp;
    char input[CHAR_SIZE];

    inputSize = 0;
    memset(input, '\0', sizeof(input));
    std::cout << prompt.str(); 
    std::cin.clear(); 
    std::cin.getline(input, CHAR_SIZE, '\n');

    userInput[inputSize] = strtok(input, " ");

    while(userInput[inputSize] != NULL) {
        inputSize++;
        userInput[inputSize] = strtok(NULL, " \n");
    }
    return userInput;
}


//-----------------------------------------------------------------------------
void outputHelp() {
    std::cout << "Wrong usage! Please use like this: " << std::endl;
    std::cout << "./ftp hostName username password" << std::endl;
}

bool execCommand(char** userInput, bool& connected){
    if(connected){
    //--------- CLIENT CONNECTED

        if(!strcmp(userInput[0], "cd"))
            client->changeDir(userInput[1]);
        else if(!strcmp(userInput[0], "open")) {
            //XXX 21 should be taken from command line too
            int port = 21; 
            if(userInput[2] != NULL){
                port = atoi(userInput[2]);
            }

            while(client->open_connection(userInput[1], port) <= 0){
                std::cout << "Cant connect. Reenter url";
                //std::cin >> userInput[1];
            }
            prompt.str("");
            std::string userString( getlogin() );
            std::cout << "Name (" << userInput[1] << ":" << userString << "): ";
            getUserInput();

            client->sendUserName(userInput[0]);
            std::cout << "Password: ";
            getUserInput();
            client->sendPassword(userInput[0]);
            connected = true; 
            prompt.str("ftp> ");
        }
        else if(!strcmp(userInput[0], "ls"))
            client->getCurrentDirContents();
        else if(!strcmp(userInput[0], "get"))
            client->downloadFile(userInput[1]);
        else if(!strcmp(userInput[0], "put"))
            client->putFile(userInput[1]);
        else if(!strcmp(userInput[0], "close")){
            client->close_connection();
            std::cout << "Client disconnected\n";
            connected = false;
        }

        else if(!strcmp(userInput[0], "mkdir"))
            client->makeDir(userInput[1]);
        else if(!strcmp(userInput[0], "rename"))
            client->renameFile(userInput[1], userInput[2]);
        else if(!strcmp(userInput[0], "exit"))
            client->quit();
        else if(!strcmp(userInput[0], "quit"))
            client->quit();
        else if(!strcmp(userInput[0], "name"))
            client->sendUserName(userInput[1]);
        else if(!strcmp(userInput[0], "password"))
            client->sendPassword(userInput[1]);
        else if(!strcmp(userInput[0], "help") || !strcmp(userInput[0], "?"))
            outputHelp(); 
        else
            std::cout << "INVALID COMMAND - Please re-enter or type (?)" << std::endl;

    }else{
    //-------- CLIENT DISCONNECTED

        if(!strcmp(userInput[0], "open")) {
            //XXX 21 should be taken from command line too
            int port = 21; 
            if(userInput[2] != NULL){
                port = atoi(userInput[2]);
            }

            while(client->open_connection(userInput[1], port) <= 0){
                std::cout << "Cant connect. Reenter url";
                std::cin >> userInput[1];
            }
            prompt.str("");
            std::string userString( getlogin() );
            std::cout << "Name (" << userInput[1] << ":" << userString << "): ";

            getUserInput();

            client->sendUserName(userInput[0]);
            std::cout << "Password: ";
            getUserInput();
            client->sendPassword(userInput[0]);
            connected = true; 
            prompt.str("ftp> ");
        }
        else if(!strcmp(userInput[0], "help") || !strcmp(userInput[0], "?"))
            outputHelp();
        else if(!strcmp(userInput[0], "exit") || !strcmp(userInput[0], "quit") || !strcmp(userInput[0], "close")){
            client->quit();
            return false;
        }else{
            std::cout << "Client disconnected - type Open to connect or (?) for more options\n";
        }

    }
    return true; 
}

//-----------------------------------------------------------------------------
// run with ./ftp ftp.tripod.com
int main( int argc, char* argv[] ) {

    //=================
    // FTPClient* client = (argc > 3) ? new FTPClient(argv[1], argv[2], argv[3]) :
                     // new FTPClient();
    //==================
    

    //serverIP = client->getServerIP();
    //prompt << "Name" << getlogin() << "):";
    //prompt << "Name) ";
    bool connected = false; 

    if(argc > 1){
        serverIP = argv[1];

        client = new FTPClient(argv[1]);
    
        std::string userString( getlogin() );
        std::cout << "Name (" << serverIP << ":" << userString << "): ";
        getUserInput();

        client->sendUserName(userInput[0]);
        std::cout << "Password: ";
        getUserInput();
        client->sendPassword(userInput[0]);
        connected = true; 
    }
    else{
        client = new FTPClient();
    }

    

    prompt.str("ftp> ");

    do{
        getUserInput();
    }while(execCommand(userInput, connected));

    return 0;
}
