/*
** EPITECH PROJECT, 2026
** my_ftp
** File description:
** args.h
*/

#ifndef ARGS_H_
    #define ARGS_H_

    #include <stdbool.h>

typedef struct {
    const int port;
    const char *path;
    bool help;
    // Must be true when the args are valid (ex: --help)
    bool valid;
} args_t;

args_t parse_args(int argc, char **argv);

#endif
