#include <string.h>
#include <stdlib.h>
#include <assert.h>

#include "buffer.h"

#define LINE_INIT_CAPACITY 32
#define LINE_CAPACITY_CHUNK 64

Line new_line()
{
    Line line = (Line) {
        .size = 0,
        .capacity = 0,
    };

    return line;
}
// grow the capacity of the line by at least n characters.
static void line_grow(Line *line, size_t n)
{
    size_t new_capacity = line->capacity == 0? LINE_INIT_CAPACITY : line->capacity;

    assert(new_capacity >= line->size);

    while (new_capacity - line->size < n)
    {
        new_capacity += LINE_CAPACITY_CHUNK;
    }

    if (new_capacity != line->capacity) {
        line->chars = realloc(line->chars, new_capacity);
    }
}

int line_insert_text_before(Line *line, const char *text, size_t col)
{
    const size_t text_size = strlen(text);
    const size_t free_space = line->capacity - line->size;

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

// int buffer_delete_line_before(Buffer *buffer, size_t row)
// {
//     if (row == 0)
//         return 0;
//
//     free(buffer->lines[row-1]->chars);
//         
//     return 1;
// }
//
// void buffer_add_line_before(Buffer *buffer, size_t row)
// {
//     if (row == 0)
//         return;
//
//     
// }
//
// void buffer_add_line_after(Buffer *buffer, size_t row)
// {
//
// }
