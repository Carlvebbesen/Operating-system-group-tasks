#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <time.h> 
#include <pthread.h>
#include <semaphore.h>
#include <signal.h>
#include <sys/wait.h>
#include <sched.h>

char webpage[] =
"HTTP/0.9 200 OK\n"
"Content-Type: text/html: charset=UTF-8\r\n\r\n"
"<!DOCTYPE html>\r\n"
"<html>\r\n"
"<body>\r\n"
"<h1>My First Heading</h1>\r\n"
"<p>My first paragraph.</p>\r\n"
"</body>\r\n"
"</html>\r\n";


int main(int args, char *argsv[]) {

    struct sockaddr_in server_addr, client_addr;
    socklen_t sin_len = sizeof(client_addr);
    int fd_server, fd_client;

    char receiveBuffer[1024], senderBuffer[1024];

    fd_server = socket(AF_INET, SOCK_STREAM, 0);

    if(fd_server < 0) {
        perror("socket");
        exit(1);
    }

    memset(&server_addr, '0', sizeof(server_addr));

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(atoi(argsv[1]));

    int rv = bind(fd_server, (struct sockaddr*)&server_addr, sizeof(server_addr));

    if (rv < 0) {
        perror("binding");
        exit(1);
    }

    // for (int i = 0; i < atoi(argsv[2]); i++) {
    //     pthread_create(&threadPool[i], NULL, connectionHandler, (void *) NULL);
    // }

    listen(fd_server, 10);

    while(1) {
        printf("\n Accepting connections at %s\n", argsv[1]);
        fd_client = accept(fd_server, (struct sockaddr *) &client_addr, &sin_len);
        if (fd_client == -1) {
            perror("Connection failed \n");
            close(fd_client);
        }
        printf("Connected with client \n");
        memset(receiveBuffer, 0, 1024);
        read(fd_client, receiveBuffer, 1023);
        send(fd_client, webpage, sizeof(webpage), 0);
        close(fd_client);
    }

    return 0;
}