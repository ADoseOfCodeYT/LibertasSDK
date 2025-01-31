// WickedEngineTests.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "sdl2.h"
#include "ImGui/imgui_impl_sdl.h"

int sdl_loop(Example_ImGui &tests)
{
    bool quit = false;
    while (!quit)
    {
        tests.Run();
        SDL_Event event;
        while(SDL_PollEvent(&event)){
            switch(event.type){
                case SDL_QUIT:
                    quit = true;
                    break;
                case SDL_WINDOWEVENT:
                    switch (event.window.event) {
                        case SDL_WINDOWEVENT_CLOSE: // exit tests
                            quit = true;
                            break;
                        case SDL_WINDOWEVENT_RESIZED:
                            // Tells the engine to reload window configuration (size and dpi)
                            tests.SetWindow(tests.window);
                            break;
                        case SDL_WINDOWEVENT_FOCUS_LOST: //TODO
                            tests.is_window_active = false;
                            break;
                        case SDL_WINDOWEVENT_FOCUS_GAINED:
                            tests.is_window_active = true;
                            if (lb::shadercompiler::GetRegisteredShaderCount() > 0)
                            {
                                std::thread([] {
                                    lb::console::Post("[Shader check] Started checking " + std::to_string(lb::shadercompiler::GetRegisteredShaderCount()) + " registered shaders for changes...");
                                    if (lb::shadercompiler::CheckRegisteredShadersOutdated())
                                    {
                                        lb::console::Post("[Shader check] Changes detected, initiating reload...");
                                        lb::eventhandler::Subscribe_Once(lb::eventhandler::EVENT_THREAD_SAFE_POINT, [](uint64_t userdata) {
                                            lb::renderer::ReloadShaders();
                                            });
                                    }
                                    else
                                    {
                                        lb::console::Post("[Shader check] All up to date");
                                    }
                                    }).detach();
                            }
                            break;
                        default:
                            break;
                    }
                default:
                    break;
            }
            lb::input::sdlinput::ProcessEvent(event);
            ImGui_ImplSDL2_ProcessEvent(&event);
        }
    }

    return 0;
}

int main(int argc, char *argv[])
{
    Example_ImGui exampleImGui;
    // TODO: Place code here.

    lb::arguments::Parse(argc, argv);

    sdl2::sdlsystem_ptr_t system = sdl2::make_sdlsystem(SDL_INIT_EVERYTHING | SDL_INIT_EVENTS);
    if (*system) {
        throw sdl2::SDLError("Error creating SDL2 system");
    }

    sdl2::window_ptr_t window = sdl2::make_window(
            "Libertas Engine Tests",
            SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
            1280, 800,
            SDL_WINDOW_SHOWN | SDL_WINDOW_VULKAN | SDL_WINDOW_RESIZABLE);
    if (!window) {
        throw sdl2::SDLError("Error creating window");
    }

    exampleImGui.SetWindow(window.get());

    int ret = sdl_loop(exampleImGui);

    SDL_Quit();
    return ret;
}
