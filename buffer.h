#pragma once
#include <stddef.h>

#include "la.h"

typedef struct {
	size_t capacity;
	size_t size;
    char *chars;
} Line;

Line new_line();


int line_insert_text_before(Line *line, const char *text, size_t col);
int line_erase_before(Line *line, size_t col);
int line_erase_after(Line *line, size_t col);

static void line_grow(Line *line, size_t n);

typedef struct {
	Vec2f pos;
	Line *lines[];
} Buffer;

int buffer_delete_line_before(Buffer *buffer, size_t row);
void buffer_add_line_before(Buffer *buffer, size_t row);
void buffer_add_line_after(Buffer *buffer, size_t row);
