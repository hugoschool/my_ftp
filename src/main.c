/*
** EPITECH PROJECT, 2026
** my_ftp
** File description:
** main.c
*/

#include "my_ftp.h"
#include <stdbool.h>
#include <stdio.h>

int main(int argc, char **argv)
{
    args_t args = parse_args(argc, argv);

    if (args.valid == false) {
        fprintf(stderr, "Invalid arguments\n");
        return 84;
    }
    if (args.help)
        return 0;
    if (!my_ftp(&args))
        return 84;
    return 0;
}
