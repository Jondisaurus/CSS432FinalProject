#include "FTPClient.h"
#include <iostream>
#include <fstream>
#define CHAR_SIZE 1000
#define MAX_FTP_ARGS 10

char** userInput;
int inputSize = 0; 


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

    std::cout << "\nftp> ";
    std::cin.getline(input, CHAR_SIZE, '\n');

    userInput[inputSize] = strtok(input, " ");

    while(userInput[inputSize] != NULL) {
        inputSize++;
        userInput[inputSize] = strtok(NULL, " \n");
    }
    return userInput;
}

void getUserCredentials(char*& userName, char*& password) {
#if 0
    //Get User/Pass
    std::cout << "\nTo login please enter your...";
    std::cout << "\nUsername: ";
    std::cin >> userName;

    std::cout << "\nPassword: ";
    std::cin >> password;
#endif
}

void outputHelp() {
    std::cout << "this is a stub :< (outputHelp)";
}

int main( int argc, char* argv[] ) {

    FTPClient* client = new FTPClient();
    // std::cout << "ftp client created" << std::endl;
    char* userName = new char[CHAR_SIZE];
    char* password = new char[CHAR_SIZE];
    char* dirName = new char[CHAR_SIZE];

    while(1){
        getUserInput();

        if(!strcmp(userInput[0], "cd")){
            client->changeDir(userInput[1]);
        }else if(!strcmp(userInput[0], "ls")){
            std::cout << client->getCurrentDirContents() << std::endl;
        }else if(!strcmp(userInput[0], "get")){
            client->getFile(userInput[0]);
        }else if(!strcmp(userInput[0], "put")){
            client->putFile(userInput[0]);
        }else if(!strcmp(userInput[0], "close")){
            client->close();
        }else if(!strcmp(userInput[0], "exit")){
            client->quit();
        }else if(!strcmp(userInput[0], "quit")){
            client->quit();
        }else{
            std::cout << "\nINVALID COMMAND - Please re-enter..." << std::endl;
        }

    }
}
