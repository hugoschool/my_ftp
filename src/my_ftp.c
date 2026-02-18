/*
** EPITECH PROJECT, 2026
** my_ftp
** File description:
** my_ftp.c
*/

#include "my_ftp.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/poll.h>
#include <unistd.h>

static ftp_t *ftp_init(void)
{
    ftp_t *ftp = malloc(sizeof(ftp_t));

    if (ftp == NULL) {
        perror("malloc");
        exit(84);
    }
    ftp->poller = poller_init();
    ftp->control_fd = -1;
    return ftp;
}

void ftp_free(ftp_t *ftp)
{
    poller_free(ftp->poller);
    if (ftp->control_fd != -1)
        close(ftp->control_fd);
    free(ftp);
}

void ftp_loop(ftp_t *ftp)
{
    int result = 0;

    while (true) {
        result = poll(ftp->poller->fds, ftp->poller->amount, -1);
        if (result == -1) {
            perror("poll");
            break;
        }
        poll_handler(ftp);
    }
}

bool my_ftp(args_t *args)
{
    ftp_t *ftp = ftp_init();

    ftp->control_fd = socket_init(args->port);
    if (ftp->control_fd == -1) {
        ftp_free(ftp);
        return false;
    }
    poller_set_init_socket(ftp->poller, ftp->control_fd);
    ftp_loop(ftp);
    ftp_free(ftp);
    return true;
}
