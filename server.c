#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>

void error(const char *msg) {
    perror(msg); // Displays the error and exits the program
    exit(1);
}

int main(int argc, char *argv[]) {
    int sockfd, newsockfd, portno, clilen, n;
    char buffer[256];
    struct sockaddr_in serv_addr, cli_addr;

    if (argc < 2) {
        fprintf(stderr, "ERROR, no port provided\n");
        exit(1);
    }

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        error("ERROR opening socket");

    bzero((char *) &serv_addr, sizeof(serv_addr));
    portno = atoi(argv[1]);
    if (portno <= 0) {
        fprintf(stderr, "ERROR, invalid port number provided\n");
        exit(1);
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);

    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
        error("ERROR on binding");

    listen(sockfd, 5);
    clilen = sizeof(cli_addr);

    printf("Waiting for a connection on port %d...\n", portno);
    newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
    if (newsockfd < 0)
        error("ERROR on accept");

    printf("Connection established with a client.\n");
    bzero(buffer, 256);
    n = read(newsockfd, buffer, 255);
    if (n < 0)
        error("ERROR reading from socket");

    printf("Here is the message: %s\n", buffer);
    fprintf(stderr, "Received %d bytes\n", n);

    n = write(newsockfd, "I got your message", 18);
    if (n < 0)
        error("ERROR writing to socket");

    close(newsockfd);
    close(sockfd);
    printf("Connection closed and server terminating.\n");

    return 0;
}
