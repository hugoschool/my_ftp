/*
** EPITECH PROJECT, 2026
** my_ftp
** File description:
** cwd.c
*/

#include "my_ftp.h"
#include "status.h"
#include "utils.h"
#include <linux/limits.h>
#include <stdbool.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>

// TODO: .. handling
static void full_appended_path(char new[], const char *initial,
    const char *client, const char *given)
{
    strcpy(new, initial);
    if (strlen(given) > 0 && given[0] == '/') {
        strcat(new, given);
    } else {
        if (strcmp(client, "/") != 0)
            strcat(new, client);
        strcat(new, "/");
        strcat(new, given);
    }
}

static bool verify_path(ftp_t *ftp, char full_path[], const char *client_path,
    const char *path)
{
    DIR *dir = NULL;

    full_appended_path(full_path, ftp->initial_path, client_path, path);
    dir = opendir(full_path);
    if (dir == NULL)
        return false;
    closedir(dir);
    return true;
}

static void get_path(char *buffer, char **path)
{
    *path = strchr(buffer, ' ');
    if (*path == NULL)
        return;
    *path = *path + 1;
    remove_crlf(*path);
}

void command_cwd(ftp_t *ftp, unsigned int *i)
{
    char *path = NULL;
    char full_path[PATH_MAX];

    if (CLIENT->login_step != LOGGED_IN) {
        WRITE_STATUS(*CLIENT->fd, 530_out);
        return;
    }
    get_path(ftp->buffer, &path);
    if (path == NULL) {
        WRITE_STATUS(*CLIENT->fd, 501);
        return;
    }
    if (!verify_path(ftp, full_path, CLIENT->path, path)) {
        WRITE_STATUS(*CLIENT->fd, 550);
        return;
    }
    client_data_change_path(CLIENT, full_path, ftp->initial_path);
    WRITE_STATUS(*CLIENT->fd, 250);
}
