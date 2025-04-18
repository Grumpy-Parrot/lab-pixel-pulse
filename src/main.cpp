#include "Platform/Platform.h"
#include "Platform/Std.h"
#include "Platform/Memory.h"
#include "Logger.h"
#define PIXELPULSE_LIBRARIES_SHOULD_IMPLEMENT
#include "Libraries/Libraries.h"

#include "Assets/AssetRegistry.h"
#include "Game/RenderPassDescriptor.h"
#include "Math/Vector2.h"
#include "Game/Input.h"
#include "Game/Scene.h"
#include "Game/SceneNode.h"
#include "Game/Events/UpdateEventPayload.h"
#include "Game/Events/AttachEventPayload.h"
#include "Physics/PhysicsWorld.h"

namespace PixelPulse
{
    class Application
    {
    private:
        bool m_shouldQuit;
        std::uint64_t m_currentTime;
        std::uint64_t m_previousTime;
        float m_deltaTime;

        ::SDL_Window *m_window;
        ::SDL_Renderer *m_renderer;
        Assets::AssetRegistry *m_assetRegistry;
        Math::Vector2<std::int32_t> m_windowSize;

        Game::Scene *m_scene;
        Game::Input m_input;
        Physics::PhysicsWorld *m_physicsWorld;

        static void handleEvents(Application *app)
        {
            SDL_Event event;
            while (SDL_PollEvent(&event))
            {
                if (event.type == SDL_EVENT_QUIT)
                {
                    app->m_shouldQuit = true;
                }
                else if (event.type == SDL_EVENT_KEY_DOWN && event.key.key == SDLK_ESCAPE)
                {
                    app->m_shouldQuit = true;
                }
                else if (event.type == SDL_EVENT_WINDOW_RESIZED)
                {
                    SDL_GetWindowSize(app->m_window, &app->m_windowSize.x, &app->m_windowSize.y);
                    Logger::info("Window resized to: %d, %d", app->m_windowSize.x, app->m_windowSize.y);
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
        Application() : m_shouldQuit(false),
                        m_currentTime(0),
                        m_previousTime(0),
                        m_deltaTime(0.0f),
                        m_window(nullptr),
                        m_renderer(nullptr),
                        m_scene(nullptr),
                        m_physicsWorld(nullptr)
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
#ifdef PIXELPULSE_DEBUG
            Logger::setLevel(Logger::Level::Debug);
#else
            Logger::setLevel(Logger::Level::Info);
#endif

            if (!SDL_Init(SDL_INIT_VIDEO))
            {
                Logger::error("Failed to initialize SDL: %s", SDL_GetError());
                return false;
            }

            m_currentTime = m_previousTime = SDL_GetTicks();

            if (!SDL_CreateWindowAndRenderer(title, width, height,
                                             SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE, &m_window, &m_renderer))
            {
                Logger::error("Failed to create window or renderer: %s", SDL_GetError());
                cleanup();
                return false;
            }

            SDL_GetWindowSize(m_window, &m_windowSize.x, &m_windowSize.y);
            Logger::info("Window size: %d, %d", m_windowSize.x, m_windowSize.y);
            SDL_SetWindowMinimumSize(m_window, 640, 480);

            if (!m_window || !m_renderer)
            {
                Logger::error("Window or renderer is null");
                cleanup();
                return false;
            }

            m_assetRegistry = PP_NEW(Assets::AssetRegistry);

            // Initialize physics world
            m_physicsWorld = PP_NEW(Physics::PhysicsWorld);

            // Create scene graph
            m_scene = PP_NEW(Game::Scene);
            m_scene->setRenderer(m_renderer);
            m_scene->setAssetRegistry(m_assetRegistry);
            m_scene->setPhysicsWorld(m_physicsWorld);

            // Load scene from JSON
            if (!m_scene->loadFromJSON("assets/scenes/demo.json"))
            {
                Logger::error("Failed to load scene from JSON");
                cleanup();
                return false;
            }
            m_scene->start();

            return true;
        }

        void update()
        {
            handleEvents(this);

            // Timing
            m_previousTime = m_currentTime;
            m_currentTime = SDL_GetTicks();
            m_deltaTime = static_cast<float>(m_currentTime - m_previousTime) / 1000.0f;

            // Subsystem updates
            m_input.update(m_deltaTime);

            // Update physics world
            m_physicsWorld->update(m_deltaTime);

            Game::Events::UpdateEventPayload updateEventPayload;
            updateEventPayload.deltaTime = m_deltaTime;
            updateEventPayload.input = &m_input;

            // Update scene graph starting from the root
            m_scene->update(updateEventPayload);

            // Render
            SDL_SetRenderDrawColor(m_renderer, 100, 149, 237, 255); // Cornflower blue background
            SDL_RenderClear(m_renderer);

            Game::RenderPassDescriptor renderPassDescriptor;
            renderPassDescriptor.windowSize = m_windowSize;

            // Render scene graph
            m_scene->render(renderPassDescriptor);

            SDL_RenderPresent(m_renderer);
        }

        void run()
        {
            const char *rendererName = SDL_GetRendererName(m_renderer);
            Logger::info("Renderer: %s", rendererName);

#ifdef PLATFORM_WASM
            emscripten_set_main_loop(updateCallback, 0, true);
#else
            while (!m_shouldQuit)
            {
                update();
            }
#endif
        }

        void cleanup()
        {
            Logger::info("Cleaning up application...");

            if (m_scene)
            {
                Logger::info("Cleaning up scene");
                PP_DELETE(m_scene);
                m_scene = nullptr;
            }

            if (m_physicsWorld)
            {
                Logger::info("Cleaning up physics world");
                PP_DELETE(m_physicsWorld);
                m_physicsWorld = nullptr;
            }

            if (m_assetRegistry)
            {
                Logger::info("Cleaning up asset registry");
                m_assetRegistry->flushUnloadQueue();
                m_assetRegistry->flushActiveQueue();
                PP_DELETE(m_assetRegistry);
                m_assetRegistry = nullptr;
            }

            if (m_window)
            {
                Logger::info("Cleaning up window");
                SDL_DestroyWindow(m_window);
                m_window = nullptr;
            }

            if (m_renderer)
            {
                Logger::info("Cleaning up renderer");
                SDL_DestroyRenderer(m_renderer);
                m_renderer = nullptr;
            }

            SDL_Quit();
        }
    };
}

#ifdef PLATFORM_WASM
PixelPulse::Application *PixelPulse::Application::instance = nullptr;
#endif

int main(int argc, char *argv[])
{
    PIXELPULSE_ARG_UNUSED(argc);
    PIXELPULSE_ARG_UNUSED(argv);

    PP_MemorySystemInitialize();

    PixelPulse::Application *app = PP_NEW(PixelPulse::Application);
    if (!app->initialize("Game", 800, 600))
    {
        return 1;
    }

    app->run();
    PP_DELETE(app);

    PP_MemorySystemShutdown();

    return 0;
}
