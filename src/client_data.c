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
    if (data->data_fd != -1)
        close(data->data_fd);
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
