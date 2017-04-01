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

#define PORT 3490
#define MAXDATASIZE 100
using namespace std;

int main(int argc, char *argv[]){
    int sockfd, numbytes;
    string buf(MAXDATASIZE," ");
    string send(MAXDATASIZE, " ");
    struct hostent *he;
    struct sockaddr_in their_addr;
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

    printf("connection has been established with server. Type any message for server\n");
    //main connection loop
    for(;;){
        getline(cin,sendbuf);
        numbytes=sizeof(sendbuf);
        sendbuf[numbytes] = '\0';

        if(numbytes == 0 || strncmp(sendbuf, "bye", 3) == 0){
            printf("Bye\n");
            break;
        }else {
            if ((numbytes = send(sockfd, sendbuf, sizeof(sendbuf), 0)) == -1) {
                perror("send");
                close(sockfd);
                exit(1);
            }


            sendbuf[numbytes] = '\0';
            printf("Sent: %s\n", sendbuf);

            if ((numbytes = recv(sockfd, buf, 127, 0)) == -1) {
                perror("recv");
                exit(1);
            }
            buf[numbytes] = '\0';
            printf('Recieved: %sn', buf);
        }
    }

    close(sockfd);
    return 0;
}