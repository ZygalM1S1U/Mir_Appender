#include "application.h"
#include "imgui.h"
#include <GL/glew.h>
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"
#include <stdio.h>
#include <iostream>
#include <SDL.h>
#include <SDL_opengl.h>
#include "app_component.h"
#include "menu_config_component.h"
#include "addons/imguidock.h"
#include <string>

#define PIXEL_HEIGHT 720
#define PIXEL_WIDTH 1280
#define IM_ARRAYSIZE(_ARR)  ((int)(sizeof(_ARR)/sizeof(*_ARR)))

template<> Application* Singleton<Application>::aSingleton = 0;

Application::Application()
	: tick_1(0),
	tick_2(0),
	tick_4(0),
	tick_50ms(0),
	tick_5sec(0)
{
    menu = new MenuConfigComponent();
}

Application::~Application()
{
    delete menu;
}

bool Application::init()
{
    bool retValue = false;

    // setup SDL and IMGUI binding
    retValue = initSDL();

    // continue if everything initialized ok
    if (retValue)
    {
        ImGui::StyleColorsDark();

        ImGui::DockContext* myDockContext=NULL; // global variable

        // When you init your application:
        myDockContext = ImGui::CreateDockContext();
        ImGui::SetCurrentDockContext(myDockContext);
    }
    return retValue;
}

void Application::run()
{
    int retVal = 0;

    ImVec4 clear_color = ImColor(114, 144, 154);

    while (!done)
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            ImGui_ImplSDL2_ProcessEvent(&event);
            if (event.type == SDL_QUIT)
                done = true;
        }

        // calculate elapsedTime
        lastTime = currTime;
        currTime = SDL_GetPerformanceCounter();
        double deltaTime = (double)((currTime - lastTime) * 1000 / (double)SDL_GetPerformanceFrequency());

        tick_5sec += deltaTime;
        if (tick_5sec >= 5000)
        {
            tick_5sec = 0;
            Event_TimerEveryFiveSec.notify(true);
        }

        tick_1 += deltaTime;
        if (tick_1 >= 1000)
        {
            tick_1 = 0;
            Event_TimerOnePerSec.notify(true);
        }

        tick_4 += deltaTime;
        if (tick_4 >= 250)
        {
            tick_4 = 0;
            Event_TimerFourPerSec.notify(true);
        }

        tick_2 += deltaTime;
        if (tick_2 >= 500)
        {
            tick_2 = 0;
            Event_TimerTwoPerSec.notify(true);
        }

        tick_50ms += deltaTime;
        if (tick_50ms >= 10)
        {
            tick_50ms = 0;
            Event_Timer10ms.notify(true);
        }

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame(window);
        ImGui::NewFrame();

        // draw main menu separate as it's not a docking panel
        menu->draw(deltaTime);

        ImVec2 temp = ImGui::GetIO().DisplaySize;
        temp.y -= menu->getHeight();

        ImGui::SetNextWindowSize(temp);
        ImGui::SetNextWindowPos(ImVec2(0, menu->getHeight()));
        const ImGuiWindowFlags flags =  (ImGuiWindowFlags_NoMove
                                         | ImGuiWindowFlags_NoBringToFrontOnFocus
                                         | ImGuiWindowFlags_NoResize
                                         | ImGuiWindowFlags_NoScrollbar
                                         | ImGuiWindowFlags_NoSavedSettings
                                         | ImGuiWindowFlags_NoTitleBar);

        //const float oldWindowRounding = ImGui::GetStyle().WindowRounding;ImGui::GetStyle().WindowRounding = 0;
        const bool visible = ImGui::Begin("imguidock window",NULL,ImVec2(0, 0),1.0f,flags);
        // ImGui::GetStyle().WindowRounding = oldWindowRounding;

        ImGui::BeginDockspace();

        // notify all application components to draw (if needed)
        Event_Draw.notify(deltaTime);

        ImGui::EndDockspace();

		ImGui::End();

        // Rendering
        ImGui::Render();

        // make sure the current context is correct (you can have multiple opengl contexts in an app)
        SDL_GL_MakeCurrent(window, glcontext);

        // make sure imgui and the opengl viewport are the same size
        glViewport(0, 0, (int)ImGui::GetIO().DisplaySize.x, (int)ImGui::GetIO().DisplaySize.y);

        // clear the screen with specific color (in preparation for next frame)
        glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);

        // render the imgui data via opengl to the screen
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // swap buffers (which shows the rendered frame)
        SDL_GL_SwapWindow(window);
    }

    // GUI shutdown
    shutdown();

	
}

bool Application::initSDL()
{
    bool retValue = true;

    // Setup SDL
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0)
    {
        retValue = false;
    }
    else
    {
        // Setup window

        // GL 3.0 + GLSL 130
        const char* glsl_version = "#version 130";
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);

        // Create window with graphics context
        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
        SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
        SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

        SDL_DisplayMode current;
        SDL_GetCurrentDisplayMode(0, &current);

		std::string title = "Mir Configurator - " + std::string(REVISION_MAJOR) + "." + std::string(REVISION_MINOR) + "." + std::string(REVISION_BUILD);

        window = SDL_CreateWindow(title.c_str(),
                                  SDL_WINDOWPOS_CENTERED,
                                  SDL_WINDOWPOS_CENTERED,
                                  PIXEL_WIDTH,
                                  PIXEL_HEIGHT,
                                  SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_MAXIMIZED);
        glcontext = SDL_GL_CreateContext(window);
        SDL_GL_SetSwapInterval(1); // Enable vsync

        // init OpenGL loader
        if (glewInit() == GLEW_OK)
        {

            // Setup Dear ImGui context
            IMGUI_CHECKVERSION();
            ImGui::CreateContext();
            ImGuiIO& io = ImGui::GetIO(); (void)io;

            // Setup Platform/Renderer bindings
            ImGui_ImplSDL2_InitForOpenGL(window, glcontext);
            ImGui_ImplOpenGL3_Init();

            // Setup Style
            ImGui::StyleColorsDark();
        }
        else
        {
            retValue = false;
        }
    }

    return retValue;
}

void Application::shutdown()
{
    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    SDL_GL_DeleteContext(glcontext);
    SDL_DestroyWindow(window);
    SDL_Quit();
}
