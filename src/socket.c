/*
** EPITECH PROJECT, 2026
** my_ftp
** File description:
** socket.c
*/

#include <netinet/in.h>
#include <stdio.h>
#include <unistd.h>

static int socket_set_options(int socket_fd, struct sockaddr_in *addr)
{
    const int enable = 1;

    if (setsockopt(socket_fd, SOL_SOCKET,
            SO_REUSEADDR, &enable, sizeof(int)) < 0) {
        perror("setsockopt");
        return -1;
    }
    if (bind(socket_fd, (struct sockaddr *)addr, sizeof(*addr)) < 0) {
        perror("bind");
        close(socket_fd);
        return -1;
    }
    if (listen(socket_fd, 10) < 0) {
        perror("listen");
        return -1;
    }
    return 1;
}

int socket_init(in_port_t port)
{
    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in server_addr = {.sin_family = AF_INET,
        .sin_port = htons(port), .sin_addr = {.s_addr = INADDR_ANY}};

    if (socket_fd < 0) {
        perror("socket");
        return -1;
    }
    if (socket_set_options(socket_fd, &server_addr) == -1)
        return -1;
    return socket_fd;
}
