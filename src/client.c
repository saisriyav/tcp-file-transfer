#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#include "config.h"
#include "network.h"
#include "utils.h"
#include "crypto.h"

int main(int argc, char *argv[]) {
    int client_fd;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE];

    FILE *file;
    int bytes_read;

    char *server_ip = DEFAULT_SERVER_IP;
    int port = DEFAULT_PORT;
    char *filename;

    int filename_length;
    long file_size;
    long total_sent = 0;

    double start_time;
    double end_time;
    double elapsed_time;
    unsigned char file_hash[SHA256_HASH_SIZE];
    char hash_string[SHA256_STRING_SIZE];
    if (argc == 2) {
        filename = argv[1];
    } else if (argc == 4) {
        server_ip = argv[1];
        port = atoi(argv[2]);
        filename = argv[3];
    } else {
        printf("Usage:\n");
        printf("  %s <filename>\n", argv[0]);
        printf("  %s <server_ip> <port> <filename>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    file = fopen(filename, "rb");

    if (file == NULL) {
        log_error("File open failed");
        exit(EXIT_FAILURE);
    }

    fseek(file, 0, SEEK_END);
    file_size = ftell(file);
    rewind(file);
    if (calculate_sha256(filename, file_hash) != 0) {
    log_error("SHA-256 calculation failed");
    fclose(file);
    exit(EXIT_FAILURE);
    }

    sha256_to_string(file_hash, hash_string);

    printf("[INFO] SHA-256 : %s\n", hash_string);

    /* Rewind again because SHA-256 already read the file */
    rewind(file);

    if (file_size < 0) {
        log_error("File size check failed");
        fclose(file);
        exit(EXIT_FAILURE);
    }

    filename_length = strlen(filename);

    if (filename_length <= 0 || filename_length >= MAX_FILENAME_LENGTH) {
        printf("[ERROR] Invalid filename length\n");
        fclose(file);
        exit(EXIT_FAILURE);
    }

    client_fd = socket(AF_INET, SOCK_STREAM, 0);

    if (client_fd < 0) {
        log_error("Socket creation failed");
        fclose(file);
        exit(EXIT_FAILURE);
    }

    log_info("Client socket created successfully");

    memset(&server_addr, 0, sizeof(server_addr));

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);

    if (inet_pton(AF_INET, server_ip, &server_addr.sin_addr) <= 0) {
        log_error("Invalid server IP address");
        fclose(file);
        close(client_fd);
        exit(EXIT_FAILURE);
    }

    if (connect(client_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        log_error("Connection failed");
        fclose(file);
        close(client_fd);
        exit(EXIT_FAILURE);
    }

    printf("[INFO] Connected to server %s:%d\n", server_ip, port);

    log_info("Sending metadata");

    if (send_all(client_fd, &filename_length, sizeof(filename_length)) < 0 ||
    send_all(client_fd, filename, filename_length) < 0 ||
    send_all(client_fd, &file_size, sizeof(file_size)) < 0 ||
    send_all(client_fd, file_hash, SHA256_HASH_SIZE) < 0) {
        log_error("Metadata send failed");
        fclose(file);
        close(client_fd);
        exit(EXIT_FAILURE);
    }

    log_info("Sending file");

    start_time = get_time_in_seconds();

    while ((bytes_read = fread(buffer, 1, BUFFER_SIZE, file)) > 0) {
        if (send_all(client_fd, buffer, bytes_read) < 0) {
            log_error("Send failed");
            fclose(file);
            close(client_fd);
            exit(EXIT_FAILURE);
        }

        total_sent += bytes_read;
        print_progress(total_sent, file_size);
    }

    end_time = get_time_in_seconds();
    elapsed_time = end_time - start_time;

    printf("\n[INFO] File sent successfully\n");
    print_transfer_stats(file_size, elapsed_time);

    fclose(file);
    close(client_fd);

    return 0;
}
