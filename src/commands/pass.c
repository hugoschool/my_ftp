/*
** EPITECH PROJECT, 2026
** my_ftp
** File description:
** pass.c
*/

#include "my_ftp.h"
#include "status.h"
#include <unistd.h>

void command_pass(ftp_t *ftp, unsigned int *i)
{
    login_step_t step = ftp->clients->clients[*i]->login_step;

    if (step == LOGGED_IN) {
        WRITE_STATUS(ftp->poller->fds[*i].fd, 230);
        return;
    }
    if (step == LOGGED_OUT) {
        WRITE_STATUS(ftp->poller->fds[*i].fd, 331);
        return;
    }
    ftp->clients->clients[*i]->login_step = LOGGED_IN;
    WRITE_STATUS(ftp->poller->fds[*i].fd, 230);
}
