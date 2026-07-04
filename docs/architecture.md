# Architecture

```
             Client

                │

          TCP Connection

                │

                ▼

      +------------------+

      |      Server      |

      +------------------+

                │

             accept()

                │

       pthread_create()

                │

      +------------------+

      | Worker Thread    |

      +------------------+

                │

     Receive Metadata

                │

     Receive File

                │

     Save File
```

---

## Components

### Client

- Opens the selected file
- Collects metadata
- Sends metadata
- Sends file contents

### Server

- Waits for incoming connections
- Creates one worker thread per client
- Receives metadata
- Receives file
- Saves received file

---

## Thread Model

One process

↓

Main Thread

↓

accept()

↓

Worker Thread

↓

Client

Each client is handled independently.
