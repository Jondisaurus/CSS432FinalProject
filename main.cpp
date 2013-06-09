#include "FTPClient.h"
#include <iostream>
#include <fstream>
#define CHAR_SIZE 1000
#define MAX_FTP_ARGS 10

char** userInput;
int inputSize = 0; 
char* serverIP;

//-----------------------------------------------------------------------------
///This function presents the user with a command line then stores the
// results in char** userInput as indiviual tokens. the number of tokens is
// given by the global variable inputSize.
char** getUserInput() {
    // std::cout << "get user input!!" << std::endl;

    if(userInput){
        delete userInput; 
    }
    userInput = new char*;
    char* temp;
    char input[CHAR_SIZE];
    inputSize = 0;
    memset(input, '\0', sizeof(input));

    std::cout << "ftp> ";
    std::cin.getline(input, CHAR_SIZE, '\n');

    userInput[inputSize] = strtok(input, " ");

    while(userInput[inputSize] != NULL) {
        inputSize++;
        userInput[inputSize] = strtok(NULL, " \n");
    }
    return userInput;
}

//-----------------------------------------------------------------------------
void getUserCredentials(/*char*& userName, char*& password*/) {

    //Get User/Pass
    // std::cout << "\nTo login please enter your...";
    // std::cout << "\nUsername: ";
    // std::cin >> userName;

    // std::cout << "\nPassword: ";
    // std::cin >> password;
    std::string userString( getlogin() );
    std::cout << "Name (" << serverIP << ":" << userString << "): " << std::endl;


}
//-----------------------------------------------------------------------------
void outputHelp() {
    std::cout << "Wrong usage! Please use like this: " << std::endl;
    std::cout << "./ftp hostName" << std::endl;
}

//-----------------------------------------------------------------------------
// run with ./ftp ftp.tripod.com
int main( int argc, char* argv[] ) {

    if(argc > 1)
        serverIP = argv[1];
    else {
        outputHelp();
        return 0;
    }

    char* userName = new char[CHAR_SIZE];
    char* password = new char[CHAR_SIZE];
    char* dirName = new char[CHAR_SIZE];

    getUserCredentials();

    FTPClient* client = new FTPClient();
    // std::cout << "ftp client created" << std::endl;
    

    while(1){
        getUserInput();

        if(!strcmp(userInput[0], "cd")){
            client->changeDir(userInput[1]);
        }else if(!strcmp(userInput[0], "ls")){
            // std::cout << client->getCurrentDirContents() << std::endl;
            client->getCurrentDirContents();
        }else if(!strcmp(userInput[0], "get")){
            // std::cout << "userIn: " << userInput[1] << std::endl;
            client->downloadFile(userInput[1]);
        }else if(!strcmp(userInput[0], "put")){
            client->putFile(userInput[1]);
        }else if(!strcmp(userInput[0], "close")){
            // client->close();
            client->quit();
        }else if(!strcmp(userInput[0], "exit")){
            client->quit();
        }else if(!strcmp(userInput[0], "quit")){
            client->quit();
        }else if(!strcmp(userInput[0], "help") || !strcmp(userInput[0], "?")){
            outputHelp(); 
        }else{
            std::cout << "\nINVALID COMMAND - Please re-enter or type (?)" << std::endl;
        }

    }
    return 0;
}
