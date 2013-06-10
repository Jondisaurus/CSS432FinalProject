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
    if(argc > 1)
        serverIP = argv[1];
    else {
        outputHelp();
        return 0;
    }

    FTPClient* client = new FTPClient(argv[1]);
    
    std::string userString( getlogin() );
    std::cout << "Name (" << serverIP << ":" << userString << "): ";
    getUserInput();

    client->sendUserName(userInput[0]);
    std::cout << "Password: ";
    getUserInput();
    client->sendPassword(userInput[0]);

    prompt << "ftp> ";

    while(1){
        getUserInput();

        if(!strcmp(userInput[0], "cd"))
            client->changeDir(userInput[1]);
        else if(!strcmp(userInput[0], "open")) {
            //XXX 21 should be taken from command line too
            client->open_connection(userInput[1], 21);
            prompt << serverIP;
        }
        else if(!strcmp(userInput[0], "ls"))
            client->getCurrentDirContents();
        else if(!strcmp(userInput[0], "get"))
            client->downloadFile(userInput[1]);
        else if(!strcmp(userInput[0], "put"))
            client->putFile(userInput[1]);
        else if(!strcmp(userInput[0], "close"))
            client->quit();
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
            std::cout << "\nINVALID COMMAND - Please re-enter or type (?)" << std::endl;
        
    }
    return 0;
}
