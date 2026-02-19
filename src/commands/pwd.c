/*
** EPITECH PROJECT, 2026
** my_ftp
** File description:
** pwd.c
*/

#include "my_ftp.h"
#include "status.h"
#include <stdbool.h>
#include <stdio.h>

void command_pwd(ftp_t *ftp, unsigned int *i)
{
    if (CLIENT->login_step != LOGGED_IN) {
        WRITE_STATUS(*CLIENT->fd, 530_out);
        return;
    }
    dprintf(*CLIENT->fd,
        "257 \"%s\" is the current directory"CRLF,
        CLIENT->path);
}
