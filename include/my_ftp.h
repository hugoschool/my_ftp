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

    #define BUFFER_SIZE 4096
    #define CRLF "\r\n"

    #define CLIENTS_INIT_SIZE 50
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
void poller_fd_add(poller_t *poller, int fd);
void poller_fd_delete(poller_t *poller, int i);
void poller_set_init_socket(poller_t *poller, int socket_fd);
void poller_free(poller_t *poller);

typedef enum {
    LOGGED_OUT,
    USERNAME_TYPED,
    WRONG_PASSWORD,
    LOGGED_IN
} login_step_t;

typedef struct {
    // Pointer to its struct pollfd file descriptor
    int *fd;
    login_step_t login_step;
} client_data_t;

typedef struct {
    client_data_t **clients;
    unsigned int amount;
    unsigned int size;
} clients_t;

typedef struct {
    poller_t *poller;
    // CONTROL socket aka the main server socket
    int control_fd;
    clients_t *clients;
    char buffer[BUFFER_SIZE];
} ftp_t;

// Clients data
client_data_t *client_data_init(int *fd);
void client_data_free(client_data_t *data);

// Clients
clients_t *clients_init(void);
void clients_adder(clients_t *clients, int *fd);
void clients_delete(clients_t *clients, int i);

// Sockets
int socket_init(in_port_t port);

// Handler
void client_quit(ftp_t *ftp, unsigned int *i);
void poll_handler(ftp_t *ftp);

// FTP
// Returns false in case of an error
bool my_ftp(args_t *args);
void ftp_free(ftp_t *ftp);

#endif
