/*
** EPITECH PROJECT, 2026
** my_ftp
** File description:
** my_ftp.h
*/

#ifndef MY_FTP_H_
    #define MY_FTP_H_

    #include "args.h"
    #include <netinet/in.h>
    #include <stdbool.h>

    #define POLLER_INIT_SIZE 50

// For all dynamic arrays (this one and all future ones):
// amount: current amount of elements in the array
// size: actual malloc'd size of the array
typedef struct {
    struct pollfd *fds;
    unsigned int amount;
    unsigned int size;
} poller_t;

poller_t *poller_init(void);
void poller_free(poller_t *poller);

typedef struct {
    int *fds;
    unsigned int amount;
    unsigned int size;
} data_t;

typedef struct {
    poller_t *poller;
    // CONTROL socket aka the main server socket
    int control_fd;
    data_t *data;
} ftp_t;

// Sockets
int socket_init(in_port_t port);

void ftp_free(ftp_t *ftp);
void my_ftp(args_t *args);

#endif
