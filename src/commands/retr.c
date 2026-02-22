/*
** EPITECH PROJECT, 2026
** my_ftp
** File description:
** retr.c
*/

#include "my_ftp.h"
#include "status.h"
#include "utils.h"
#include <dirent.h>
#include <linux/limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

static bool close_file_and_return(bool a, int fd)
{
    if (fd != -1)
        close(fd);
    return a;
}

static bool send_file(const char *pathname, int fd)
{
    char *content = NULL;
    struct stat s;
    int file_fd = open(pathname, O_RDONLY);

    if (file_fd == -1)
        return false;
    if (fstat(file_fd, &s) == -1)
        return close_file_and_return(false, file_fd);
    content = malloc(sizeof(char) * s.st_size + 1);
    if (content == NULL)
        return close_file_and_return(false, file_fd);
    read(file_fd, content, s.st_size);
    content[s.st_size] = '\0';
    write(fd, content, s.st_size);
    free(content);
    return close_file_and_return(true, file_fd);
}

static void close_and_error(ftp_t *ftp, unsigned int *i, int fd)
{
    close_data_socket(ftp, i, fd);
    WRITE_STATUS(*CLIENT->fd, 426);
    exit(1);
}

static void child_process(ftp_t *ftp, unsigned int *i, const char *pathname)
{
    int fd = get_data_socket(ftp, i);

    if (fd == -1) {
        perror("accept");
        close_and_error(ftp, i, fd);
    }
    if (!send_file(pathname, fd))
        close_and_error(ftp, i, fd);
    WRITE_STATUS(*CLIENT->fd, 226);
    close_data_socket(ftp, i, fd);
    exit(0);
}

// No parent_process or whole server waits for a response.
static void fork_process(ftp_t *ftp, unsigned int *i, const char *pathname)
{
    pid_t pid = fork();

    if (pid == 0) {
        child_process(ftp, i, pathname);
    }
}

static bool verify_path(ftp_t *ftp, unsigned int *i, const char *pathname,
    char full_path[])
{
    strncpy(full_path, ftp->initial_path, PATH_MAX);
    strcat(full_path, CLIENT->path);
    strcat(full_path, pathname);
    return access(full_path, R_OK) == 0;
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
    WRITE_STATUS(*CLIENT->fd, 150);
    fork_process(ftp, i, full_path);
}

void command_retr(ftp_t *ftp, unsigned int *i)
{
    char *pathname = NULL;

    if (CLIENT->login_step != LOGGED_IN) {
        WRITE_STATUS(*CLIENT->fd, 530_out);
        return;
    }
    if (CLIENT->data_mode == NO_MODE) {
        WRITE_STATUS(*CLIENT->fd, 425);
        return;
    }
    pathname = strchr(ftp->buffer, ' ');
    if (pathname)
        valid_pathname_handle(ftp, i, pathname);
    else
        WRITE_STATUS(*CLIENT->fd, 501);
}
