/*
** EPITECH PROJECT, 2026
** my_ftp
** File description:
** noop.c
*/

#include "my_ftp.h"
#include "status.h"
#include <stdbool.h>

void command_noop(ftp_t *ftp, unsigned int *i)
{
    WRITE_STATUS(ftp->poller->fds[*i].fd, 200);
}
