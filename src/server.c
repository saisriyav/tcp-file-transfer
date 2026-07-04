#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

#define PORT 8080
#define BUFFER_SIZE 1024
#define FILENAME_SIZE 256

int recv_all(int socket_fd, void *buffer, int data_size) {
    int total_received = 0;
    int bytes_received;

    while (total_received < data_size) {
        bytes_received = recv(socket_fd,
                              (char *)buffer + total_received,
                              data_size - total_received,
                              0);

        if (bytes_received <= 0) {
            return -1;
        }

        total_received += bytes_received;
    }

    return 0;
}

void *handle_client(void *arg) {
    int client_fd = *(int *)arg;
    free(arg);

    char buffer[BUFFER_SIZE];
    char filename[FILENAME_SIZE];
    char output_filename[FILENAME_SIZE + 20];

    FILE *file;

    int filename_length;
    long file_size;
    long total_received = 0;
    int bytes_received;
    int bytes_to_receive;

    printf("Worker thread started for client.\n");

    if (recv_all(client_fd, &filename_length, sizeof(filename_length)) < 0) {
        perror("Filename length receive failed");
        close(client_fd);
        return NULL;
    }

    if (filename_length <= 0 || filename_length >= FILENAME_SIZE) {
        printf("Invalid filename length.\n");
        close(client_fd);
        return NULL;
    }

    memset(filename, 0, sizeof(filename));

    if (recv_all(client_fd, filename, filename_length) < 0) {
        perror("Filename receive failed");
        close(client_fd);
        return NULL;
    }

    filename[filename_length] = '\0';

    if (recv_all(client_fd, &file_size, sizeof(file_size)) < 0) {
        perror("File size receive failed");
        close(client_fd);
        return NULL;
    }

    snprintf(output_filename, sizeof(output_filename), "received_%s", filename);

    file = fopen(output_filename, "wb");

    if (file == NULL) {
        perror("File creation failed");
        close(client_fd);
        return NULL;
    }

    while (total_received < file_size) {
        bytes_to_receive = BUFFER_SIZE;

        if (file_size - total_received < BUFFER_SIZE) {
            bytes_to_receive = file_size - total_received;
        }

        bytes_received = recv(client_fd, buffer, bytes_to_receive, 0);

        if (bytes_received <= 0) {
            break;
        }

        fwrite(buffer, 1, bytes_received, file);
        total_received += bytes_received;
    }

    if (total_received == file_size) {
        printf("File received successfully and saved as %s.\n", output_filename);
    } else {
        printf("File transfer incomplete. Expected %ld bytes, received %ld bytes.\n",
               file_size, total_received);
    }

    fclose(file);
    close(client_fd);

    printf("Client disconnected. Worker thread finished.\n");

    return NULL;
}

int main() {
    int server_fd;
    int client_fd;

    struct sockaddr_in server_addr;
    pthread_t thread;

    server_fd = socket(AF_INET, SOCK_STREAM, 0);

    if (server_fd < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    printf("Server socket created successfully.\n");

    memset(&server_addr, 0, sizeof(server_addr));

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("Server bound to port %d.\n", PORT);

    if (listen(server_fd, 5) < 0) {
        perror("Listen failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    while (1) {
        printf("\n=====================================\n");
        printf("Server is waiting for a client connection...\n");

        client_fd = accept(server_fd, NULL, NULL);

        if (client_fd < 0) {
            perror("Accept failed");
            continue;
        }

        printf("Client connected.\n");

        int *client_socket = malloc(sizeof(int));

        if (client_socket == NULL) {
            perror("Memory allocation failed");
            close(client_fd);
            continue;
        }

        *client_socket = client_fd;

        if (pthread_create(&thread, NULL, handle_client, client_socket) != 0) {
            perror("Thread creation failed");
            close(client_fd);
            free(client_socket);
            continue;
        }

        pthread_detach(thread);
    }

    close(server_fd);

    return 0;
}
