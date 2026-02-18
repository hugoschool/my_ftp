/*
** EPITECH PROJECT, 2026
** my_ftp
** File description:
** strcmp_start.c
*/

#include <string.h>

int strcmp_start(const char *str, const char *start)
{
    return strncmp(str, start, strlen(start));
}
