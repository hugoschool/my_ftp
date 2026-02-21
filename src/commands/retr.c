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

static bool send_file(const char *pathname, int fd)
{
    char *content = NULL;
    struct stat s;
    int file_fd = open(pathname, O_RDONLY);

    if (file_fd == -1)
        return false;
    if (fstat(file_fd, &s) == -1)
        return false;
    content = malloc(sizeof(char) * s.st_size + 1);
    if (content == NULL)
        return false;
    read(file_fd, content, s.st_size);
    content[s.st_size] = '\0';
    write(fd, content, s.st_size);
    free(content);
    return true;
}

static void close_and_error(client_data_t *data)
{
    client_data_close_data_socket(data);
    WRITE_STATUS(*data->fd, 426);
    exit(1);
}

static void child_process(ftp_t *ftp, unsigned int *i, const char *pathname)
{
    struct sockaddr_in caddr;
    socklen_t caddrl = sizeof(caddr);
    int cfd = accept(CLIENT->data_fd, (struct sockaddr *)&caddr, &caddrl);

    if (cfd == -1) {
        perror("accept");
        close_and_error(CLIENT);
    }
    if (!send_file(pathname, cfd))
        close_and_error(CLIENT);
    WRITE_STATUS(*CLIENT->fd, 226);
    client_data_close_data_socket(CLIENT);
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
    if (CLIENT->data_fd == -1) {
        WRITE_STATUS(*CLIENT->fd, 425);
        return;
    }
    pathname = strchr(ftp->buffer, ' ');
    if (pathname)
        valid_pathname_handle(ftp, i, pathname);
    else
        WRITE_STATUS(*CLIENT->fd, 501);
}
