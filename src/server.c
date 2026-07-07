#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

#include "config.h"
#include "network.h"
#include "utils.h"
#include "crypto.h"

void *handle_client(void *arg) {
    int client_fd = *(int *)arg;
    free(arg);

    char buffer[BUFFER_SIZE];
    char filename[MAX_FILENAME_LENGTH];
    char output_filename[MAX_FILENAME_LENGTH + 20];

    FILE *file;

    int filename_length;
    long file_size;
    long total_received = 0;
    int bytes_received;
    int bytes_to_receive;

    double start_time;
    double end_time;
    double elapsed_time;
    unsigned char client_hash[SHA256_HASH_SIZE];
    unsigned char server_hash[SHA256_HASH_SIZE];

    char client_hash_string[SHA256_STRING_SIZE];
    char server_hash_string[SHA256_STRING_SIZE];

    log_info("Worker thread started for client");

    if (recv_all(client_fd, &filename_length, sizeof(filename_length)) < 0) {
        log_error("Filename length receive failed");
        close(client_fd);
        return NULL;
    }
    
    if (filename_length <= 0 || filename_length >= MAX_FILENAME_LENGTH) {
        log_info("Invalid filename length");
        close(client_fd);
        return NULL;
    }

    memset(filename, 0, sizeof(filename));

    if (recv_all(client_fd, filename, filename_length) < 0) {
        log_error("Filename receive failed");
        close(client_fd);
        return NULL;
    }

    filename[filename_length] = '\0';

    if (recv_all(client_fd, &file_size, sizeof(file_size)) < 0) {
        log_error("File size receive failed");
        close(client_fd);
        return NULL;
    }
    if (recv_all(client_fd, client_hash, SHA256_HASH_SIZE) < 0) {
        log_error("SHA-256 hash receive failed");
        close(client_fd);
        return NULL;
    }
    snprintf(output_filename, sizeof(output_filename), "received_%s", filename);

    file = fopen(output_filename, "wb");

    if (file == NULL) {
        log_error("File creation failed");
        close(client_fd);
        return NULL;
    }

    log_info("Receiving file");

    start_time = get_time_in_seconds();

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
        print_progress(total_received, file_size);
    }

    end_time = get_time_in_seconds();
    elapsed_time = end_time - start_time;

    printf("\n");
    fclose(file);
    file = NULL;
    if (total_received == file_size) {
        printf("[INFO] File received successfully and saved as %s\n", output_filename);
        print_transfer_stats(file_size, elapsed_time);
        if (calculate_sha256(output_filename, server_hash) != 0) {
            log_error("Server SHA-256 calculation failed");
        } else {
            sha256_to_string(client_hash, client_hash_string);
            sha256_to_string(server_hash, server_hash_string);

            printf("[INFO] Client SHA-256 : %s\n", client_hash_string);
            printf("[INFO] Server SHA-256 : %s\n", server_hash_string);

            if (compare_sha256(client_hash, server_hash) == 0) {
               printf("[INFO] Integrity verification passed\n");
        } else {
               printf("[ERROR] Integrity verification failed\n");
        }
    }
    } else {
        printf("[ERROR] File transfer incomplete. Expected %ld bytes, received %ld bytes\n",
               file_size, total_received);
    }

    if (file != NULL) {
    fclose(file);
    }

    close(client_fd);

    log_info("Client disconnected. Worker thread finished");

    return NULL;
}

int main(int argc, char *argv[]) {
    int server_fd;
    int client_fd;
    int port = DEFAULT_PORT;

    struct sockaddr_in server_addr;
    pthread_t thread;

    if (argc == 2) {
        port = atoi(argv[1]);
    } else if (argc > 2) {
        printf("Usage: %s [port]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    server_fd = socket(AF_INET, SOCK_STREAM, 0);

    if (server_fd < 0) {
        log_error("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    log_info("Server socket created successfully");

    memset(&server_addr, 0, sizeof(server_addr));

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        log_error("Bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("[INFO] Server bound to port %d\n", port);

    if (listen(server_fd, BACKLOG) < 0) {
        log_error("Listen failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    log_info("Server is listening for client connections");

    while (1) {
        client_fd = accept(server_fd, NULL, NULL);

        if (client_fd < 0) {
            log_error("Accept failed");
            continue;
        }

        log_info("Client connected");

        int *client_socket = malloc(sizeof(int));

        if (client_socket == NULL) {
            log_error("Memory allocation failed");
            close(client_fd);
            continue;
        }

        *client_socket = client_fd;

        if (pthread_create(&thread, NULL, handle_client, client_socket) != 0) {
            log_error("Thread creation failed");
            close(client_fd);
            free(client_socket);
            continue;
        }

        pthread_detach(thread);
    }

    close(server_fd);

    return 0;
}
