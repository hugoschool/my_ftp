/*
** EPITECH PROJECT, 2026
** my_ftp
** File description:
** client_data.c
*/

#include "my_ftp.h"
#include <stdio.h>
#include <stdlib.h>

client_data_t *client_data_init(int *fd)
{
    client_data_t *data = malloc(sizeof(client_data_t));

    if (data == NULL) {
        perror("malloc");
        exit(84);
    }
    data->fd = fd;
    data->login_step = LOGGED_OUT;
    return data;
}

void client_data_free(client_data_t *data)
{
    if (data == NULL)
        return;
    free(data);
    data = NULL;
}
