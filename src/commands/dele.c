/*
** EPITECH PROJECT, 2026
** my_ftp
** File description:
** dele.c
*/

#include "my_ftp.h"
#include "status.h"
#include "utils.h"
#include <linux/limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>

static bool verify_path(ftp_t *ftp, unsigned int *i, const char *pathname,
    char full_path[])
{
    strncpy(full_path, ftp->initial_path, PATH_MAX);
    strcat(full_path, CLIENT->path);
    strcat(full_path, pathname);
    return access(full_path, F_OK) == 0;
}

static void valid_pathname_handle(ftp_t *ftp, unsigned int *i, char *pathname)
{
    char full_path[PATH_MAX];

    pathname = pathname + 1;
    remove_crlf(pathname);
    if (!verify_path(ftp, i, pathname, full_path)) {
        WRITE_STATUS(*CLIENT->fd, 550);
        return;
    }
    if (remove(full_path) == -1) {
        perror("remove");
        WRITE_STATUS(*CLIENT->fd, 550);
        return;
    }
    WRITE_STATUS(*CLIENT->fd, 250);
}

void command_dele(ftp_t *ftp, unsigned int *i)
{
    char *pathname = NULL;

    if (CLIENT->login_step != LOGGED_IN) {
        WRITE_STATUS(*CLIENT->fd, 530_out);
        return;
    }
    pathname = strchr(ftp->buffer, ' ');
    if (pathname)
        valid_pathname_handle(ftp, i, pathname);
    else
        WRITE_STATUS(*CLIENT->fd, 501);
}
