#include <sys/socket.h>
#include "network.h"

int send_all(int socket_fd, const void *data, size_t data_size) {
    size_t total_sent = 0;

    while (total_sent < data_size) {
        ssize_t bytes_sent = send(socket_fd,
                                  (const char *)data + total_sent,
                                  data_size - total_sent,
                                  0);

        if (bytes_sent <= 0) {
            return -1;
        }

        total_sent += bytes_sent;
    }

    return 0;
}

int recv_all(int socket_fd, void *buffer, size_t data_size) {
    size_t total_received = 0;

    while (total_received < data_size) {
        ssize_t bytes_received = recv(socket_fd,
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
