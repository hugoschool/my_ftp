/*
** EPITECH PROJECT, 2026
** my_ftp
** File description:
** cdup.c
*/

#include "my_ftp.h"
#include "status.h"
#include <stdbool.h>

void command_cdup(ftp_t *ftp, unsigned int *i)
{
    if (CLIENT->login_step != LOGGED_IN) {
        WRITE_STATUS(*CLIENT->fd, 530_out);
        return;
    }
    client_data_change_path(CLIENT, NULL, ftp->initial_path);
    WRITE_STATUS(ftp->poller->fds[*i].fd, 250);
}
