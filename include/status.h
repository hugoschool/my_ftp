/*
** EPITECH PROJECT, 2026
** my_ftp
** File description:
** status.h
*/

#ifndef STATUS_H_
    #define STATUS_H_

    #include <string.h>
    #include <unistd.h>

    #ifndef CRLF
        #define CRLF "\r\n"
    #endif

    #define GET_STATUS(c) status._##c
    #define WRITE_STATUS(fd, c) write(fd, status._##c, strlen(status._##c))

typedef struct {
    const char *_150;
    const char *_200;
    const char *_214_all;
    const char *_220;
    const char *_221;
    const char *_226;
    const char *_227;
    const char *_230;
    const char *_250;
    const char *_331;
    const char *_421;
    const char *_425;
    const char *_426;
    const char *_500;
    const char *_501;
    const char *_530_out;
    const char *_530_incorrect;
    const char *_550;
} status_codes_t;

static const status_codes_t status = {
    ._150 = "150 File status okay; about to open data connection."CRLF,
    ._200 = "200 Command okay."CRLF,
    ._214_all = "214 Here's a list of available commands:"CRLF,
    ._220 = "220 Service ready for new user."CRLF,
    ._221 = "221 Service closing control connection."CRLF,
    ._226 = "226 Closing data connection."CRLF,
    ._227 = "227 Entering Passive Mode (%s,%d,%d)"CRLF,
    ._230 = "230 User logged in, proceed."CRLF,
    ._250 = "250 Requested file action okay, completed."CRLF,
    ._331 = "331 User name okay, need password."CRLF,
    ._421 = "421 Service not available, closing control connection."CRLF,
    ._425 = "425 Can't open data connection."CRLF,
    ._426 = "426 Connection closed."CRLF,
    ._500 = "500 Unknown command."CRLF,
    ._501 = "501 Incorrect arguments."CRLF,
    ._530_out = "530 Please login with USER and PASS."CRLF,
    ._530_incorrect = "530 Login incorrect."CRLF,
    ._550 = "550 Requested action not taken."CRLF,
};

#endif
