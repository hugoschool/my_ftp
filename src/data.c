/*
** EPITECH PROJECT, 2026
** my_ftp
** File description:
** data.c
*/

#include "my_ftp.h"
#include <unistd.h>

int get_data_socket(ftp_t *ftp, unsigned int *i)
{
    struct sockaddr_in caddr;
    socklen_t caddrl = sizeof(caddr);
    int fd = 0;

    if (CLIENT->data_mode == PASSIVE)
        fd = accept(CLIENT->data_fd, (struct sockaddr *)&caddr, &caddrl);
    else
        fd = CLIENT->data_fd;
    return fd;
}

// A wrapper to close "accept"ed file descriptors
// Set fd to -1 for it to not be closed
void close_data_socket(ftp_t *ftp, unsigned int *i, int fd)
{
    if (CLIENT->data_mode == PASSIVE) {
        if (fd != -1)
            close(fd);
        client_data_close_data_socket(CLIENT);
    } else if (CLIENT->data_mode == ACTIVE) {
        client_data_close_data_socket(CLIENT);
    }
}
