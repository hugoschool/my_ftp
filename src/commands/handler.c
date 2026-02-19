/*
** EPITECH PROJECT, 2026
** my_ftp
** File description:
** handler.c
*/

#include "commands.h"
#include "my_ftp.h"
#include "status.h"
#include "utils.h"

void commands_handler(ftp_t *ftp, unsigned int *i)
{
    if (strcmp_end(ftp->buffer, CRLF) != 0)
        return;
    for (int cmd_i = 0; cmds[cmd_i].end == false; cmd_i++) {
        if (strcmp_start(ftp->buffer, cmds[cmd_i].command) == 0) {
            cmds[cmd_i].function(ftp, i);
            return;
        }
    }
    WRITE_STATUS(*CLIENT->fd, 500);
}
