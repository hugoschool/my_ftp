/*
** EPITECH PROJECT, 2026
** my_ftp
** File description:
** utils.h
*/

#ifndef UTILS_H_
    #define UTILS_H_

    #include <stddef.h>

int strcmp_start(const char *str, const char *start);
int strcmp_end(const char *str, const char *end);
void remove_crlf(char *str);
size_t strccount(const char *str, char c);

#endif
