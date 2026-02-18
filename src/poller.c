/*
** EPITECH PROJECT, 2026
** my_ftp
** File description:
** poller.c
*/

#include "my_ftp.h"
#include <stdio.h>
#include <stdlib.h>
#include <poll.h>

poller_t *poller_init(void)
{
    poller_t *poller = malloc(sizeof(poller_t));

    if (poller == NULL) {
        perror("malloc");
        exit(84);
    }
    poller->fds = calloc(POLLER_INIT_SIZE, sizeof(struct pollfd));
    if (poller->fds == NULL) {
        perror("calloc");
        exit(84);
    }
    poller->fds[0].fd = -1;
    poller->fds[0].events = POLLIN;
    poller->fds[0].revents = 0;
    poller->amount = 1;
    poller->size = POLLER_INIT_SIZE;
    return poller;
}

void poller_free(poller_t *poller)
{
    free(poller->fds);
    free(poller);
}

void poller_set_init_socket(poller_t *poller, int socket_fd)
{
    poller->fds[0].fd = socket_fd;
}
