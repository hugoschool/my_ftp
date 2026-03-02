# my_ftp

Recreate a File Transfer Protocol (FTP) server.

It's respecting a subset of the RFC 959 provided by the subject (see `epitech-protocol.txt`).

First project in the `Network Programming` module.

Subject is available in the PDF file.

## Build

You will need a C compiler and GNU Make to compile the project.

```sh
make -j`nproc`
```

## Usage

```
./myftp --help
```

You can then use a `telnet` client to connect to the FTP server.

```sh
./myftp 4242 .
telnet 127.0.0.1 4242
```

A `nc` server can be used for the active mode (`PORT` command)

```sh
nc -l 3106

# In the client, type `PORT 0,0,0,0,12,34`.
```
