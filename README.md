# Olilands Server

## Description

The **Olilands Server** is a game server developed in C++ that uses **Boost.Asio** for asynchronous communication and **spdlog** for efficient logging. Designed to handle multiple client connections in a scalable and robust manner, the server is ideal for games requiring high performance and low latency.

## Features

- **Asynchronous Communication**: Efficiently manages multiple client connections using Boost.Asio.
- **Advanced Logging**: Utilizes spdlog for detailed logs with support for various log levels.
- **Memory Monitoring**: A `memorymonitor` class that monitors memory usage and issues alerts when exceeding 80%.
- **Modular Structure**: Code organized in separate folders for easier maintenance and scalability.
- **Dynamic Configuration with CMake**: Build system configured to automatically include all source files.

## Architecture

The server's architecture is divided into the following main classes:

- **Server**: Responsible for accepting new client connections.
- **Session**: Manages communication with individual clients.
- **MemoryMonitor**: Monitors server memory usage and logs alerts as necessary.

## Requirements

- **C++20**
- **CMake** (minimum version 3.14)
- **Boost** (components: system)
- **spdlog**
- **Ninja** (optional but recommended for faster builds)
- **Xcode Command Line Tools** (for macOS)

## Installation

1. **Clone the Repository**

    ```bash
    git clone https://github.com/gutogirardon/olilands-server.git
    cd olilands-server
    ```

2. **Install Dependencies**

    - **Boost** and **spdlog** can be installed via Homebrew:

        ```bash
        brew install boost
        brew install spdlog
        ```

3. **Configure the Build with CMake**

    ```bash
    mkdir build
    cd build
    cmake ..
    ```

4. **Compile the Project**

    ```bash
    make
    ```

## Usage

After compilation, run the server:

```bash
./olilands_server
