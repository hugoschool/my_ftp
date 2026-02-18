/*
** EPITECH PROJECT, 2026
** my_ftp
** File description:
** status.h
*/

#ifndef STATUS_H_
    #define STATUS_H_

    #include <string.h>

    #ifndef CRLF
        #define CRLF "\r\n"
    #endif

    #define WRITE_STATUS(fd, c) write(fd, status._##c, strlen(status._##c))

typedef struct {
    char *_220;
    char *_230;
    char *_331;
} status_codes_t;

static const status_codes_t status = {
    ._220 = "220 Service ready for new user."CRLF,
    ._230 = "230 User logged in, proceed."CRLF,
    ._331 = "331 User name okay, need password."CRLF,
};

#endif
