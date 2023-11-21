// Chat Application - UDP Client
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
    struct sockaddr_in servaddr;

    // Creating socket file descriptor
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&servaddr, 0, sizeof(servaddr));

    // Filling server information
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(PORT);
    servaddr.sin_addr.s_addr = INADDR_ANY;

    socklen_t len;

    printf("Client connected to the server.\n");

    while (1) {
        // Get user input to send to the server
        printf("Client: ");
        fgets(buffer, sizeof(buffer), stdin);

        // Send the user input to the server
        sendto(sockfd, (const char *)buffer, strlen(buffer), 0, (const struct sockaddr *)&servaddr, sizeof(servaddr));

        if (strcmp(buffer, "exit\n") == 0) {
            // Client requested exit
            printf("Exiting client...\n");
            break;
        }

        // Receive message from the server
        len = sizeof(servaddr);
        int n = recvfrom(sockfd, (char *)buffer, MAXLINE, 0, (struct sockaddr *)&servaddr, &len);

        if (n == 0) {
            // Server disconnected
            printf("Server disconnected.\n");
            break;
        }

        buffer[n] = '\0';
        printf("Server: %s\n", buffer);
    }

    close(sockfd);
    return 0;
}
