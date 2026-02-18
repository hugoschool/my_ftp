/*
** EPITECH PROJECT, 2026
** my_ftp
** File description:
** strcmp_end.c
*/

#include <string.h>
#include <unistd.h>

int strcmp_end(const char *str, const char *end)
{
    size_t str_n = 0;
    size_t end_n = 0;

    if (str == NULL || end == NULL)
        return 0;
    str_n = strlen(str);
    end_n = strlen(end);
    if (end_n > str_n)
        return 0;
    return strncmp(str + str_n - end_n, end, end_n);
}
