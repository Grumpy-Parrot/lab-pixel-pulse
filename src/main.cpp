#include "Platform/Platform.h"
#include "Platform/Std.h"
#include <SDL3/SDL.h>

#define UNUSED(x) (void)(x)

struct AppState {
    SDL_Window* window;
    SDL_Renderer* renderer;
    bool shouldQuit;

    AppState() : window(nullptr), renderer(nullptr), shouldQuit(false) {}
};

static AppState* appState = nullptr;

static void on_update() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_EVENT_QUIT) {
            appState->shouldQuit = true;
        }
        else if (event.type == SDL_EVENT_KEY_DOWN && event.key.key == SDLK_ESCAPE) {
            appState->shouldQuit = true;
        }
    }

    SDL_SetRenderDrawColor(appState->renderer, 124, 0, 0, 255);
    SDL_RenderClear(appState->renderer);

    SDL_RenderPresent(appState->renderer);
}

int main(int argc, char* argv[]) {
    UNUSED(argc);
    UNUSED(argv);

    if (!SDL_Init(SDL_INIT_VIDEO)) {
        std::cerr << "Failed to initialize SDL: " << SDL_GetError() << std::endl;
        return 1;
    }

    appState = new AppState();

    SDL_CreateWindowAndRenderer("Game", 800, 600, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE, &appState->window, &appState->renderer);

    if (!appState->window) {
        std::cerr << "Failed to create window: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    if (!appState->renderer) {
        std::cerr << "Failed to create renderer: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(appState->window);
        SDL_Quit();
        return 1;
    }

    const char* rendererName = SDL_GetRendererName(appState->renderer);
    std::cout << "Renderer: " << rendererName << std::endl;

    #ifdef PLATFORM_WASM
    emscripten_set_main_loop(on_update, 0, true);
    #else
    while (!appState->shouldQuit) {
        on_update();
    }
    #endif

    SDL_DestroyRenderer(appState->renderer);
    SDL_DestroyWindow(appState->window);
    SDL_Quit();

    delete appState;
    return 0;
}
