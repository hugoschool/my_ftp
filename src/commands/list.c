/*
** EPITECH PROJECT, 2026
** my_ftp
** File description:
** list.c
*/

#include "my_ftp.h"
#include "status.h"
#include <dirent.h>
#include <linux/limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>

static bool list_all_files(int fd, const char *initial_path, char *path)
{
    char full_path[PATH_MAX];
    struct dirent *entry = NULL;
    DIR *dir = NULL;

    strncpy(full_path, initial_path, PATH_MAX);
    strcat(full_path, path);
    dir = opendir(full_path);
    if (dir == NULL)
        return false;
    entry = readdir(dir);
    while (entry != NULL) {
        dprintf(fd, "%s\n", entry->d_name);
        entry = readdir(dir);
    }
    closedir(dir);
    return true;
}

static void close_and_error(ftp_t *ftp, unsigned int *i, int fd)
{
    close_data_socket(ftp, i, fd);
    WRITE_STATUS(*CLIENT->fd, 426);
    exit(1);
}

static void child_process(ftp_t *ftp, unsigned int *i)
{
    int fd = get_data_socket(ftp, i);

    if (fd == -1) {
        perror("accept");
        close_and_error(ftp, i, fd);
    }
    printf("File descriptor is %d\n", fd);
    if (!list_all_files(fd, ftp->initial_path, CLIENT->path))
        close_and_error(ftp, i, fd);
    WRITE_STATUS(*CLIENT->fd, 226);
    close_data_socket(ftp, i, fd);
    exit(0);
}

// No parent_process or whole server waits for a response.
static void fork_process(ftp_t *ftp, unsigned int *i)
{
    pid_t pid = fork();

    if (pid == 0) {
        child_process(ftp, i);
    }
}

// TODO: [pathname]
void command_list(ftp_t *ftp, unsigned int *i)
{
    if (CLIENT->login_step != LOGGED_IN) {
        WRITE_STATUS(*CLIENT->fd, 530_out);
        return;
    }
    if (CLIENT->data_mode == NO_MODE) {
        WRITE_STATUS(*CLIENT->fd, 425);
        return;
    }
    WRITE_STATUS(*CLIENT->fd, 150);
    fork_process(ftp, i);
}
