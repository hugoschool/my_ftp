/*
** EPITECH PROJECT, 2026
** my_ftp
** File description:
** stor.c
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
    char read_buffer[BUFFER_SIZE];
    ssize_t bytes_read = -1;
    int file_fd = open(pathname, O_RDWR | O_CREAT, 0644);

    if (file_fd == -1)
        return false;
    bytes_read = read(fd, read_buffer, BUFFER_SIZE);
    while (bytes_read > 0) {
        read_buffer[bytes_read] = '\0';
        write(file_fd, read_buffer, bytes_read);
        bytes_read = read(fd, read_buffer, BUFFER_SIZE);
    }
    close(file_fd);
    return true;
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
static void fork_process(ftp_t *ftp, unsigned int *i, char full_path[])
{
    pid_t pid = fork();

    if (pid == 0) {
        child_process(ftp, i, full_path);
    }
}

static bool verify_path(ftp_t *ftp, unsigned int *i, char full_path[],
    char *pathname)
{
    DIR *dir = NULL;

    strncpy(full_path, ftp->initial_path, PATH_MAX);
    strcat(full_path, CLIENT->path);
    dir = opendir(full_path);
    if (dir == NULL)
        return false;
    closedir(dir);
    strcat(full_path, pathname);
    return true;
}

static void valid_pathname_handle(ftp_t *ftp, unsigned int *i, char *pathname)
{
    char full_path[PATH_MAX];

    pathname = pathname + 1;
    remove_crlf(pathname);
    if (!verify_path(ftp, i, full_path, pathname)) {
        WRITE_STATUS(*CLIENT->fd, 550);
        return;
    }
    WRITE_STATUS(*CLIENT->fd, 150);
    fork_process(ftp, i, full_path);
}

void command_stor(ftp_t *ftp, unsigned int *i)
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
