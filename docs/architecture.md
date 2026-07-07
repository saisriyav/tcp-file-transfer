# System Architecture

## Overview

The Linux Multithreaded TCP File Transfer System is designed using a client-server architecture built on top of the TCP protocol. The client establishes a TCP connection with the server, sends file metadata followed by the file contents, and terminates the connection after successful transmission.

The server listens continuously for incoming client connections. Each client is handled in an independent POSIX thread, allowing multiple file transfers to occur concurrently without blocking new connections.

---

## High-Level Architecture

```
                +----------------------+
                |       Client         |
                +----------------------+
                           |
                    TCP Connection
                           |
===========================|===========================
                           |
                +----------------------+
                |       Server         |
                +----------------------+
                           |
                  pthread_create()
                           |
                +----------------------+
                |    Worker Thread     |
                +----------------------+
                           |
            Receive Metadata & File Data
                           |
                  Compute SHA-256 Hash
                           |
                 Verify File Integrity
                           |
                  Save File to Disk
```

---

## Client Workflow

1. Parse command-line arguments.
2. Create a TCP socket.
3. Connect to the server.
4. Calculate the SHA-256 hash of the file.
5. Send file metadata:
   - Filename length
   - Filename
   - File size
   - SHA-256 hash
6. Send file contents.
7. Display progress and transfer statistics.
8. Close the connection.

---

## Server Workflow

1. Create a listening TCP socket.
2. Bind to the configured port.
3. Listen for incoming client connections.
4. Accept a new client.
5. Create a worker thread using POSIX Threads.
6. Receive metadata from the client.
7. Receive and store the file.
8. Calculate the SHA-256 hash of the received file.
9. Compare the calculated hash with the client-provided hash.
10. Report whether integrity verification passed or failed.
11. Close the client connection.

---

## Threading Model

Each accepted client connection is handled independently using `pthread_create()`. The worker thread performs the complete file transfer and integrity verification process before terminating.

This design allows the server to continue accepting new client connections while existing transfers are processed concurrently.

---

## Networking Design

The application uses TCP sockets to provide reliable, ordered, and error-checked delivery of file data.

Reliable transmission is implemented through helper functions:

- `send_all()`
- `recv_all()`

These functions repeatedly call `send()` and `recv()` until all requested bytes have been transmitted or received.

---

## File Transfer Process

```
Client
   │
   ├── Filename Length
   ├── Filename
   ├── File Size
   ├── SHA-256 Hash
   └── File Data
               │
               ▼
Server
   │
   ├── Receive Metadata
   ├── Receive File
   ├── Save File
   ├── Compute SHA-256
   └── Verify Integrity
```

---

## Error Handling

The application validates:

- Socket creation
- Connection establishment
- File opening
- Memory allocation
- Data transmission
- File reception
- SHA-256 computation

Meaningful error messages are displayed whenever an operation fails.

---

## Performance Monitoring

During each transfer, the application measures:

- Transfer progress
- Total transfer time
- File size
- Average transfer speed

These statistics are displayed upon successful completion of the transfer.

---

## Security

File integrity is verified using the SHA-256 cryptographic hash algorithm.

The client computes the hash before transmission and sends it as part of the metadata. After the server receives and stores the file, it computes its own SHA-256 hash and compares the two values.

If both hashes match, the transfer is confirmed to be free from corruption.

---

## Design Goals

The project was designed with the following objectives:

- Modular software architecture
- Reliable file transfer
- Concurrent client handling
- Clear separation of networking and utility functions
- Ease of maintenance
- Extensibility for future enhancements such as TLS encryption and IPv6 support
