#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

void error(const char *msg) {
    perror(msg); // Displays the error and ends the program
    exit(1); // Changed from exit(0) to exit(1) to indicate an error exit status
}

int main(int argc, char *argv[]) {
    int sockfd, portno, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    char buffer[256];

    printf("Starting the client...\n");

    if (argc < 3) {
        fprintf(stderr, "Usage: %s hostname port\n", argv[0]);
        exit(1); // Error exit status
    }

    // Convert the port number from string to int
    portno = atoi(argv[2]);
    if (portno <= 0) {
        fprintf(stderr, "ERROR: Invalid port provided.\n");
        exit(1);
    }

    printf("Creating a socket...\n");
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        error("ERROR opening socket");

    printf("Looking up the host '%s'...\n", argv[1]);
    server = gethostbyname(argv[1]);
    if (server == NULL) {
        fprintf(stderr, "ERROR: No such host found.\n");
        exit(1);
    }

    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr,
          (char *)&serv_addr.sin_addr.s_addr,
          server->h_length);
    serv_addr.sin_port = htons(portno);

    printf("Connecting to the server at %s on port %d...\n", argv[1], portno);
    if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
        error("ERROR connecting");

    printf("Successfully connected to the server.\nPlease enter the message to send: ");
    bzero(buffer, 256);
    fgets(buffer, 255, stdin);

    n = write(sockfd, buffer, strlen(buffer));
    if (n < 0)
        error("ERROR writing to socket");
    else
        fprintf(stderr, "Sent %d bytes to the server.\n", n);

    printf("Closing the socket and terminating the client.\n");
    close(sockfd);

    return 0;
}
