# Application Protocol

## Overview

The TCP File Transfer Application uses a custom application-layer protocol to transfer both metadata and file contents reliably over a TCP connection.

---

## Packet Format

```
+------------------------------+
| Filename Length (4 bytes)    |
+------------------------------+
| Filename (N bytes)           |
+------------------------------+
| File Size (8 bytes)          |
+------------------------------+
| File Data                    |
+------------------------------+
```

---

## Packet Flow

Client

↓

Connect

↓

Send Filename Length

↓

Send Filename

↓

Send File Size

↓

Send File Data

↓

Disconnect

---

## Why This Design?

TCP is a byte-stream protocol.

It does not preserve message boundaries.

Therefore the application sends metadata before the file so the server knows:

- filename length
- filename
- expected file size
- when transfer is complete

---

## Reliability

Metadata is transmitted using:

- send_all()

- recv_all()

These helper functions ensure all requested bytes are transmitted even if send() or recv() returns partial results.
