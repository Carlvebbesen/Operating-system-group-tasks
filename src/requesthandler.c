#include <bbuffer.h>
#include <stdio.h>

#define ERRORHeaders "HTTP/1.1 404 Not Found\nContent-Type: text/html\n"
#define ErrorBody "<html><body><h1>The requested page could not be found</h1></body></html>"
#define OKResponseHeader "HTTP/1.1 200 OK\n"
#define ResponseContentType "Content-Type: text/html\n"

void handleRequest(BNDBUF *bb, char *fileDirectory) {

    char receiveBuffer[1024], senderBuffer[1024];
    FILE *fp;
    char * fileLocation = fileDirectory;
    char * requestType;
    char * filePath;
    char response[1024*4096], body[1024*2048];
    char data[1024] = {0};

    memset(receiveBuffer, 0, 1024);

    while(1) {
        int fd = bb_get(bb);

        bzero(receiveBuffer, sizeof(receiveBuffer));
        read(fd, receiveBuffer, 1023);
        requestType = strtok(receiveBuffer, " ");
        fileLocation = strdup(argsv[1]);
        filePath = strtok(NULL, " ");
        strcpy(body, "");
        bzero(data, sizeof(data));
        if (!(fp = fopen(strcat(fileLocation, filePath), "r"))) {
            snprintf(response, sizeof(response),
                "%sContent-Length: %zu\n\n%s", ERRORHeaders, strlen(ErrorBody), ErrorBody);
            send(fd, response, sizeof(response), 0);
            close(fd);
            continue;
        }
        while(fgets(data, 1024, fp) != NULL) {
            strcat(body, data);
            bzero(data, 1024);
        }
        snprintf(response, sizeof(response),
            "%sContent-Length: %zu\n\n%s", OKResponseHeader, strlen(body), body);
        if(send(fd, response, sizeof(response), 0) == -1) {
            perror("[-]Error in sending file.");
            exit(1);
        }
        fclose(fp);
        close(fd);
    }
}
