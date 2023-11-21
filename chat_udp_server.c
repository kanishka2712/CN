// Chat Application - UDP Server
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#define PORT 8080
#define MAXLINE 1024

int main() {
    int sockfd;
    char buffer[MAXLINE];
    struct sockaddr_in servaddr, cliaddr;

    // Creating socket file descriptor
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&servaddr, 0, sizeof(servaddr));
    memset(&cliaddr, 0, sizeof(cliaddr));

    // Filling server information
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(PORT);

    // Bind the socket with the server address
    if (bind(sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    socklen_t len;
    int n;

    printf("Server is running...\n");

    while (1) {
        // Receive message from the client
        len = sizeof(cliaddr);
        n = recvfrom(sockfd, (char *)buffer, MAXLINE, 0, (struct sockaddr *)&cliaddr, &len);

        if (n == 0) {
            // Client disconnected
            printf("Client disconnected.\n");
            break;
        }

        buffer[n] = '\0';
        printf("Client: %s\n", buffer);

        if (strcmp(buffer, "exit\n") == 0) {
            // Client requested exit
            printf("Client has left the chat.\n");
            break;
        }

        // Get user input to send back to the client
        printf("Server: ");
        fgets(buffer, sizeof(buffer), stdin);

        // Send the user input back to the client
        sendto(sockfd, (const char *)buffer, strlen(buffer), 0, (const struct sockaddr *)&cliaddr, len);
    }

    close(sockfd);
    return 0;
}
