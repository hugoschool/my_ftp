/*
** EPITECH PROJECT, 2026
** my_ftp
** File description:
** client_data.c
*/

#include "my_ftp.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

client_data_t *client_data_init(int *fd)
{
    client_data_t *data = malloc(sizeof(client_data_t));

    if (data == NULL) {
        perror("malloc");
        exit(84);
    }
    data->fd = fd;
    data->path = strdup("/");
    if (data->path == NULL) {
        perror("strdup");
        exit(84);
    }
    data->data_fd = -1;
    data->login_step = LOGGED_OUT;
    data->data_mode = NO_MODE;
    return data;
}

// data->fd should not be freed here, it's the poller's responsibility
// to free it.
void client_data_free(client_data_t *data)
{
    if (data == NULL)
        return;
    if (data->path)
        free(data->path);
    client_data_close_data_socket(data);
    free(data);
    data = NULL;
}

void client_data_change_path(client_data_t *data, const char *full_path,
    const char *initial_path)
{
    if (data->path)
        free(data->path);
    if (full_path == NULL)
        data->path = strdup("/");
    else
        data->path = strdup(full_path + strlen(initial_path));
    if (data->path == NULL) {
        perror("strdup");
        exit(84);
    }
}

// This function shouldn't really be used as is unless needed.
// It's probably better to use `close_data_socket` instead.
void client_data_close_data_socket(client_data_t *data)
{
    data->data_mode = NO_MODE;
    if (data->data_fd != -1)
        close(data->data_fd);
    data->data_fd = -1;
}
