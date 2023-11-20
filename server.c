#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define PORT 8080

int main() {
    int server_fd, new_socket;
    struct sockaddr_in server_addr, client_addr;
    int opt = 1;
    int addrlen = sizeof(client_addr);

    // Create a socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Set socket options to reuse address
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1) {
        perror("Setsockopt failed");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    // Bind the socket
    if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(server_fd, 3) < 0) {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d...\n", PORT);

    while (1) {
        // Accept a new connection
        if ((new_socket = accept(server_fd, (struct sockaddr*)&client_addr, (socklen_t*)&addrlen)) < 0) {
            perror("Accept failed");
            exit(EXIT_FAILURE);
        }

        printf("Connection accepted from %s:%d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

        while (1) {
            // Process the HTTP request and send a response
            char request_buffer[4096];
            ssize_t bytes_received = recv(new_socket, request_buffer, sizeof(request_buffer) - 1, 0);

            if (bytes_received > 0) {
                request_buffer[bytes_received] = '\0';
                printf("Received request:\n%s\n", request_buffer);

                // Send an HTTP response
                char response[] = "HTTP/1.1 200 OK\r\nContent-Length: 12\r\n\r\nHello, World!";
                send(new_socket, response, strlen(response), 0);
            } else if (bytes_received == 0) {
                // Connection closed by the client
                printf("Connection closed by the client.\n");
                break;
            } else {
                // Error in recv
                perror("Receive failed");
                break;
            }
        }

        // Close the socket after all requests from the same client
        close(new_socket);
    }

    // Close the server socket when done
    close(server_fd);

    return 0;
}
