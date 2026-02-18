/*
** EPITECH PROJECT, 2026
** my_ftp
** File description:
** my_ftp.c
*/

#include "my_ftp.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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
    struct sockaddr_in addr;
    socklen_t addrl = sizeof(addr);
    int cfd = -1;
    const char *str = "Hello world!\n";

    while (true) {
        cfd = accept(ftp->control_fd, (struct sockaddr *) &addr, &addrl);
        if (cfd < 0) {
            perror("accept");
            break;
        }
        write(cfd, str, strlen(str));
        close(cfd);
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
    ftp_loop(ftp);
    ftp_free(ftp);
    return true;
}
