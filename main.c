#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <SDL.h>
#include <SDL_ttf.h>

#include "SDL_events.h"
#include "SDL_keycode.h"
#include "la.h"

#define FONT_WIDTH 12
#define FONT_HEIGHT 20
#define FONT_SIZE 12

SDL_Window *window;
SDL_Renderer *renderer;

SDL_Color white;
SDL_Color black;
SDL_Color blue;
SDL_Color red;
SDL_Color green;

#define BUFFER_CAPACITY 1024
char buffer[BUFFER_CAPACITY];
size_t buffer_size = 0;

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
        render_character(text[x], pos, font, tex_buffer, color, scale);
        pos.x += FONT_WIDTH * scale;
    }

    SDL_DestroyTexture(tex_buffer);
}

void render_text(const char *text, Vec2f pos, TTF_Font *font, SDL_Color color, float scale)
{
    render_text_sized(text, strlen(text), pos, font, color, scale);
}

void handle_textinput(SDL_Event event)
{
    if (event.key.repeat)
    {
        // don't handle repeats right now.
        return;
    }

    size_t text_size = strlen(event.text.text);
    const size_t free_space = BUFFER_CAPACITY - buffer_size;

    if (text_size > free_space) {
        text_size = free_space;
    }

    memcpy(buffer + buffer_size, event.text.text, text_size);
    buffer_size += text_size;
}

void handle_keydown(SDL_Event event)
{
    switch(event.key.keysym.sym) {
        case SDLK_BACKSPACE:
        if (buffer_size > 0) {
            buffer_size -= 1; 
        }
    }
}

void clear_screen(void)
{
    scc(SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0));
    scc(SDL_RenderClear(renderer));
}

int main()
{
    scc(SDL_Init(SDL_INIT_VIDEO));

    window = scp(SDL_CreateWindow("ked", 0, 0, 800, 600, SDL_WINDOW_RESIZABLE));
    renderer = scp(SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED));

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
            render_text_sized(buffer, buffer_size, vec2f(10.0, 10.0), font, green, 2.0f);
        }

        SDL_RenderPresent(renderer);
    }


    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();

    SDL_Quit();
    return 0;
}
