# OperatingSystemsLab

Yet another repo about os  
Based on the course of oslab in ece-ntua, 2020

### Basic chatting tool

Two-way communication over TCP/IP, using BSD Sockets Api with no message encryption.

```bash
$ cd basic-chat-tool
$ make

# Run server
$ ./server_z1 <PORT>
# Run client
$ ./client_z1 <SERVER_HOST> <SERVER_PORT>

# Exchange messages between them
#e.g.
$ ./server_z1 8080
$ ./client 127.0.0.1 8080
```

### Encrypted chatting tool

Using <a href='https://github.com/cryptodev-linux/cryptodev-linux'>Cryptdev-linux</a> device driver as encryption method.

```bash
$ cd cryptodev-chat-tool
# Extract the cryptodev-linux gz file

# Cd into the extracted directory
$ make

# Will cp cryptodev.h to /usr/local/include/crypto/cryptodev.h, which we will use
$ sudo make install

# Load module into Linux kernel
$ sudo insmod cryptodev.ko

# Cd back to cryptodev-chat-tool and run makefile
# Run server
$ ./server-enc <PORT>
# Run client
$ ./client_enc <SERVER_HOST> <SERVER_PORT>
```
