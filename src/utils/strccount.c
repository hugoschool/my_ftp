/*
** EPITECH PROJECT, 2026
** my_ftp
** File description:
** Count the number of times the char appears in the string.
*/

#include <stddef.h>

size_t strccount(const char *str, char c)
{
    size_t count = 0;

    if (str == NULL)
        return 0;
    for (size_t i = 0; str[i] != 0; i++) {
        if (c == str[i])
            count++;
    }
    return count;
}
