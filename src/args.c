/*
** EPITECH PROJECT, 2026
** my_ftp
** File description:
** args.c
*/

#include "args.h"
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>

static void parse_help(void)
{
    printf("USAGE: ./myftp port path\n"
        "  port is the port number on which the server socket listens\n"
        "  path is the path to the home directory for the Anonymous user\n");
}

static const char *parse_path(const char *path)
{
    if (access(path, R_OK) == -1) {
        return NULL;
    }
    return path;
}

args_t parse_args(int argc, char **argv)
{
    int port = -1;
    const char *path = NULL;

    if (argc == 2 && (strcmp(argv[1], "--help") == 0
            || strcmp(argv[1], "-h") == 0)) {
        parse_help();
        return (args_t){
            .port = -1, .path = NULL, .help = true, .valid = true,
        };
    }
    if (argc == 3) {
        port = atoi(argv[1]);
        path = parse_path(argv[2]);
        return (args_t){
            .port = port, .path = path,
            .help = false, .valid = path ? true : false,
        };
    }
    return (args_t){.valid = false};
}
