/*
** EPITECH PROJECT, 2026
** my_ftp
** File description:
** handler.c
*/

#include "my_ftp.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/poll.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

void new_client_handler(ftp_t *ftp)
{
    struct sockaddr_in caddr;
    socklen_t caddrl = sizeof(caddr);
    int cfd = accept(ftp->control_fd, (struct sockaddr *) &caddr, &caddrl);

    if (cfd == -1) {
        perror("accept");
        return;
    }
    printf("Connection from %s:%d\n",
        inet_ntoa(caddr.sin_addr),
        ntohs(caddr.sin_port));
    poller_fd_add(ftp->poller, cfd);
    clients_adder(ftp->clients, &ftp->poller->fds[ftp->poller->amount].fd);
}

void client_quit(ftp_t *ftp, unsigned int *i)
{
    int fd = ftp->poller->fds[*i].fd;
    struct sockaddr_in addr;
    socklen_t addrl = sizeof(addr);

    if (fd != ftp->control_fd) {
        getpeername(fd, (struct sockaddr *)&addr, &addrl);
        printf("%s:%d left\n",
            inet_ntoa(addr.sin_addr),
            ntohs(addr.sin_port));
        if (close(fd) == -1)
            perror("close");
        poller_fd_delete(ftp->poller, *i);
        clients_delete(ftp->clients, *i);
        i--;
    }
}

void client_handler(ftp_t *ftp, unsigned int *i)
{
    size_t temp_size = 256;
    char buffer[temp_size];
    int fd = ftp->poller->fds[*i].fd;
    int bytes_read = read(fd, buffer, temp_size);
    const char *str = "Hello world!\n";

    if (bytes_read <= 0) {
        client_quit(ftp, i);
    } else {
        buffer[bytes_read] = 0;
        if (write(fd, str, strlen(str)) == -1) {
            perror("write");
            exit(84);
        }
    }
}

static void handle_pollin_events(ftp_t *ftp, unsigned int *i)
{
    if (ftp->poller->fds[*i].fd == ftp->control_fd) {
        new_client_handler(ftp);
    } else {
        client_handler(ftp, i);
    }
}

void poll_handler(ftp_t *ftp)
{
    unsigned int amount = ftp->poller->amount;

    for (unsigned int i = 0; i < amount; i++) {
        if (ftp->poller->fds[i].revents & POLLIN) {
            handle_pollin_events(ftp, &i);
        }
    }
}
