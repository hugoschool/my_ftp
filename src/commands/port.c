/*
** EPITECH PROJECT, 2026
** my_ftp
** File description:
** port.c
*/

#include "my_ftp.h"
#include "status.h"
#include "utils.h"
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>

// We assume that each given byte is valid
static char *decode_ip(const char *options)
{
    size_t start = 0;
    size_t current = 0;
    char *temp = NULL;
    char *ip = calloc(strlen(IP_TEMPLATE) + 1, sizeof(char));

    for (size_t i = 0; i < 4; i++) {
        for (current = start; options[current] != ','; current++);
        temp = calloc((current - start) + 1, sizeof(char));
        if (temp == NULL) {
            perror("malloc");
            exit(84);
        }
        strncpy(temp, &options[start], current - start);
        strcat(ip, temp);
        if (i != 3)
            strcat(ip, ".");
        free(temp);
        start = current + 1;
    }
    return ip;
}

// Get the p1, p2 combined into a single in_port_t
static in_port_t decode_port(const char *options)
{
    size_t i = 0;
    in_port_t port = 0;
    size_t start = 0;

    for (size_t skip_i = 0; skip_i < 4; skip_i++) {
        for (; options[i] != ','; i++);
        i++;
    }
    start = i;
    for (; options[i] != ','; i++);
    i++;
    port += atoi(&options[start]) * 256;
    port += atoi(&options[i]);
    return port;
}

static void create_data_socket(ftp_t *ftp, unsigned int *i,
    struct in_addr *addr, in_port_t port)
{
    struct sockaddr_in sockaddr;

    CLIENT->data_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (CLIENT->data_fd == -1) {
        perror("socket_init");
        WRITE_STATUS(*CLIENT->fd, 421);
        return;
    }
    sockaddr.sin_addr = *addr;
    sockaddr.sin_port = htons(port);
    sockaddr.sin_family = AF_INET;
    if (connect(CLIENT->data_fd, (struct sockaddr *)&sockaddr,
            sizeof(sockaddr)) == -1) {
        perror("connect");
        WRITE_STATUS(*CLIENT->fd, 421);
        return;
    }
    WRITE_STATUS(*CLIENT->fd, 200);
}

// A port command should be given an "option" like this (page 28 of RFC959):
// PORT h1,h2,h3,h4,p1,p2
static void port_handler(ftp_t *ftp, unsigned int *i, char *options)
{
    char *ip = NULL;
    in_port_t port = 0;
    struct in_addr addr;

    options = options + 1;
    remove_crlf(options);
    if (strccount(options, ',') != 5) {
        WRITE_STATUS(*CLIENT->fd, 501);
        return;
    }
    ip = decode_ip(options);
    port = decode_port(options);
    inet_aton(ip, &addr);
    free(ip);
    create_data_socket(ftp, i, &addr, port);
}

void command_port(ftp_t *ftp, unsigned int *i)
{
    char *options = NULL;

    if (CLIENT->login_step != LOGGED_IN) {
        WRITE_STATUS(*CLIENT->fd, 530_out);
        return;
    }
    options = strchr(ftp->buffer, ' ');
    if (options)
        port_handler(ftp, i, options);
    else
        WRITE_STATUS(*CLIENT->fd, 501);
}
