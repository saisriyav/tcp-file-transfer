# Application Protocol Specification

## Overview

The Linux Multithreaded TCP File Transfer System uses a custom application-layer protocol built on top of TCP.

TCP provides reliable, ordered, and error-checked delivery of data. This protocol defines the format and sequence of information exchanged between the client and server so that files can be reconstructed accurately and verified using SHA-256.

---

# Communication Flow

```
Client                           Server

socket()

connect() ---------------------->

                        accept()

Filename Length ---------------->

Filename ----------------------->

File Size ---------------------->

SHA-256 Hash ------------------->

File Data ---------------------->

                        Save File

                        Compute SHA-256

                        Verify Integrity

                        Close Connection
```

---

# Packet Structure

The client transmits data in the following order.

| Order | Field | Size | Description |
|------:|-------|------|-------------|
| 1 | Filename Length | 4 Bytes | Length of the filename |
| 2 | Filename | Variable | Original filename |
| 3 | File Size | 8 Bytes | Total size of the file |
| 4 | SHA-256 Hash | 32 Bytes | SHA-256 digest of the original file |
| 5 | File Data | Variable | Raw file contents |

---

# Metadata

## Filename Length

The client first sends the length of the filename.

Example:

```
12
```

This allows the server to determine how many bytes must be read for the filename.

---

## Filename

The original filename is transmitted.

Example:

```
report.pdf
```

The server stores the received file as:

```
received_report.pdf
```

---

## File Size

The total file size (in bytes) is transmitted before sending the file.

Example:

```
10485760
```

This enables the server to know when the complete file has been received.

---

## SHA-256 Hash

The client computes the SHA-256 hash before transmission.

Example:

```
e5b844cc57f57094ea4585e235f36c78...
```

The server computes the SHA-256 hash again after writing the file and compares both values.

Matching hashes indicate that the file was transferred without corruption.

---

## File Data

The file is transmitted as a continuous stream of bytes.

Data is sent in fixed-size chunks using:

- `send_all()`
- `recv_all()`

This ensures complete transmission even when individual `send()` or `recv()` calls transfer fewer bytes than requested.

---

# Protocol Sequence

```
Client

Filename Length
        │
Filename
        │
File Size
        │
SHA-256 Hash
        │
File Data
        ▼

================ TCP ================

        ▲

Server

Receive Filename Length

Receive Filename

Receive File Size

Receive SHA-256 Hash

Receive File Data

Compute SHA-256

Compare Hashes

Save File
```

---

# Integrity Verification

After the file transfer completes:

1. The server computes the SHA-256 hash of the received file.
2. The received hash is converted into hexadecimal format.
3. The client hash and server hash are compared.
4. If they match:

```
Integrity verification passed
```

Otherwise:

```
Integrity verification failed
```

---

# Error Handling

The protocol validates:

- Invalid filename length
- Socket communication failures
- Connection interruptions
- File opening errors
- Partial file transfers
- SHA-256 calculation failures

Appropriate error messages are displayed for each failure condition.

---

# Design Considerations

The protocol was designed with the following goals:

- Simple implementation
- Reliable transmission
- Binary file support
- Metadata separation
- File integrity verification
- Extensibility for future enhancements

Future protocol extensions may include:

- TLS encryption
- File compression
- Multiple file transfer
- Resume interrupted transfers
- Authentication
