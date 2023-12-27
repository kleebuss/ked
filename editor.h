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

static int line_grow(Line *line, size_t n);

typedef struct {
	Vec2f pos;
	size_t capacity;
	size_t size;
	Line *lines;
	size_t cursor_row;
	size_t cursor_col;
} Editor;

Editor new_editor();

void editor_insert_text_before_cursor(Editor *editor, const char *text);
int editor_erase_before_cursor(Editor *editor);
int editor_erase_after_cursor(Editor *editor);
void editor_insert_new_line(Editor *editor);

static int editor_grow(Editor *editor, size_t n);
