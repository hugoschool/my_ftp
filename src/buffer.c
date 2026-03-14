/*
** EPITECH PROJECT, 2026
** my_ftp
** File description:
** buffer.c
*/

#include "my_ftp.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

buffer_t *buffer_init(void)
{
    buffer_t *buffer = malloc(sizeof(buffer_t));

    if (buffer == NULL) {
        perror("malloc");
        exit(84);
    }
    buffer->buffer = malloc(sizeof(char) * BUFFER_SIZE);
    if (buffer->buffer == NULL) {
        perror("malloc");
        exit(84);
    }
    buffer->amount = 0;
    buffer->size = BUFFER_SIZE;
    buffer->previous = NULL;
    return buffer;
}

void buffer_free(buffer_t *buffer)
{
    free(buffer->buffer);
    free(buffer);
}

void buffer_append(buffer_t *buffer, char *str, size_t len)
{
    size_t new_amount = buffer->amount + len;

    if (buffer->size <= new_amount) {
        buffer->size = buffer->size * 2;
        buffer->buffer = realloc(buffer->buffer, sizeof(char) * buffer->size);
        if (buffer->buffer == NULL) {
            perror("realloc");
            exit(84);
        }
    }
    if (buffer->amount == 0)
        strncpy(buffer->buffer, str, len);
    else
        strncat(buffer->buffer, str, len);
    buffer->amount = new_amount;
    buffer->buffer[buffer->amount] = 0;
}

void buffer_clear(buffer_t *buffer)
{
    memset(buffer->buffer, 0, buffer->amount);
    buffer->previous = NULL;
}

static void allocate_ftp_buffer(buffer_t *buffer, ftp_t *ftp, size_t size)
{
    ftp->buffer = malloc(sizeof(char) * (size + 1));
    if (ftp->buffer == NULL) {
        perror("malloc");
        exit(84);
    }
    strncpy(ftp->buffer, buffer->previous, size);
    ftp->buffer[size] = 0;
}

// True if command was found
bool buffer_set_current_command(buffer_t *buffer, ftp_t *ftp)
{
    char *ptr = NULL;
    size_t size = 0;

    if (buffer == NULL)
        return false;
    if (buffer->previous == NULL)
        buffer->previous = buffer->buffer;
    ptr = strstr(buffer->previous, CRLF);
    if (ptr == NULL)
        return false;
    if (ftp->buffer != NULL)
        free(ftp->buffer);
    ptr = ptr + strlen(CRLF);
    size = ptr - buffer->previous;
    allocate_ftp_buffer(buffer, ftp, size);
    buffer->previous = ptr;
    return true;
}
