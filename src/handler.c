/*
** EPITECH PROJECT, 2026
** my_ftp
** File description:
** handler.c
*/

#include "my_ftp.h"
#include <stdio.h>
#include <sys/poll.h>
#include <arpa/inet.h>
#include <unistd.h>

// TODO: poller_fd_add(poller, accepted_fd);
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
    close(cfd);
}

// TODO: client_handler(ftp, i);
static void handle_pollin_events(ftp_t *ftp, unsigned int *i)
{
    if (ftp->poller->fds[*i].fd == ftp->control_fd) {
        new_client_handler(ftp);
    } else {
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
