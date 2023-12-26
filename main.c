#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <SDL.h>
#include <SDL_ttf.h>

TTF_Font *font;
int FONT_SIZE = 12;

SDL_Window *window;
SDL_Renderer *renderer;

SDL_Color white;
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
    white.r = 255;
    white.b = 255;
    white.g = 255;
    white.a = 0;
}

void init_fonts(void)
{
   scc(TTF_Init());
   font = scp(TTF_OpenFont("fonts/ProggyVector-Regular.ttf", FONT_SIZE));
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

SDL_Texture *get_text_texture(char *text)
{
    SDL_Surface *surface;

    surface = scp(TTF_RenderUTF8_Blended(font, text, white));

    return surface_to_texture(surface, 1);
}

int main()
{
    scc(SDL_Init(SDL_INIT_VIDEO));

    window = scp(SDL_CreateWindow("ked", 0, 0, 800, 600, SDL_WINDOW_RESIZABLE));
    renderer = scp(SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED));

    init_colors();
    init_fonts();

    bool quit = false;

    while (!quit) {
        SDL_Event event = {0};
        while (SDL_PollEvent(&event)) {
            switch(event.type) {
                case SDL_QUIT:
                    quit = true;
                    break;
            }
        }

        scc(SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0));
        scc(SDL_RenderClear(renderer));

        SDL_Texture *text_texture = get_text_texture("hello world!");

        SDL_Rect rect;
        rect.x = 0;
        rect.y = 0;
        rect.w = 100;
        rect.h = 12;

        scc(SDL_RenderCopy(renderer, text_texture, NULL, &rect));

        SDL_RenderPresent(renderer);
    }


    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();

    SDL_Quit();
    return 0;
}
