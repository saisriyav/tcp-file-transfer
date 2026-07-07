# Linux Multithreaded TCP File Transfer System in C

A high-performance TCP client-server file transfer application developed in **C** on **Linux** using **POSIX sockets** and **POSIX threads**. The application supports reliable file transmission, concurrent client handling, configurable networking, real-time transfer monitoring, performance analysis, and SHA-256 integrity verification through a custom application-layer protocol.

---

## Overview

This project demonstrates the implementation of a production-style TCP file transfer system built using low-level Linux networking APIs. The application emphasizes reliable communication, modular software design, concurrent programming, and secure file verification.

The client transfers files to the server over TCP while transmitting metadata required for reconstruction and integrity validation. Each client connection is handled independently using POSIX threads, allowing multiple clients to be served concurrently.

---

## Key Features

- TCP client-server architecture
- POSIX socket programming
- Multithreaded server using POSIX Threads
- Concurrent client handling
- Configurable server IP address and port
- Reliable data transmission using `send_all()` and `recv_all()`
- Custom application-layer protocol
- Binary file transfer support
- Real-time transfer progress
- Transfer performance statistics
- SHA-256 integrity verification
- Structured logging
- Modular software architecture
- Build automation using Make

---

## System Architecture

![Architecture Diagram](docs/architecture-diagram.svg)

---

## Project Structure

```text
tcp-file-transfer/
│
├── include/
│   ├── config.h
│   ├── crypto.h
│   ├── network.h
│   └── utils.h
│
├── src/
│   ├── client.c
│   ├── server.c
│   ├── network.c
│   ├── crypto.c
│   └── utils.c
│
├── docs/
│   ├── architecture.md
│   ├── protocol.md
│   ├── architecture-diagram.svg
│   └── sequence-diagram.svg
│
├── screenshots/
│   ├── client-terminal.png
│   ├── server-terminal.png
│   ├── verification.png
│   └── project-structure.png
│
├── Makefile
├── README.md
└── LICENSE
```

---

## Application Protocol

The client transmits metadata followed by the file contents in the following order.

| Order | Field | Description |
|------:|-------|-------------|
| 1 | Filename Length | Length of the filename |
| 2 | Filename | Original filename |
| 3 | File Size | Total size of the file in bytes |
| 4 | SHA-256 Hash | SHA-256 digest used for integrity verification |
| 5 | File Data | Binary file contents |

### Communication Flow

![Sequence Diagram](docs/sequence-diagram.svg)

---

## Build

### Install Dependencies

```bash
sudo apt update
sudo apt install libssl-dev
```

### Compile

```bash
make clean
make
```

---

## Usage

### Start the Server

```bash
./server
```

Run on a custom port:

```bash
./server 8080
```

### Send a File

Using default settings:

```bash
./client sample.txt
```

Specify server IP and port:

```bash
./client 127.0.0.1 8080 test_10mb.bin
```

---

## Screenshots

### Client

![Client Terminal](screenshots/client-terminal.png)

### Server

![Server Terminal](screenshots/server-terminal.png)

### SHA-256 Integrity Verification

![Verification](screenshots/verification.png)

### Project Structure

![Project Structure](screenshots/project-structure.png)

---

## Skills Demonstrated

- C Programming
- Linux System Programming
- POSIX Socket Programming
- TCP/IP Networking
- Client-Server Architecture
- Concurrent Programming
- POSIX Threads
- Custom Application-Layer Protocol Design
- SHA-256 Cryptographic Hashing (OpenSSL)
- Reliable File Transfer
- Binary File Processing
- Performance Measurement
- Error Handling
- Modular Software Design
- Build Automation using Make

---

## Future Enhancements

- TLS/SSL encrypted communication
- IPv6 support
- Multiple file transfer
- Resume interrupted transfers
- File compression
- Authentication and authorization
- Bandwidth throttling
- Timestamped logging
- Configuration file support

---

## Documentation

Additional design documentation is available in:

- `docs/architecture.md`
- `docs/protocol.md`

---

## License

This project is licensed under the MIT License.
