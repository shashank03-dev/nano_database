# NanoDB: A Minimal In-Memory Key-Value Store

<div align="center">

![C](https://img.shields.io/badge/language-C-blue.svg)
![License](https://img.shields.io/badge/license-MIT-green.svg)
![Status](https://img.shields.io/badge/status-educational-orange.svg)

**NanoDB** is a lightweight, persistent key-value store built from scratch in C. Designed as an educational prototype, it demonstrates the core concepts behind high-performance databases like **Redis** and **Bitcask**.

[Architecture](#-architecture) •
[Quick Start](#-quick-start) •
[Usage](#-usage-guide) •
[Internals](#-under-the-hood)

</div>

---

## 📖 About

NanoDB is not just another wrapper around `sqlite`. It is a custom implementation of a **Log-Structured Database**.

Traditional databases often struggle with write performance because they need to seek random locations on the disk to update records. NanoDB solves this by using an **Append-Only Log** architecture, ensuring $O(1)$ write performance while maintaining data persistence. Read operations are accelerated using an in-memory **Hash Table** index.

## 🏗 Architecture

NanoDB employs a hybrid storage model combining Disk and RAM:

### 1. The Storage Engine (Append-Only Log)

- **Concept**: Data is never overwritten. Updates are simply appended to the end of the database file (`.db`).
- **Benefit**: Sequential writes are significantly faster than random writes, maximizing disk throughput.
- **Persistence**: Every `SET` command is flushed to disk immediately, ensuring durability against crashes.

### 2. The Index (In-Memory Hash Table)

- **Concept**: An in-memory index maps keys to their exact byte offset in the file.
- **Hashing**: Uses the **DJB2** algorithm to efficiently hash string keys to integer indices.
- **Performance**: Lookups are $O(1)$ (constant time). The server computes the hash, jumps (`fseek`) to the file location, and reads the value.

### 3. The Protocol (TCP Server)

- **Communication**: Operates over TCP sockets on port `8888`.
- **Interface**: Custom text-based protocol similar to Redis (e.g., `+OK`, `:1`).

## 🚀 Quick Start

### Prerequisites

- GCC Compiler
- Linux/Unix environment (Socket API)

### Build and Run

1.  **Clone and Compile**

    ```bash
    # Compile the server source code
    gcc server.c storage.c -o nanodb_server
    ```

2.  **Start the Server**
    ```bash
    ./nanodb_server
    ```
    _Output:_ `NanoDB Server started on port 8888...`

## 🎮 Usage Guide

You can interact with NanoDB using `netcat` (nc) or by writing a custom TCP client.

### Connect via Terminal

Open a new terminal window:

```bash
nc localhost 8888
```

### Supported Commands

| Command    | Syntax              | Description             | Example              |
| :--------- | :------------------ | :---------------------- | :------------------- |
| **SET**    | `SET <key> <value>` | Store a key-value pair. | `SET user:101 alice` |
| **GET**    | `GET <key>`         | Retrieve value by key.  | `GET user:101`       |
| **EXISTS** | `EXISTS <key>`      | Check if a key exists.  | `EXISTS user:101`    |
| **STATS**  | `STATS`             | Show server statistics. | `STATS`              |
| **QUIT**   | `QUIT`              | Disconnect from server. | `QUIT`               |

### Session Example

```text
SET user:1 shashank
> +OK

GET user:1
> shashank

EXISTS user:1
> :1

STATS
> Total Keys: 1
```

## 📺 Demo

<p align="center">
  <img src="demo.gif" width="700" alt="NanoDB Demo">
</p>

## 📂 Project Structure

- `server.c` - Main entry point. Handles TCP connections and protocol parsing.
- `storage.c` - Core database logic. Implements `db_set`, `db_get`, and file handling.
- `db.h` - Header file defining the `NanoDB` struct and function prototypes.
- `main.c` - A standalone CLI test runner (optional usage).
