#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>

#include <SDL.h>
#include <SDL_ttf.h>
#include <string.h>

#include "SDL_events.h"
#include "SDL_keycode.h"

#include "SDL_render.h"
#include "la.h"

#include "buffer.h"

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

Vec2f buffer_pos;
Line line = {0};

size_t cursor_row = 0;
size_t cursor_col = 0;


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

    for (size_t x = 0; x < text_size; x++)
    {
        char c = text[x];

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
        .x = pos.x += floorf((float)cursor_col * FONT_WIDTH * FONT_SCALE),
        .y = pos.y += floorf((float)cursor_row * FONT_HEIGHT * FONT_SCALE)
    };

    const SDL_Rect cursor_rect = {
        .x = cursor_pos.x,
        .y = cursor_pos.y,
        .w = FONT_WIDTH * FONT_SCALE,
        .h = FONT_HEIGHT * FONT_SCALE,
    };
    
    scc(SDL_SetRenderDrawColor(renderer, cursor_color.r, cursor_color.g, cursor_color.b, cursor_color.a));
    scc(SDL_RenderFillRect(renderer, &cursor_rect));

    if (cursor_col < line.size)
    {
        char str[1];
        str[0] = line.chars[cursor_col];
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

    line_insert_text_before(&line, event.text.text, cursor_col);

    cursor_col++;
}

void handle_keydown(SDL_Event event)
{
    switch(event.key.keysym.sym) {
        case SDLK_BACKSPACE:
            if (line_erase_before(&line, cursor_col) > 0)
                cursor_col--;
        break;
        case SDLK_DELETE:
            line_erase_after(&line, cursor_col);
        break;
        case SDLK_LEFT:
            if (cursor_col > 0) {
                cursor_col -= 1;
            }
        break;
        case SDLK_RIGHT:
            if (cursor_col < line.size) {
                cursor_col += 1;
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

    line = new_line();

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

        if (line.size > 0)
        {
            render_text_sized(line.chars, line.size, buffer_pos, font, white, FONT_SCALE);
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
