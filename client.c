#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 1024

int send_all(int socket_fd, void *data, int data_size) {
    int total_sent = 0;
    int bytes_sent;

    while (total_sent < data_size) {
        bytes_sent = send(socket_fd,
                          (char *)data + total_sent,
                          data_size - total_sent,
                          0);

        if (bytes_sent <= 0) {
            return -1;
        }

        total_sent += bytes_sent;
    }

    return 0;
}

int main(int argc, char *argv[]) {
    int client_fd;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE];

    FILE *file;
    int bytes_read;

    char *filename;
    int filename_length;
    long file_size;

    if (argc != 2) {
        printf("Usage: %s <filename>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    filename = argv[1];

    client_fd = socket(AF_INET, SOCK_STREAM, 0);

    if (client_fd < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    printf("Client socket created successfully.\n");

    memset(&server_addr, 0, sizeof(server_addr));

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);

    if (inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr) <= 0) {
        perror("Invalid address");
        close(client_fd);
        exit(EXIT_FAILURE);
    }

    if (connect(client_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connection failed");
        close(client_fd);
        exit(EXIT_FAILURE);
    }

    printf("Connected to server.\n");

    file = fopen(filename, "rb");

    if (file == NULL) {
        perror("File open failed");
        close(client_fd);
        exit(EXIT_FAILURE);
    }

    fseek(file, 0, SEEK_END);
    file_size = ftell(file);
    rewind(file);

    filename_length = strlen(filename);

    if (send_all(client_fd, &filename_length, sizeof(filename_length)) < 0 ||
        send_all(client_fd, filename, filename_length) < 0 ||
        send_all(client_fd, &file_size, sizeof(file_size)) < 0) {
        perror("Metadata send failed");
        fclose(file);
        close(client_fd);
        exit(EXIT_FAILURE);
    }

    while ((bytes_read = fread(buffer, 1, BUFFER_SIZE, file)) > 0) {
        if (send_all(client_fd, buffer, bytes_read) < 0) {
            perror("Send failed");
            fclose(file);
            close(client_fd);
            exit(EXIT_FAILURE);
        }
    }

    printf("File sent successfully.\n");

    fclose(file);
    close(client_fd);

    return 0;
}
