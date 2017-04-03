#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <iostream>
#include <vector>
#include "User.h"
#include "functions.h"
#include "Appointment.h"
#include <sstream>

#define MYPORT 3490
#define BACKLOG 10

void sigchld_handler(int s){

    while(wait(NULL) > 0);

}

int main(void) {
    int sockfd, new_fd;
    struct sockaddr_in my_addr;
    struct sockaddr_in their_addr;
    int sin_size;
    struct sigaction sa;
    int yes = 1;
    std::string sendbuf(512, ' ');
    char *recvbuf;
    char *caddr;
    int numbytes;
    std::vector<User> userList;
    //Initialize databases.
    readUserFile(userList);
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket");
        exit(1);
    }

    printf("SOCK_FD=%d\n", sockfd);
    if(setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1){
        perror("setsockopt");
        exit(1);
    }

    my_addr.sin_family = AF_INET;
    my_addr.sin_port = htons(MYPORT);
    my_addr.sin_addr.s_addr = INADDR_ANY;
    memset(&(my_addr.sin_zero), '\0', 8);

    if(bind(sockfd, (struct sockaddr *) &my_addr, sizeof(struct sockaddr)) == -1){
        perror("bind");
        exit(1);
    }

    if(listen(sockfd, BACKLOG) == -1){
        perror("listen");
        exit(1);
    }

    sa.sa_handler = sigchld_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    if(sigaction(SIGCHLD, &sa, NULL) == -1){
        perror("sigaction");
        exit(1);
    }

    while(1){
        sin_size = sizeof(struct sockaddr_in);
        //added (socklen_t*) due to type mismatch
        if((new_fd = accept(sockfd, (struct sockaddr *)&their_addr, (socklen_t*) &sin_size)) == -1){
            perror("accept");
            exit(1);
        }
        printf("server: got connection from %s\n",  inet_ntoa(their_addr.sin_addr));
        if(!fork()){
            //reinitialize db in case something has changed
            readUserFile(userList);
            recvbuf = (char*) calloc(512,sizeof(char));
            //Check for login
            std::string username, password,input;
            //std::cout <<"hell";
            bool moveOn = false;
            while(!moveOn){
                numbytes=recv(new_fd, recvbuf, 512, 0);
                if(numbytes < 0){
                    perror("recv");
                    close(new_fd);
                    exit(1);
                }
                recvbuf[numbytes] = '\0';
                input = recvbuf;
                //std::cout << toupper(input[0]);
                //If the user passed in the option to login.
                printf("%s\n",recvbuf);
                if(toupper(input[0]) == 65){
                    //While we don't have an error
                    while(1){
                        //Recieve username
                        numbytes=recv(new_fd, recvbuf, 512, 0);
                        if(numbytes < 0){
                            perror("recv");
                            close(new_fd);
                            exit(1);
                        }
                        recvbuf[numbytes] = '\0';
                        username = recvbuf;
                        printf("%s,\n", recvbuf);
                        //recieve password
                        numbytes=recv(new_fd, recvbuf, 512, 0);
                        if(numbytes < 0){
                            perror("recv");
                            close(new_fd);
                            exit(1);
                        }
                        recvbuf[numbytes] = '\0';
                        printf("%s,\n", recvbuf);
                        password = recvbuf;
                        //Find the index in the vector where the username exists
                        int index = verifyUser(username, userList);
                        printf("%d\n", index);
                        //If we found a proper user
                        if(index != -1){
                            //If the passwor matches pass a success and break
                            if(passwordVerify(password, userList, index)){
                                // check if user is already logged in.
                                if(checkLogin(username)){
                                    input = "SUCCESS\0";
                                    if(send(new_fd, input.c_str(), 512, 0) == -1){
                                        perror("send");
                                        close(new_fd);
                                        exit(1);
                                    }
                                    //Break out of the loops
                                    moveOn = true;
                                    break;
                                }
                                //if user was already logged in let client know
                                else{
                                    input = "LOGIN\0";
                                    if(send(new_fd, input.c_str(), 512, 0) == -1){
                                        perror("send");
                                        close(new_fd);
                                        exit(1);
                                    }
                                }
                            }
                            //If the pass word is wrong tell the client we had an error
                            else{
                                input = "PASS\0";
                                if(send(new_fd, input.c_str() , 512, 0) == -1){
                                    perror("send");
                                    close(new_fd);
                                    exit(1);
                                }
                            }
                        }
                        //if the username doesn't exist tell client that the user name doesnt exist
                        else{
                            input = "USER\0";
                            if(send(new_fd, input.c_str() , 512, 0) == -1){
                                perror("send");
                                close(new_fd);
                                exit(1);
                        }  
                    }
                    }
                }
                //If we are adding a new user
                else if(toupper(input[0]) == 'B'){
                    std::string user, pass, email, phone, name;
                    printf("here\n");
                    moveOn = true;
                    //Recieve USername
                    numbytes=recv(new_fd, recvbuf, 512, 0);
                    if(numbytes < 0){
                        perror("recv");
                        close(new_fd);
                        exit(1);
                    }
                    recvbuf[numbytes] = '\0';
                    user = recvbuf;
                    numbytes=recv(new_fd, recvbuf, 512, 0);
                    //Password
                    if(numbytes < 0){
                        perror("recv");
                        close(new_fd);
                        exit(1);
                    }
                    pass = recvbuf;
                    //email
                    numbytes=recv(new_fd, recvbuf, 512, 0);
                    if(numbytes < 0){
                        perror("recv");
                        close(new_fd);
                        exit(1);
                    }
                    recvbuf[numbytes] = '\0';
                    email = recvbuf;
                    //phone
                    numbytes=recv(new_fd, recvbuf, 512, 0);
                    if(numbytes < 0){
                        perror("recv");
                        close(new_fd);
                        exit(1);
                    }
                    recvbuf[numbytes] = '\0';
                    phone = recvbuf;
                    //Name
                    numbytes=recv(new_fd, recvbuf, 512, 0);
                    if(numbytes < 0){
                        perror("recv");
                        close(new_fd);
                        exit(1);
                    }
                    if(verifyUser(user,userList) != -1){
                        recvbuf[numbytes] = '\0';
                        name = recvbuf;
                        User newUser (user,email,pass,phone,name);
                        userList.push_back(newUser);
                        updateUserFile(userList);
                        username = user;
                        checkLogin(username);
                        updateAppointmentFile(username, userList);
                        printf("User created!\n");
                        input = "SUCCESS\0";
                        if(send(new_fd, input.c_str() , 512, 0) == -1){
                            perror("send");
                            close(new_fd);
                            _exit(1);
                        }
                    }
                    else{
                        input = "ERR\0";
                        if(send(new_fd, input.c_str() , 512, 0) == -1){
                            perror("send");
                            close(new_fd);
                            _exit(1);
                        }
                    }
                }
                else if((toupper(input[0]) == 'C') || numbytes==0){
                    printf("Client (%s) has been disconnected\n", inet_ntoa(their_addr.sin_addr));
                    close(new_fd);
                    _exit(0);
                }
            }
            while(1){
                numbytes=recv(new_fd, recvbuf, 512, 0);
                if(numbytes < 0){
                    perror("recv");
                    close(new_fd);
                    _exit(1);
                }
                recvbuf[numbytes] = '\0';
                //Deleting user
                if(toupper(recvbuf[0]) == 'A'){
                    numbytes=recv(new_fd, recvbuf, 512, 0);
                    if(numbytes < 0){
                        perror("recv");
                        close(new_fd);
                        exit(1);
                    }
                    recvbuf[numbytes] = '\0';
                    if(toupper(recvbuf[0])== 'Y'){
                    //If we properly deleted the user
                        if(deleteUser(username,userList)){
                            input = "SUCCESS\0";
                            if(send(new_fd, input.c_str() , 512, 0) == -1){
                                perror("send");
                                close(new_fd);
                                _exit(1);
                            }
                            removeUserLogin(username);
                            updateUserFile(userList);
                            _exit(0);  
                        }
                        else{
                            input = "FAIL\0";
                            if(send(new_fd, input.c_str() , 512, 0) == -1){
                                perror("send");
                                close(new_fd);
                                _exit(1);
                            }
                        }
                    }
                }
                //modify user info
                else if(toupper(recvbuf[0]) == 'B'){
                    std::string field;
                    numbytes=recv(new_fd, recvbuf, 512, 0);
                    if(numbytes < 0){
                        perror("recv");
                        close(new_fd);
                        _exit(1);
                    }
                    recvbuf[numbytes] = '\0';
                    field = recvbuf;
                    numbytes=recv(new_fd, recvbuf, 512, 0);
                    if(numbytes < 0){
                        perror("recv");
                        close(new_fd);
                        _exit(1);
                    }
                    recvbuf[numbytes] = '\0';
                    input = recvbuf;
                    std::string check = input;
                    if(updateUser(username,field,input,userList)){
                        input = "SUCCESS\0";
                        if(send(new_fd, input.c_str() , 512, 0) == -1){
                            perror("send");
                            close(new_fd);
                            _exit(1);
                        }
                        updateUserFile(userList);
                    }
                    else{
                        input = "FAIL\0";
                        if(send(new_fd, input.c_str() , 512, 0) == -1){
                            perror("send");
                            close(new_fd);
                            _exit(1);
                        }
                    }
                    printf("%d\n", verifyUser(check, userList));
                }
                //Add appointment
                else if(toupper(recvbuf[0]) == 'C'){
                    std::string title, date, tyme;
                    //get appointment title
                    numbytes=recv(new_fd, recvbuf, 512, 0);
                    if(numbytes < 0){
                        perror("recv");
                        close(new_fd);
                        _exit(1);
                    }
                    recvbuf[numbytes] = '\0';
                    title = recvbuf;
                    //get appointment date
                    numbytes=recv(new_fd, recvbuf, 512, 0);
                    if(numbytes < 0){
                        perror("recv");
                        close(new_fd);
                        _exit(1);
                    }
                    recvbuf[numbytes] = '\0';
                    date = recvbuf;
                    numbytes=recv(new_fd, recvbuf, 512, 0);
                    if(numbytes < 0){
                        perror("recv");
                        close(new_fd);
                        _exit(1);
                    }
                    recvbuf[numbytes] = '\0';
                    //get appointment time
                    tyme = recvbuf;
                    //add appointment a let client know if we were successful
                    if(newAppointment(username, userList, title, date, tyme)){
                        input = "SUCCESS\0";
                        if(send(new_fd, input.c_str() , 512, 0) == -1){
                            perror("send");
                            close(new_fd);
                            _exit(1);
                        }
                        updateAppointmentFile(username, userList);
                    }
                    else{
                        input = "FAIL\0";
                        if(send(new_fd, input.c_str() , 512, 0) == -1){
                            perror("send");
                            close(new_fd);
                            _exit(1);
                        }
                    }
                }
                //Delete appointment
                else if(toupper(recvbuf[0]) == 'D'){
                    //get the users appointments
                    std::vector<Appointment> appointments = getAppointmentList(username, userList);
                    //Go through the appointment list and send the titles
                    for(int i = 0; i < appointments.size(); ++i){
                        input = appointments[i].getTitle();
                        if(send(new_fd, input.c_str() , 512, 0) == -1){
                            perror("send");
                            close(new_fd);
                            _exit(1);
                        }
                    }
                    //Send END when we are out of appointments
                    input = "END\0";
                    if(send(new_fd, input.c_str() , 512, 0) == -1){
                        perror("send");
                        close(new_fd);
                        exit(1);
                    }//Recieve which appointment we want to delete
                    numbytes=recv(new_fd, recvbuf, 512, 0);
                    if(numbytes < 0){
                        perror("recv");
                        close(new_fd);
                        _exit(1);
                    }
                    recvbuf[numbytes] = '\0';
                    //convert value into int
                    input =recvbuf;
                    int index;
                    std::stringstream ss(input);
                    ss >> index;
                    //Decrement as client sends the index+1
                    index--;
                    //Remove the appointment and let user know if we were successful or not.
                    if(remAppointment(username, userList, index)){
                        input = "SUCCESS\0";
                        if(send(new_fd, input.c_str() , 512, 0) == -1){
                            perror("send");
                            close(new_fd);
                            _exit(1);
                        }
                        updateAppointmentFile(username, userList);
                    }
                    else{
                        input = "FAIL\0";
                        if(send(new_fd, input.c_str() , 512, 0) == -1){
                            perror("send");
                            close(new_fd);
                            _exit(1);
                        }
                    }
                }
                //Display all appointments
                else if(toupper(recvbuf[0]) == 'E'){
                    //get the appointment list
                   std::vector<Appointment> appointments = getAppointmentList(username, userList);
                   //send the appointment info to the client
                   for(int i = 0; i< appointments.size(); ++i){
                        input = getAppointString(appointments[i]);
                        if(send(new_fd, input.c_str() , 512, 0) == -1){
                            perror("send");
                            close(new_fd);
                            _exit(1);
                        }
                   }
                   //let client side know we are at the end of the file
                   input = "END\0";
                   if(send(new_fd, input.c_str() , 512, 0) == -1){
                        perror("send");
                        close(new_fd);
                        _exit(1);
                    } 
                }
                //Check for schedule conflicts
                else if(toupper(recvbuf[0]) == 'F'){
                    //conflict check returns all of the conflicts
                    input = conflictCheck(username, userList);
                    input = "";
                    //if there are no conflicts set input to NONE
                    if(input.compare("") == 0) input = "NONE";
                    //send data
                    if(send(new_fd, input.c_str() , 512, 0) == -1){
                        perror("send");
                        close(new_fd);
                        _exit(1);
                    } 
                }
                else if(toupper(recvbuf[0]) == 'G'){
                    //get the users appointments
                    std::vector<Appointment> appointments = getAppointmentList(username, userList);
                    //Go through the appointment list and send the titles
                    for(int i = 0; i < appointments.size(); ++i){
                        input = appointments[i].getTitle();
                        if(send(new_fd, input.c_str() , 512, 0) == -1){
                            perror("send");
                            close(new_fd);
                            _exit(1);
                        }
                    }
                    //Send END when we are out of appointments
                    input = "END\0";
                    if(send(new_fd, input.c_str() , 512, 0) == -1){
                        perror("send");
                        close(new_fd);
                        exit(1);
                    }//Recieve which appointment we want to delete
                    numbytes=recv(new_fd, recvbuf, 512, 0);
                    if(numbytes < 0){
                        perror("recv");
                        close(new_fd);
                        _exit(1);
                    }
                    recvbuf[numbytes] = '\0';
                    //convert value into int
                    input =recvbuf;
                    int index;
                    std::stringstream ss(input);
                    ss >> index;
                    //Decrement as client sends the index+1
                    index--;
                    //Recieve which field the user would like to change which will
                    //be represented as a for title b for time and c for date
                    numbytes=recv(new_fd, recvbuf, 512, 0);
                    if(numbytes < 0){
                        perror("recv");
                        close(new_fd);
                        _exit(1);
                    }
                    recvbuf[numbytes] = '\0';
                    input = recvbuf;
                    //recieve the change the user would like to make
                    numbytes=recv(new_fd, recvbuf, 512, 0);
                    if(numbytes < 0){
                        perror("recv");
                        close(new_fd);
                        _exit(1);
                    }
                    recvbuf[numbytes] = '\0';
                    std::string change = recvbuf;
                    //Attempt to update the appointment
                    //If we succeed let client know and update the db
                    if(appointmentUpdate(username, userList, index, input, change)){
                        input = "SUCCESS\0";
                        if(send(new_fd, input.c_str() , 512, 0) == -1){
                            perror("send");
                            close(new_fd);
                            _exit(1);
                        }
                        updateAppointmentFile(username, userList);
                    }
                    //else let client know that we suck
                    else{
                        input = "FAIL\0";
                        if(send(new_fd, input.c_str() , 512, 0) == -1){
                            perror("send");
                            close(new_fd);
                            _exit(1);
                        }
                    }
                }
                else if(toupper(recvbuf[0]) == 'H'){
                    //recieve the start date that we are gonna examine
                    numbytes=recv(new_fd, recvbuf, 512, 0);
                    if(numbytes < 0){
                        perror("recv");
                        close(new_fd);
                        _exit(1);
                    }
                    recvbuf[numbytes] = '\0';
                    std::string start = recvbuf;
                    //recieve  the end date
                    numbytes=recv(new_fd, recvbuf, 512, 0);
                    if(numbytes < 0){
                        perror("recv");
                        close(new_fd);
                        _exit(1);
                    }
                    recvbuf[numbytes] = '\0';
                    std::string end = recvbuf;
                    input = timeRange(username, userList, start, end);
                    if(input.compare("") == 0) input = "NONE";
                    if(send(new_fd, input.c_str() , 512, 0) == -1){
                            perror("send");
                            close(new_fd);
                            _exit(1);
                        }
                }
                else if(numbytes==0 || strncmp(recvbuf, "bye",3)== 0){
                    printf("Client (%s) has been disconnected\n", inet_ntoa(their_addr.sin_addr));
                    removeUserLogin(username);
                    close(new_fd);
                    _exit(0);
                }
            }
            close(sockfd);
            close(new_fd);
            _exit(0);
        }
        close(new_fd);
    }
    return 0;
}
