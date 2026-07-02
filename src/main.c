#include <SDL2/SDL.h>
#include <stdbool.h>

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
#define CELL_SIZE 40
#define FPS 60

int main(int argc, char *argv[]) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL_Init failed: %s", SDL_GetError());
        return 1;
    }

    SDL_Window *window = SDL_CreateWindow(
        "Checkerboard Demo",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH,
        WINDOW_HEIGHT,
        SDL_WINDOW_SHOWN
    );

    if (!window) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL_CreateWindow failed: %s", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL_CreateRenderer failed: %s", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    bool running = true;
    Uint32 frame_time = 1000 / FPS;

    while (running) {
        Uint32 frame_start = SDL_GetTicks();

        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                    running = false;
                    break;
                case SDL_KEYDOWN:
                    if (event.key.keysym.sym == SDLK_ESCAPE) {
                        running = false;
                    }
                    break;
            }
        }

        Uint32 ticks = SDL_GetTicks();
        int shift = (ticks / 100) % (2 * CELL_SIZE);

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        for (int y = 0; y < WINDOW_HEIGHT; y += CELL_SIZE) {
            for (int x = 0; x < WINDOW_WIDTH; x += CELL_SIZE) {
                int pattern_x = (x + shift) / CELL_SIZE;
                int pattern_y = y / CELL_SIZE;
                bool is_white = (pattern_x + pattern_y) % 2 == 0;

                if (is_white) {
                    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
                } else {
                    SDL_SetRenderDrawColor(renderer, 50, 50, 50, 255);
                }

                SDL_Rect cell = {x, y, CELL_SIZE, CELL_SIZE};
                SDL_RenderFillRect(renderer, &cell);
            }
        }

        SDL_RenderPresent(renderer);

        Uint32 frame_elapsed = SDL_GetTicks() - frame_start;
        if (frame_elapsed < frame_time) {
            SDL_Delay(frame_time - frame_elapsed);
        }
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
