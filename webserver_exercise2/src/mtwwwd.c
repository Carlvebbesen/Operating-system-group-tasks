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
#include <signal.h>
#include <sys/wait.h>
#include <sys/syscall.h>
#include <sched.h>
#include "bbuffer.h"

#define ERRORHeaders "HTTP/1.1 404 Not Found\nContent-Type: text/html\n"
#define ErrorBody "<html><body><h1>The requested page could not be found</h1><img src='https://as1.ftcdn.net/v2/jpg/01/41/11/48/1000_F_141114881_1NUIUQ3uDi6EqDijNPoUHPN81dYONt1E.jpg' alt='duck'/></body></html>"
#define OKResponseHeader "HTTP/1.1 200 OK\n"
#define ResponseContentType "Content-Type: text/html\n"

char *fileDirectory;

void *handleRequest(void *arg)
{
    char receiveBuffer[1024], senderBuffer[1024];
    FILE *fp;
    char *fileLocation;
    char *requestType;
    char *filePath;
    char response[1024 * 200], body[1024 * 150];
    char data[1024] = {0};

    memset(receiveBuffer, 0, 1024);

    while (1)
    {
        int fd = bb_get(arg);
        bzero(receiveBuffer, sizeof(receiveBuffer));
        read(fd, receiveBuffer, 1023);
        requestType = strtok(receiveBuffer, " ");
        fileLocation = strdup(fileDirectory);
        filePath = strtok(NULL, " ");
        printf("File path: %s%s\n", fileLocation, filePath);
        strcpy(body, "");
        bzero(data, sizeof(data));
        if (!(fp = fopen(strcat(fileLocation, filePath), "r")))
        {
            snprintf(response, sizeof(response),
                     "%sContent-Length: %zu\n\n%s", ERRORHeaders, strlen(ErrorBody), ErrorBody);
            send(fd, response, sizeof(response), 0);
            close(fd);
            continue;
        }
        while (fgets(data, 1024, fp) != NULL)
        {
            strcat(body, data);
            bzero(data, 1024);
        }
        snprintf(response, sizeof(response),
                 "%sContent-Length: %zu\n\n%s", OKResponseHeader, strlen(body), body);
        if (send(fd, response, sizeof(response), 0) == -1)
        {
            perror("[-]Error in sending file.");
            exit(1);
        }
        fclose(fp);
        close(fd);
    }
    return NULL;
}

int main(int args, char *argsv[])
{
    struct sockaddr_in server_addr, client_addr;
    socklen_t sin_len = sizeof(client_addr);
    int fd_server, fd_client;
    pthread_t threadPool[atoi(argsv[3])];

    fileDirectory = argsv[1];

    fd_server = socket(AF_INET, SOCK_STREAM, 0);

    if (fd_server < 0)
    {
        perror("socket");
        exit(1);
    }

    memset(&server_addr, '0', sizeof(server_addr));

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(atoi(argsv[2]));

    int rv = bind(fd_server, (struct sockaddr *)&server_addr, sizeof(server_addr));

    if (rv < 0)
    {
        perror("binding");
        exit(1);
    }

    BNDBUF *bb = bb_init(atoi(argsv[4]));

    for (int i = 0; i < sizeof(threadPool) / sizeof(threadPool[0]); i++)
    {
        pthread_create(&threadPool[i], NULL, handleRequest, (void *)bb);
    }

    listen(fd_server, 10);

    printf("\n Accepting connections at %s\n", argsv[2]);

    while (1)
    {
        fd_client = accept(fd_server, (struct sockaddr *)&client_addr, &sin_len);
        if (fd_client == -1)
        {
            perror("Connection failed \n");
            close(fd_client);
        }
        else
        {
            bb_add(bb, fd_client);
        }
    }

    return 0;
}
