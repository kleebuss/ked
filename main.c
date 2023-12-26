#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <SDL.h>
#include <SDL_ttf.h>
#include <string.h>

#include "SDL_events.h"
#include "SDL_keycode.h"

#include "SDL_render.h"
#include "la.h"

#define FONT_WIDTH 12
#define FONT_HEIGHT 20
#define FONT_SIZE 14
#define FONT_SCALE 2

SDL_Window *window;
SDL_Renderer *renderer;

SDL_Color white;
SDL_Color black;
SDL_Color blue;
SDL_Color red;
SDL_Color green;

#define BUFFER_CAPACITY 1024
char buffer[BUFFER_CAPACITY];
size_t buffer_cursor = 0;
size_t buffer_size = 0;
Vec2f buffer_pos;

void buffer_erase_before_cursor(void) {
    if (buffer_size == 0 || buffer_cursor <= 0) {
        return;
    }

    char *dest_addr = buffer + buffer_cursor - 1;
    size_t len_to_move = buffer_size - buffer_cursor;

    memmove(dest_addr, buffer + buffer_cursor, len_to_move);
    buffer_size -= 1;
    buffer_cursor -= 1;
}

void buffer_erase_after_cursor(void) {
    if (buffer_size == 0 || buffer_cursor == buffer_size) {
        return;
    }

    char *dest_addr = buffer + buffer_cursor;
    size_t len_to_move = buffer_size - buffer_cursor - 1;

    memmove(dest_addr, buffer + buffer_cursor + 1, len_to_move);
    buffer_size -= 1;
}

void buffer_insert_text_before_cursor(const char *text)
{
    size_t text_size = strlen(text);
    const size_t free_space = BUFFER_CAPACITY - buffer_size;

    if (text_size > free_space) {
        text_size = free_space;
    }

    // copy everything right of the cursor further to the right.
    char *dest_addr = buffer + text_size + buffer_cursor;
    
    memmove(dest_addr, buffer + buffer_cursor, buffer_size - buffer_cursor);

    // copy the input string into place and update the cursor.
    memcpy(buffer + buffer_cursor, text, text_size);
    buffer_size += text_size;
    buffer_cursor += text_size;
}

// check SDL return code and panic if negative.
void scc(int code)
{
    if (code < 0)
    {
        fprintf(stderr, "SDL Error: %s\n", SDL_GetError());
        exit(1);
    }
}

// check SDL pointer and panic if NULL.
void *scp(void *ptr)
{
    if (ptr == NULL)
    {
        fprintf(stderr, "SDL Error: %s\n", SDL_GetError());
        exit(1);
    }

    return ptr;
}

void init_colors(void)
{
    white = (SDL_Color) {
        .r = 255,
        .g = 255,
        .b = 255,
        .a = 0
    };

    black = (SDL_Color) {
        .r = 0,
        .g = 0,
        .b = 0,
        .a = 0
    };

    red = (SDL_Color) {
        .r = 255,
        .g = 0,
        .b = 0,
        .a = 0
    };

    green = (SDL_Color) {
        .r = 0,
        .g = 255,
        .b = 0,
        .a = 0
    };

    blue = (SDL_Color) {
        .r = 0,
        .g = 0,
        .b = 255,
        .a = 0
    };
}

TTF_Font *load_font(const char* path)
{
   scc(TTF_Init());
   return scp(TTF_OpenFont(path, FONT_SIZE));
}

SDL_Texture *surface_to_texture(SDL_Surface *surface, int destroy_surface)
{
    SDL_Texture *texture;

    texture = scp(SDL_CreateTextureFromSurface(renderer, surface));

    if (destroy_surface)
    {
        SDL_FreeSurface(surface);
    }

    return texture;
}

SDL_Texture *get_text_texture(char c, TTF_Font *font, SDL_Color color)
{
    SDL_Surface *surface;

    surface = scp(TTF_RenderUTF8_Blended(font, &c, color));

    return surface_to_texture(surface, 1);
}

void render_character(char c, Vec2f pos, TTF_Font *font, SDL_Texture *tex_buffer, SDL_Color color, float scale)
{
    tex_buffer = get_text_texture(c, font, color);

    SDL_Rect rect;
    rect.x = pos.x;
    rect.y = pos.y;
    rect.w = FONT_WIDTH * scale;
    rect.h = FONT_HEIGHT * scale;

    scc(SDL_RenderCopy(renderer, tex_buffer, NULL, &rect));
}

void render_text_sized(const char *text, size_t text_size, Vec2f pos, TTF_Font *font, SDL_Color color, float scale)
{
    SDL_Texture *tex_buffer = {0};
    Vec2f startpos = pos;

    for (size_t x = 0; x < text_size; x++)
    {
        char c = text[x];

        if (c == '\n')
        {
            pos.x = startpos.x;
            pos.y += FONT_HEIGHT * scale;
        }

        render_character(c, pos, font, tex_buffer, color, scale);
        pos.x += FONT_WIDTH * scale;
    }

    SDL_DestroyTexture(tex_buffer);
}

void render_text(const char *text, Vec2f pos, TTF_Font *font, SDL_Color color, float scale)
{
    render_text_sized(text, strlen(text), pos, font, color, scale);
}

void render_cursor(Vec2f pos, SDL_Color cursor_color, SDL_Color font_color, TTF_Font *font)
{
    Vec2f cursor_pos = {
        .x = pos.x += (int) floorf(buffer_cursor * FONT_WIDTH * FONT_SCALE),
        .y = pos.y,
    };

    const SDL_Rect cursor_rect = {
        .x = cursor_pos.x,
        .y = cursor_pos.y,
        .w = FONT_WIDTH * FONT_SCALE,
        .h = FONT_HEIGHT * FONT_SCALE,
    };
    
    scc(SDL_SetRenderDrawColor(renderer, cursor_color.r, cursor_color.g, cursor_color.b, cursor_color.a));
    scc(SDL_RenderFillRect(renderer, &cursor_rect));

    if (buffer_cursor < buffer_size)
    {
        char str[1];
        str[0] = buffer[buffer_cursor];
        render_text_sized(str, 1, cursor_pos, font, font_color, FONT_SCALE);
    }
}

void handle_textinput(SDL_Event event)
{
    if (event.key.repeat)
    {
        // don't handle repeats right now.
        return;
    }

    buffer_insert_text_before_cursor(event.text.text);
}

void handle_keydown(SDL_Event event)
{
    switch(event.key.keysym.sym) {
        case SDLK_BACKSPACE:
            buffer_erase_before_cursor();
        break;
        case SDLK_DELETE:
            buffer_erase_after_cursor();
        break;
        case SDLK_LEFT:
            if (buffer_cursor > 0) {
                buffer_cursor -= 1;
            }
        break;
        case SDLK_RIGHT:
            if (buffer_cursor < buffer_size) {
                buffer_cursor += 1;
            }
        break;
        default:
        break;
    }
}

void clear_screen(void)
{
    scc(SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0));
    scc(SDL_RenderClear(renderer));
}

int main2(void)
{
    buffer_insert_text_before_cursor("Hello, world.");
    buffer_cursor = 6;
    buffer_erase_before_cursor();
    return 0;
}

int main(void)
{
    scc(SDL_Init(SDL_INIT_VIDEO));

    window = scp(SDL_CreateWindow("ked", 0, 0, 800, 600, SDL_WINDOW_RESIZABLE));
    renderer = scp(SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED));

    buffer_pos = (Vec2f) {
        .x = 10,
        .y = 10
    };

    init_colors();
    TTF_Font *font = load_font("fonts/ProggyVector-Regular.ttf");

    bool quit = false;

    while (!quit) {
        SDL_Event event = {0};
        while (SDL_PollEvent(&event)) {
            switch(event.type) {
                case SDL_QUIT:
                    quit = true;
                    break;
                case SDL_TEXTINPUT: 
                    handle_textinput(event);
                    break;
                case SDL_KEYDOWN: 
                    handle_keydown(event);
                    break;
            }
        }

        clear_screen();

        if (buffer_size > 0)
        {
            render_text_sized(buffer, buffer_size, buffer_pos, font, white, FONT_SCALE);
        }

        render_cursor(buffer_pos, white, black, font);

        SDL_RenderPresent(renderer);
    }


    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();

    SDL_Quit();
    return 0;
}
