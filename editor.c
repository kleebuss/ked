#include <string.h>
#include <stdlib.h>
#include <assert.h>

#include "editor.h"
#include "la.h"

#define LINE_INIT_CAPACITY 32
#define LINE_CAPACITY_CHUNK 64

#define EDITOR_INIT_CAPACITY 16

Line new_line()
{
    Line line = (Line) {
        .size = 0,
        .capacity = LINE_INIT_CAPACITY,
        .chars = malloc(LINE_INIT_CAPACITY)
    };

    return line;
}
// grow the capacity of line.chars if needed to support n additional chars.
static int line_grow(Line *line, size_t n)
{
    size_t capacity = line->capacity == 0? LINE_INIT_CAPACITY : line->capacity;

    size_t free_space = line->capacity - line->size;

    if (n < free_space)
        return 0;

    size_t delta = n - free_space;
    size_t new_size = capacity + delta;

    line->chars = realloc(line->chars, new_size);

    return delta;
}

int line_insert_text_before(Line *line, const char *text, size_t col)
{
    if (col > line->size)
    {
        return 0;
    }

    const size_t text_size = strlen(text);

    line_grow(line, text_size);

    // copy everything right of the cursor further to the right.
    char *dest_addr = line->chars + text_size + col;
    
    memmove(dest_addr, line->chars + col, line->size - col);

    // copy the input string into place and update the cursor.
    memcpy(line->chars + col, text, text_size);
    line->size += text_size;
    return text_size;
}

int line_erase_before(Line *line, size_t col)
{
    if (line->size == 0 || col <= 0) {
        return 0;
    }

    char *dest_addr = line->chars + col - 1;
    size_t len_to_move = line->size - col;

    memmove(dest_addr, line->chars + col, len_to_move);
    line->size -= 1;
    return 1;
}

int line_erase_after(Line *line, size_t col)
{
    if (line->size == 0 || col == line->size) {
        return 0;
    }

    char *dest_addr = line->chars + col;
    size_t len_to_move = line->size - col - 1;

    memmove(dest_addr, line->chars + col + 1, len_to_move);
    line->size -= 1;
    return 1;
}

// insert new lines, growing the lines array if needed.
// returns the number of new lines allocated.
static int editor_grow(Editor *editor, size_t n)
{
    size_t capacity = editor->capacity == 0? LINE_INIT_CAPACITY : editor->capacity;
    size_t free_space = editor->capacity - editor->size;

    if (n < free_space)
        return 0;

    size_t delta = n - free_space;
    size_t new_size = (capacity + delta) * sizeof(editor->lines[0]);

    editor->lines = realloc(editor->lines, new_size);

    return delta;
}

Editor new_editor()
{
    Line line = new_line();

    Editor editor = {
        .pos = vec2f(10.0, 10.0),
        .capacity = EDITOR_INIT_CAPACITY,
        .cursor_row = 0,
        .cursor_col = 0,
        .lines = malloc(EDITOR_INIT_CAPACITY * sizeof(line))
    };

    editor.lines[0] = line;

    return editor;
}

void editor_insert_text_before_cursor(Editor *editor, const char *text)
{
    if (editor->size == 0) {
        editor_insert_new_line(editor);
    }

    if (editor->cursor_row >= editor->size)
    {
        editor->cursor_row = editor->size -1;
    }

    Line *line = &editor->lines[editor->cursor_row];
    line_insert_text_before(line, text, editor->cursor_col);
    editor->cursor_col += strlen(text);
}

// returns the number of characters deleted
// if -1, the current line was deleted.
int editor_erase_before_cursor(Editor *editor)
{
    assert(editor->cursor_row < editor->size);
    assert(editor->cursor_row >= 0);

    if (editor->cursor_col == 0)
    {
        // We're at the beginning of a line.
        // Backspacing should delete the current line unless
        // we're on the first line.
        
        if (editor->cursor_row == 0)
        {
            // nothing to do!
            return 0;
        }

        // We're not on the first line, so we should delete this line and
        // append its contents to the previous line.

        Line *this_line = &editor->lines[editor->cursor_row];
        Line *prev_line = &editor->lines[editor->cursor_row - 1];

        line_insert_text_before(prev_line, this_line->chars, prev_line->size);
        editor->cursor_col--;
        editor->cursor_row = prev_line->size;
        return -1;
    }
    
    size_t n = line_erase_before(&editor->lines[editor->cursor_row], editor->cursor_col);
    editor->cursor_col -= n;
    return n;
}

int editor_erase_after_cursor(Editor *editor)
{
    assert(editor->cursor_row < editor->size);
    
    if (editor->cursor_col == editor->lines[editor->cursor_row].size)
    {
        // we're at the end of a line     
        // the delete key should move the contents of the next line to this one
        // unless there is no next line.
        
        if (editor->size == editor->cursor_row + 1)
        {
            // nothing to do!
            return 0;
        }

        // TODO make this work
        return 0;
    }
    
    size_t n = line_erase_after(&editor->lines[editor->cursor_row], editor->cursor_col);
    return n;
}

// int editor_delete_line(Editor *editor, size_t row)
// {
//
// }

void editor_insert_new_line(Editor *editor)
{
    if (editor->cursor_row >= editor->size)
    {
        editor->cursor_row = editor->size - 1;
    }

    editor_grow(editor, 1);

    if (editor->size > 0 && editor->cursor_row < editor->size) {
        size_t element_size = sizeof(editor->lines[0]);

        memmove(
            editor->lines + (editor->cursor_row + 1) * element_size,
            editor->lines + (editor->cursor_row + 2) * element_size,
            (editor->size - editor->cursor_row - 1) * element_size
        );

        memset(&editor->lines[editor->cursor_row], 0, element_size);
    }

    editor->lines[editor->cursor_row] = new_line();


    editor->cursor_row++;
    editor->size++;
}

// int editor_delete_line_before(Editor *editor, size_t row)
// {
//     if (row == 0)
//         return 0;
//
//     free(editor->lines[row-1]->chars);
//         
//     return 1;
// }
//
// void editor_add_line_before(Editor *editor, size_t row)
// {
//     if (row == 0)
//         return;
//
//     
// }
//
// void editor_add_line_after(Editor *editor, size_t row)
// {
//
// }
