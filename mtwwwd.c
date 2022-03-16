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

#define webpage "<html><body><h1>My First Heading</h1><p>My first paragraph.</p></body></html>"


int main(int args, char *argsv[]) {

    struct sockaddr_in server_addr, client_addr;
    socklen_t sin_len = sizeof(client_addr);
    int fd_server, fd_client;
    FILE *fp;

    char receiveBuffer[1024], senderBuffer[1024];

    fd_server = socket(AF_INET, SOCK_STREAM, 0);

    if(fd_server < 0) {
        perror("socket");
        exit(1);
    }

    memset(&server_addr, '0', sizeof(server_addr));

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(atoi(argsv[2]));

    int rv = bind(fd_server, (struct sockaddr*)&server_addr, sizeof(server_addr));

    if (rv < 0) {
        perror("binding");
        exit(1);
    }

    // for (int i = 0; i < atoi(argsv[2]); i++) {
    //     pthread_create(&threadPool[i], NULL, connectionHandler, (void *) NULL);
    // }

    listen(fd_server, 10);

    char * fileLocation = argsv[1];
    char * requestType;
    char * filePath;
    char data[1024] = {0};
    
    printf("\n Accepting connections at %s\n", argsv[2]);

    while(1) {
        fd_client = accept(fd_server, (struct sockaddr *) &client_addr, &sin_len);
        if (fd_client == -1) {
            perror("Connection failed \n");
            close(fd_client);
        }
        printf("Connected with client \n");
        memset(receiveBuffer, 0, 1024);
        read(fd_client, receiveBuffer, 1023);
        requestType = strtok(receiveBuffer, " ");
        filePath = strtok(NULL, " ");
        // while (requestType != NULL) {
        //     printf("%s", requestType);
        //     requestType = strtok(NULL, " ");
        // }
        fp = fopen(strcat(fileLocation, filePath), "r");
 
        while(fgets(data, 1024, fp) != NULL) {
            if (send(fd_client, data, sizeof(data), 0) == -1) {
                perror("[-]Error in sending file.");
                exit(1);
            }
            bzero(data, 1024);
        }
        fclose (fp);
        close(fd_client);
    }

    return 0;
}