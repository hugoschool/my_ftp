/*
** EPITECH PROJECT, 2026
** my_ftp
** File description:
** user.c
*/

#include "commands.h"
#include "my_ftp.h"
#include "status.h"
#include <string.h>
#include <sys/poll.h>
#include <unistd.h>

void command_user(ftp_t *ftp, unsigned int *i)
{
    login_step_t step = CLIENT->login_step;
    const char *username = NULL;

    if (step == LOGGED_IN) {
        WRITE_STATUS(ftp->poller->fds[*i].fd, 230);
        return;
    }
    username = strchr(ftp->buffer, ' ');
    if (username && strncmp(username + 1,
            USER_ANONYMOUS, strlen(USER_ANONYMOUS)) == 0) {
        CLIENT->login_step = CORRECT_USERNAME;
    } else {
        CLIENT->login_step = INCORRECT_USERNAME;
    }
    WRITE_STATUS(ftp->poller->fds[*i].fd, 331);
    return;
}
