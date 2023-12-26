#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <SDL.h>

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
int main()
{
    scc(SDL_Init(SDL_INIT_VIDEO));

    SDL_Window *window;
    SDL_Renderer *renderer;

    window = scp(SDL_CreateWindow("ked", 0, 0, 800, 600, SDL_WINDOW_RESIZABLE));
    renderer = scp(SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED));

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
        SDL_RenderPresent(renderer);
    }


    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    SDL_Quit();
    return 0;
}
