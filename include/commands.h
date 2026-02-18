/*
** EPITECH PROJECT, 2026
** my_ftp
** File description:
** commands.h
*/

#ifndef COMMANDS_H_
    #define COMMANDS_H_

    #include "my_ftp.h"
    #include <unistd.h>

    #define USER_ANONYMOUS "Anonymous"

void command_quit(ftp_t *ftp, unsigned int *i);
void command_user(ftp_t *ftp, unsigned int *i);

typedef struct {
    const char *command;
    void (*function)(ftp_t *ftp, unsigned int *i);
    bool end;
} commands_t;

static const commands_t cmds[] = {
    {.command = "QUIT", .function = &command_quit, .end = false},
    {.command = "USER", .function = &command_user, .end = false},
    {.command = "", .function = NULL, .end = true}
};

void commands_handler(ftp_t *ftp, unsigned int *i);

#endif
