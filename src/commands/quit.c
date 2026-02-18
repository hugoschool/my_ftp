/*
** EPITECH PROJECT, 2026
** my_ftp
** File description:
** quit.c
*/

#include "my_ftp.h"
#include <stdbool.h>

void command_quit(ftp_t *ftp, unsigned int *i)
{
    client_quit(ftp, i, true);
}
