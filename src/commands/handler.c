/*
** EPITECH PROJECT, 2026
** my_ftp
** File description:
** handler.c
*/

#include "commands.h"
#include "my_ftp.h"
#include "utils.h"

void commands_handler(ftp_t *ftp, unsigned int *i)
{
    if (strcmp_end(ftp->buffer, CRLF) != 0)
        return;
    for (int x = 0; cmds[x].end == false; i++) {
        if (strcmp_start(ftp->buffer, cmds[x].command) == 0) {
            cmds[x].function(ftp, i);
            break;
        }
    }
}
