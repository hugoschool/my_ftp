/*
** EPITECH PROJECT, 2026
** my_ftp
** File description:
** pasv.c
*/

#include "my_ftp.h"
#include "status.h"
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

static char *get_formatted_ip(char *ip)
{
    size_t len = strlen(ip);

    for (size_t i = 0; i < len; i++) {
        if (ip[i] == '.') {
            ip[i] = ',';
        }
    }
    return ip;
}

static void get_formatted_port(in_port_t *port, int *p1, int *p2)
{
    uint16_t net_port = ntohs(*port);

    *p1 = net_port / 256;
    *p2 = net_port - (*p1 * 256);
}

static void validate_pasv(client_data_t *data, struct in_addr *in,
    in_port_t port)
{
    char *ip = get_formatted_ip(inet_ntoa(*in));
    int p1 = -1;
    int p2 = -1;

    get_formatted_port(&port, &p1, &p2);
    data->data_mode = PASSIVE;
    dprintf(*data->fd, GET_STATUS(227), ip, p1, p2);
}

void command_pasv(ftp_t *ftp, unsigned int *i)
{
    struct sockaddr_in data;
    socklen_t datal = sizeof(data);

    if (CLIENT->login_step != LOGGED_IN) {
        WRITE_STATUS(*CLIENT->fd, 530_out);
        return;
    }
    if (CLIENT->data_fd != -1)
        close_data_socket(ftp, i, -1);
    CLIENT->data_fd = socket_init(NULL, 0);
    if (CLIENT->data_fd == -1) {
        WRITE_STATUS(*CLIENT->fd, 425);
        return;
    }
    if (getsockname(CLIENT->data_fd, (struct sockaddr *)&data, &datal) == -1) {
        WRITE_STATUS(*CLIENT->fd, 425);
        return;
    }
    return validate_pasv(CLIENT, &data.sin_addr, data.sin_port);
}
