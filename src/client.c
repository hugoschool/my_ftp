/*
** EPITECH PROJECT, 2026
** my_ftp
** File description:
** client.c
*/

#include "my_ftp.h"
#include <stdio.h>
#include <stdlib.h>

// Initial amount is 1 and NULL because we consider that the first
// client is not a client but the server socket
clients_t *clients_init(void)
{
    clients_t *clients = malloc(sizeof(clients_t));

    if (clients == NULL) {
        perror("malloc");
        exit(84);
    }
    clients->clients = calloc(CLIENTS_INIT_SIZE, sizeof(client_data_t));
    if (clients->clients == NULL) {
        perror("calloc");
        exit(84);
    }
    clients->amount = 1;
    clients->size = CLIENTS_INIT_SIZE;
    return clients;
}

void clients_adder(clients_t *clients, int *fd)
{
    if (clients->amount == clients->size) {
        clients->size += 1;
        clients->clients =
            realloc(clients->clients, sizeof(client_data_t) * (clients->size));
        if (clients->clients == NULL) {
            perror("realloc");
            exit(84);
        }
    }
    clients->clients[clients->amount] = client_data_init(fd);
    clients->amount++;
}

void clients_delete(clients_t *clients, int i)
{
    if (clients->clients[i] == NULL)
        return;
    client_data_free(clients->clients[i]);
    clients->clients[i] = clients->clients[clients->amount - 1];
    clients->amount--;
}
