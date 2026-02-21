/*
** EPITECH PROJECT, 2026
** my_ftp
** File description:
** help.c
*/

#include "commands.h"
#include "my_ftp.h"
#include "status.h"
#include <stdbool.h>
#include <stdio.h>

// TODO: handle per command help
void command_help(ftp_t *ftp, unsigned int *i)
{
    WRITE_STATUS(*CLIENT->fd, 214_all);
    for (int cmd_i = 0; cmds[cmd_i].end == false; cmd_i++) {
        dprintf(*CLIENT->fd, "  %s"CRLF, cmds[cmd_i].command);
    }
}
