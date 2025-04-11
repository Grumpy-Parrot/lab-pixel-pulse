#include "Platform/Platform.h"
#include "Platform/Std.h"
#include <SDL3/SDL.h>

#define UNUSED(x) (void)(x)

static bool shouldQuit = false;
static SDL_Window* window = nullptr;
static SDL_Renderer* renderer = nullptr;

static void on_update() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_EVENT_QUIT) {
            shouldQuit = true;
        }
        else if (event.type == SDL_EVENT_KEY_DOWN && event.key.key == SDLK_ESCAPE) {
            shouldQuit = true;
        }
    }

    SDL_SetRenderDrawColor(renderer, 124, 0, 0, 255);
    SDL_RenderClear(renderer);

    SDL_RenderPresent(renderer);
}

int main(int argc, char* argv[]) {
    UNUSED(argc);
    UNUSED(argv);

    if (!SDL_Init(SDL_INIT_VIDEO)) {
        std::cerr << "Failed to initialize SDL: " << SDL_GetError() << std::endl;
        return 1;
    }

    SDL_CreateWindowAndRenderer("Game", 800, 600, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE, &window, &renderer);

    if (!window) {
        std::cerr << "Failed to create window: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    if (!renderer) {
        std::cerr << "Failed to create renderer: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    const char* rendererName = SDL_GetRendererName(renderer);
    std::cout << "Renderer: " << rendererName << std::endl;

    #ifdef PLATFORM_WASM
    emscripten_set_main_loop(on_update, 0, true);
    #else
    while (!shouldQuit) {
        on_update();
        SDL_Delay(16);
    }
    #endif

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
