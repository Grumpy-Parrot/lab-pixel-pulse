#include "Platform/Platform.h"
#include "Platform/Std.h"
#include "Logger.h"
#define GP_LIBRARIES_SHOULD_IMPLEMENT
#include "Libraries/Libraries.h"

#include "Assets/AssetRegistry.h"
#include "Assets/Image.h"
#include "Game/Sprite.h"
#include "Math/Vector2.h"
#include "RenderPassDescriptor.h"
#include "Game/Input.h"
#include "Game/UpdateEventPayload.h"

#define UNUSED(x) (void)(x)

class Application
{
private:
    SDL_Window *window;
    SDL_Renderer *renderer;
    bool shouldQuit;
    AssetRegistry *assetRegistry;
    Vector2<std::int32_t> windowSize;

    std::uint64_t currentTime;
    std::uint64_t previousTime;
    float deltaTime;

    // TODO: Replace with scene graph
    std::vector<Sprite *> sprites;
    Input input;

    static void handleEvents(Application *app)
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_EVENT_QUIT)
            {
                app->shouldQuit = true;
            }
            else if (event.type == SDL_EVENT_KEY_DOWN && event.key.key == SDLK_ESCAPE)
            {
                app->shouldQuit = true;
            }
            else if (event.type == SDL_EVENT_WINDOW_RESIZED)
            {
                SDL_GetWindowSize(app->window, &app->windowSize.x, &app->windowSize.y);
                Logger::info("Window resized to: %d, %d", app->windowSize.x, app->windowSize.y);
            }
        }
    }

#ifdef PLATFORM_WASM
    static void updateCallback()
    {
        if (instance)
        {
            instance->update();
        }
    }

    static Application *instance;
#endif

public:
    Application() : window(nullptr), renderer(nullptr), shouldQuit(false),
                 currentTime(0), previousTime(0), deltaTime(0.0f)
    {
#ifdef PLATFORM_WASM
        instance = this;
#endif
    }

    ~Application()
    {
        cleanup();
    }

    bool initialize(const char *title, int width, int height)
    {
        if (!SDL_Init(SDL_INIT_VIDEO))
        {
            Logger::error("Failed to initialize SDL: %s", SDL_GetError());
            return false;
        }

        currentTime = previousTime = SDL_GetTicks();

        if (!SDL_CreateWindowAndRenderer(title, width, height,
                                         SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE, &window, &renderer))
        {
            Logger::error("Failed to create window or renderer: %s", SDL_GetError());
            cleanup();
            return false;
        }

        SDL_GetWindowSize(window, &windowSize.x, &windowSize.y);
        Logger::info("Window size: %d, %d", windowSize.x, windowSize.y);
        SDL_SetWindowMinimumSize(window, 640, 480);

        if (!window || !renderer)
        {
            Logger::error("Window or renderer is null");
            cleanup();
            return false;
        }

        assetRegistry = new AssetRegistry();

        Image *vampireImage = assetRegistry->make<Image>(AssetMakeRequest{"assets/vampire.png"});

        assetRegistry->loadAllAssets();

        Sprite *vampireSprite = new Sprite(vampireImage);
        if (!vampireSprite->init(renderer))
        {
            Logger::error("Failed to initialize sprite");
            cleanup();
            return false;
        }

        vampireSprite->move(Vector2<float>(0, 0));
        vampireSprite->scale(Vector2<float>(0.2f, 0.2f));

        sprites.push_back(vampireSprite);

        return true;
    }

    void update()
    {
        handleEvents(this);

        // Timing
        previousTime = currentTime;
        currentTime = SDL_GetTicks();
        deltaTime = (currentTime - previousTime) / 1000.0f;

        // Subsystem updates
        input.update(deltaTime);

        UpdateEventPayload updateEventPayload;
        updateEventPayload.deltaTime = deltaTime;
        updateEventPayload.input = &input;

        // Render
        SDL_SetRenderDrawColor(renderer, 124, 0, 0, 255);
        SDL_RenderClear(renderer);

        RenderPassDescriptor renderPassDescriptor;
        renderPassDescriptor.windowSize = windowSize;

        Vector2<float> origin;
        origin.x = 0;
        origin.y = 0;

        for (Sprite *sprite : sprites)
        {
            sprite->onUpdate(updateEventPayload);
            sprite->render(renderer, &renderPassDescriptor);
        }

        SDL_RenderPresent(renderer);
    }

    void run()
    {
        const char *rendererName = SDL_GetRendererName(renderer);
        Logger::info("Renderer: %s", rendererName);

#ifdef PLATFORM_WASM
        emscripten_set_main_loop(updateCallback, 0, true);
#else
        while (!shouldQuit)
        {
            update();
        }
#endif
    }

    void cleanup()
    {
        for (Sprite *sprite : sprites)
        {
            delete sprite;
        }
        sprites.clear();

        assetRegistry->unloadAllAssets();
        delete assetRegistry;

        if (renderer)
        {
            SDL_DestroyRenderer(renderer);
            renderer = nullptr;
        }

        if (window)
        {
            SDL_DestroyWindow(window);
            window = nullptr;
        }

        SDL_Quit();
    }
};

#ifdef PLATFORM_WASM
Application *Application::instance = nullptr;
#endif

int main(int argc, char *argv[])
{
    UNUSED(argc);
    UNUSED(argv);

    Application app;
    if (!app.initialize("Game", 800, 600))
    {
        return 1;
    }

    app.run();
    return 0;
}
