#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <iostream>
#include <sstream>
#include <vector>
#include "clientFunctions.h"

#define PORT 14760
#define MAXDATASIZE 512
using namespace std;

int main(int argc, char *argv[]){
    int sockfd, numbytes;
    char buf[MAXDATASIZE];
    char sendbuf[MAXDATASIZE];
    struct hostent *he;
    struct sockaddr_in their_addr;
    string username;
    //Check for command line arguments
    if(argc != 2){
        fprintf(stderr, "usage: client hostname\n");
        exit(1);
    }
    //assign the hostname to the first command line argument
    if((he=gethostbyname(argv[1])) == NULL){
        perror("gethostbyname");
        exit(1);
    }
    //create a TCP socket
    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1){
        perror("socket");
        exit(1);
    }

    their_addr.sin_family = AF_INET;
    their_addr.sin_port = htons(PORT);
    their_addr.sin_addr = *((struct in_addr *)he->h_addr);
    memset(&(their_addr.sin_zero), '\0', 8);
    //check for connection
    if(connect(sockfd, (struct sockaddr *)&their_addr, sizeof(struct sockaddr)) == -1){
        perror("connect");
        exit(1);
    }

    printf("connection has been established with server.\n");
    //main connection loop
    string input;
    for(;;){
        while(1){
            cout << "A.Login\nB.New User\nC.Exit\n";
            getline(cin, input);
            if(input.length() == 1 && toupper(input[0]) > 64 && toupper(input[0]) < 68) break;
            else cout << "Invalid Input\n";
        }
        //If the user would like to login
        if(toupper(input[0]) == 65){
            send(sockfd,input.c_str(),512,0);
            while(1){
                //Gte the username
                cout << "Enter username:\n";
                getline(cin, input);
                username = input;
                //Send the username
                send(sockfd,input.c_str(),512,0);
                //Get the password
                cout << "Enter password:\n";
                getline(cin, input);
                send(sockfd, input.c_str(), 512,0);
                //Recieve the response from the server
                numbytes = recv(sockfd, buf,512,0);
                buf[numbytes] = '\0';

                input = buf;
                //If our login is successful break out of the loop.
                if(input.compare("SUCCESS") == 0){
                    cout << "Login successful!!";
                    break;
                }
                else if(input.compare("USER") == 0) cout << "Invalid Username\n";
                else if(input.compare("PASS") == 0) cout << "Invalid Password\n";
                else if(input.compare("LOGIN") == 0) cout << "User already signed in.\n";

            }
        }
        //If we are creatting a new user
        else if(toupper(input[0]) == 66){
            while(1){
                send(sockfd,input.c_str(),512,0);
                cout << "Enter username:\n";
                getline(cin, input);
                username = input;
                //Send the username
                send(sockfd,input.c_str(),512,0);
                //Get the password
                cout << "Enter password:\n";
                getline(cin, input);
                send(sockfd, input.c_str(), 512,0);
                cout << "Enter email:\n";
                getline(cin, input);
                send(sockfd, input.c_str(), 512,0);
                cout << "Enter phone:\n";
                getline(cin, input);
                send(sockfd, input.c_str(), 512,0);
                cout << "Enter your name:\n";
                getline(cin, input);
                send(sockfd, input.c_str(), 512,0);

                numbytes = recv(sockfd, buf,512,0);
                buf[numbytes] = '\0';
                input = buf;
                if(input.compare("ERR") == 0){
                    cout << "Username exists\n";
                    input = "B";
                }
                else break;
            }
        }
        else if(toupper(input[0]) == 67){
            cout << "Goodbye!!!\n";
            exit(0);
        }
        for(;;){
            //Ask the user what they would like to change
            while(1){
                cout << "\nA.Delete User\nB.Modify User Info\nC.Add new appointment\nD.Remove appointment."
                        << "\nE.Display appointments.\nF.Check for schedule conflicts\nG.Update appointment\nH.View Appointments in Time Range\nZ.Exit\n";
                getline(cin, input);
                if(toupper(input[0]) == 'Z'){
                    cout << "Disconnecting from server\n";
                    exit(0);
                }
                else if (input == "" || toupper(input[0]) < 'A' || toupper(input[0]) > 'H' ||input.length() > 1){ 
                    cout << "Invalid input\n";
                }
                else break;
            }
            numbytes=sizeof(sendbuf);
            sendbuf[numbytes] = '\0';
            //Send the user input to the server
            if ((numbytes = send(sockfd, input.c_str(), MAXDATASIZE, 0)) == -1) {
                perror("send");
                close(sockfd);
                exit(1);
            }
            //If the user decided to delete there account
            if(toupper(input[0]) == 65){
                bool kill = false;
                //ask user if they are certain
                while(1){
                    cout << "Are you sure you would like to delete your account?(y/n)\n";
                    getline(cin, input);
                    if(input.length() != 1 || (toupper(input[0]) != 'Y' && toupper(input[0]) != 'N')){
                        cout << "Invalid input\n";
                    }
                    //if they say yes, flip the kill switch and break
                    else if(toupper(input[0]) == 'Y'){
                        kill = true;
                        break;
                    }
                    //if they say no don't flip the kill switch
                    else break;
                }
                if ((numbytes = send(sockfd, input.c_str(), MAXDATASIZE, 0)) == -1) {
                        perror("send");
                        close(sockfd);
                        exit(1);
                    }
                //if kill switch is flipped send the server the want to kill
                if(kill){
                    if ((numbytes = recv(sockfd, buf, MAXDATASIZE, 0)) == -1) {
                    perror("recv");
                    exit(1);
                    }
                    buf[numbytes] = '\0';
                     input = buf;
                    if(input.compare("SUCCESS") == 0){
                        cout << "Goodbye forever :,(\n";
                        exit(0);
                    }
                }
            }
            else if(toupper(input[0]) == 66){
                while(1){
                    //Ask the user which field they would like to alter
                    cout << "Which field would you like to alter?\n";
                    cout << "A.Username\nB.Password\nC.Email\nD.Phone Number\nE.Name\n";
                    getline(cin, input);
                    //Check to make sure they inputted only one character
                    if(input.length() != 1){
                        cout << "Please input only one character\n";
                    }
                    //Make sure that one character is valid.
                    else if(toupper(input[0]) == 'A' || toupper(input[0]) == 'B' || toupper(input[0]) =='C' ||
                        toupper(input[0]) == 'D' || toupper(input[0]) == 'E') {
                        
                        break;
                    }
                    else{
                        cout << "Invalid Input\n";
                    }
                }
                //Send the users decision.
                if ((numbytes = send(sockfd, input.c_str(), MAXDATASIZE, 0)) == -1) {
                        perror("send");
                        close(sockfd);
                        exit(1);
                    }
                while(1){
                    //Recieve the change the user would like to make
                    cout << "Enter your change:\n";
                    getline(cin,input);
                    //Verify the user would like to change the data
                    cout << "You entered: " << input << ". Is that okay?(y/n)\n";
                    std::string decision;
                    getline(cin, decision);
                    if(decision.length() != 1 || (toupper(decision[0]) != 'Y' && toupper(decision[0]) != 'N')){
                        cout << "Invalid input\n";
                    }
                    //if they say yes, change the field
                    else if(toupper(decision[0]) == 'Y' && input.length() !=0) break;
                    if(input.length() == 0) cout << "Nothing was inputted...\n";
                }
                //send the change to the server
                if((numbytes = send(sockfd, input.c_str(), MAXDATASIZE, 0)) == -1) {
                        perror("send");
                        close(sockfd);
                        exit(1);
                }
                //recieve if we succeeded
                if ((numbytes = recv(sockfd, buf, MAXDATASIZE, 0)) == -1) {
                    perror("recv");
                    exit(1);
                }
                buf[numbytes] = '\0';
                input = buf;
                //If we succeeded let user know
                if(input.compare("SUCCESS") == 0){
                    cout << "Change successful\n";
                }
                else{
                    cout << "Change failed :'(\n";
                }
            }
            else if(toupper(input[0]) == 67){
                //Ask the user for info and send.
                while(1){
                    cout << "Enter the title of the appointment:\n";
                    getline(cin, input);
                    if(input.length() == 0) cout << "Nothing was inputted\n";
                    else break;
                }
                if((numbytes = send(sockfd, input.c_str(), MAXDATASIZE, 0)) == -1) {
                        perror("send");
                        close(sockfd);
                        exit(1);
                }
                //Not currently implemented, but make sure user implements a valid format
                while(1){
                    cout << "Enter the date of the appointment:\n";
                    getline(cin, input);
                    if(input.length() == 0) cout << "Nothing was inputted\n";
                    else if(!checkDateFormat(input)) cout << "Invalid date.\n";
                    else break;
                }
                if((numbytes = send(sockfd, input.c_str(), MAXDATASIZE, 0)) == -1) {
                        perror("send");
                        close(sockfd);
                        exit(1);
                }
                //Not currently implemented, but make sure user implements a valid format
                while(1){
                    cout << "Enter the time of the appointment:\n";
                    getline(cin, input);
                    if(input.length() == 0) cout << "Nothing was inputted\n";
                    else if(!checkTimeFormat(input)) cout << "Invalid time.\n";
                    else break;
                }
                if((numbytes = send(sockfd, input.c_str(), MAXDATASIZE, 0)) == -1) {
                        perror("send");
                        close(sockfd);
                        exit(1);
                }
                //recieve if we succeeded
                if ((numbytes = recv(sockfd, buf, MAXDATASIZE, 0)) == -1) {
                    perror("recv");
                    exit(1);
                }
                buf[numbytes] = '\0';
                input = buf;
                //If we succeeded let user know
                if(input.compare("SUCCESS") == 0){
                    cout << "Appointment added\n";
                }
                else{
                    cout << "Addition failed\n";
                }
            }
            else if(toupper(input[0]) == 68){
                vector<string> appointments;
                while(1){
                    //recieve the appointment titles 1 by 1.
                    if ((numbytes = recv(sockfd, buf, MAXDATASIZE, 0)) == -1) {
                        perror("recv");
                        exit(1);
                    }
                    buf[numbytes] = '\0';
                    input = buf;
                    //the server sends "END" if we are out of appointments for thius user
                    if(input.compare("END") != 0){
                        //Add appointment to the list
                        appointments.push_back(input);
                    }
                    else break;
                }
                while(1){
                    //Ask user which appointment they would like to delete.
                    cout << "Which appointment would you like to delete?\n";
                    for(int i =0; i< appointments.size(); ++i){
                        cout << i+1 << ":  " << appointments[i] << endl;
                    }
                    getline(cin, input);
                    //convert the value to an int for comparison
                    string strInt = input;
                    int intStr;
                    stringstream ss(strInt);
                    ss >> intStr;
                    //check if the input is incorrect.
                    if(intStr<1 || intStr > appointments.size() || input.length() == 0){
                        cout << "Invalid input\n\n";
                    }
                    else break;
                }
                //Tell which the server which appointment we would like to delete.
                //NOTE:On server you need to subtract 1 to access the right appointment
                if((numbytes = send(sockfd, input.c_str(), MAXDATASIZE, 0)) == -1) {
                        perror("send");
                        close(sockfd);
                        exit(1);
                }
                if ((numbytes = recv(sockfd, buf, MAXDATASIZE, 0)) == -1) {
                        perror("recv");
                        exit(1);
                }
                buf[numbytes] = '\0';
                input = buf;
                if(input.compare("SUCCESS") == 0){
                    cout << "Appointment deleted!\n\n";
                }
                else{
                    cout << "Deletion failure :'(\n\n";
                }
            }
            else if(toupper(input[0]) == 69){
                while(1){
                    //recieve the appointment from the server
                    if ((numbytes = recv(sockfd, buf, MAXDATASIZE, 0)) == -1) {
                        perror("recv");
                        exit(1);
                    }
                    buf[numbytes] = '\0';
                    input = buf;
                    //If we aren't at the end of the file
                    //Display the appointment.
                    if(input.compare("END") != 0){
                        cout << input;
                    }
                    else break;
                }
            }
            else if(toupper(input[0]) == 70){
                if ((numbytes = recv(sockfd, buf, MAXDATASIZE, 0)) == -1) {
                        perror("recv");
                        exit(1);
                    }
                buf[numbytes] = '\0';
                input = buf;
                cout << input;
            }
            else if(toupper(input[0]) == 71){
                vector<string> appointments;
                while(1){
                    //recieve the appointment from the server
                    if ((numbytes = recv(sockfd, buf, MAXDATASIZE, 0)) == -1) {
                        perror("recv");
                        exit(1);
                    }
                    buf[numbytes] = '\0';
                    input = buf;
                    //If we aren't at the end of the file
                    //Display the appointment.
                    if(input.compare("END") != 0){
                        appointments.push_back(input);
                    }
                    else break;
                }
                while(1){
                    //Ask user which appointment they would like to change.
                    cout << "Which appointment would you like to change?\n";
                    for(int i =0; i< appointments.size(); ++i){
                        cout << i+1 << ":  " << appointments[i] << endl;
                    }
                    getline(cin, input);
                    //convert the value to an int for comparison
                    string strInt = input;
                    int intStr;
                    stringstream ss(strInt);
                    ss >> intStr;
                    //check if the input is incorrect.
                    if(intStr<1 || intStr > appointments.size() || input.length() == 0){
                        cout << "Invalid input\n\n";
                    }
                    else break;
                }
                //send the server which appointment is changing
                if((numbytes = send(sockfd, input.c_str(), MAXDATASIZE, 0)) == -1) {
                        perror("send");
                        close(sockfd);
                        exit(1);
                }
                while(1){
                    cout << "Which field would you like to alter?\n";
                    cout << "A.Title\nB.Time\nC.Date\n";
                    getline(cin, input);
                    if(input.length() !=1)cout << "Invalid input\n";
                    else break;
                }
                if((numbytes = send(sockfd, input.c_str(), MAXDATASIZE, 0)) == -1) {
                        perror("send");
                        close(sockfd);
                        exit(1);
                }
                while(1){
                    std::string checker = input;
                    cout << "Enter the change you would like to make:\n";
                    getline(cin, input);
                    if(input == "") cout << "Invalid input\n";
                    else if(toupper(checker[0]) == 'B' && !checkTimeFormat(input)) cout << "Invalid time.\n";
                    else if(toupper(checker[0]) == 'C' && !checkDateFormat(input)) cout << "Invalid date.\n";
                    else break;
                }
                if((numbytes = send(sockfd, input.c_str(), MAXDATASIZE, 0)) == -1) {
                        perror("send");
                        close(sockfd);
                        exit(1);
                }
                if ((numbytes = recv(sockfd, buf, MAXDATASIZE, 0)) == -1) {
                        perror("recv");
                        exit(1);
                    }
                buf[numbytes] = '\0';
                input = buf;
                if(input.compare("SUCCESS") == 0){
                    cout << "Appointment Changed!!\n\n";
                }
                else{
                    cout << "Change failure :'(\n\n";
                }
            }
            else if(toupper(input[0]) == 72){
                while(1){
                    cout << "Enter the start date:\n";
                    getline(cin,input);
                    if(input == "") cout << "Invalid input\n";
                    else if(!checkDateFormat(input)) cout << "Invalid date.\n";
                    else break;
                }
                if((numbytes = send(sockfd, input.c_str(), MAXDATASIZE, 0)) == -1) {
                        perror("send");
                        close(sockfd);
                        exit(1);
                }
                while(1){
                    cout << "Enter the end date:\n";
                    getline(cin,input);
                    if(input == "") cout << "Invalid input\n";
                    else if(!checkDateFormat(input)) cout << "Invalid date.\n";
                    else break;
                }
                if((numbytes = send(sockfd, input.c_str(), MAXDATASIZE, 0)) == -1) {
                        perror("send");
                        close(sockfd);
                        exit(1);
                }
                if ((numbytes = recv(sockfd, buf, MAXDATASIZE, 0)) == -1) {
                        perror("recv");
                        exit(1);
                    }
                buf[numbytes] = '\0';
                input = buf;
                if(input.compare("NONE") == 0){
                    cout << "No appointments in time range.\n";
                }
                else{
                    cout << "Appointments in time range:\n" << input << "\n";
                }
            }
            else{
                cout << "How'd you get here?\n";
            }
            
        /*if(numbytes == 0 || strncmp(sendbuf,"bye",3) == 0){
            printf("Bye\n");
            break;
        }else {
            if ((numbytes = send(sockfd, sendbuf, MAXDATASIZE, 0)) == -1) {
                perror("send");
                close(sockfd);
                exit(1);
            }


            sendbuf[numbytes] = '\0';
            cout << "Sent: " << sendbuf << endl;
            //printf("Sent: %s\n", sendbuf);
            if ((numbytes = recv(sockfd, buf, MAXDATASIZE, 0)) == -1) {
                perror("recv");
                exit(1);
            }
            buf[numbytes] = '\0';
            cout << "Recieved: "  << buf << endl;
            //printf('Recieved: %sn', buf);
        }*/
    }

    close(sockfd);
    return 0;
}
}
