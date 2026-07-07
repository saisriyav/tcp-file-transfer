#ifndef NETWORK_H
#define NETWORK_H

#include <stddef.h>

int send_all(int socket_fd, const void *data, size_t data_size);
int recv_all(int socket_fd, void *buffer, size_t data_size);

#endif
