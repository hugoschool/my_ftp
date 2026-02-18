/*
** EPITECH PROJECT, 2026
** my_ftp
** File description:
** handler.c
*/

#include "commands.h"
#include "my_ftp.h"
#include "status.h"
#include <stdio.h>
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
    int *cfdr = NULL;

    if (cfd == -1) {
        perror("accept");
        return;
    }
    printf("Connection from %s:%d\n",
        inet_ntoa(caddr.sin_addr),
        ntohs(caddr.sin_port));
    poller_fd_add(ftp->poller, cfd);
    cfdr = &ftp->poller->fds[ftp->poller->amount - 1].fd;
    clients_adder(ftp->clients, cfdr);
    WRITE_STATUS(*cfdr, 220);
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

// TODO: buffer should also be cleared here
// TODO: make big circular buffer xd
void client_handler(ftp_t *ftp, unsigned int *i)
{
    int fd = ftp->poller->fds[*i].fd;
    int bytes_read = read(fd, ftp->buffer, BUFFER_SIZE);

    if (bytes_read <= 0) {
        client_quit(ftp, i);
    } else {
        ftp->buffer[bytes_read] = 0;
        commands_handler(ftp, i);
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
