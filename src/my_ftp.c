/*
** EPITECH PROJECT, 2026
** my_ftp
** File description:
** my_ftp.c
*/

#include "my_ftp.h"
#include <stdio.h>
#include <stdlib.h>

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
    free(ftp);
}

void my_ftp(args_t *args)
{
    ftp_t *ftp = ftp_init();

    (void)args;
    ftp_free(ftp);
    return;
}
