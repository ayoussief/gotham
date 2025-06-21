// Copyright (c) 2025 The Gotham Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or https://opensource.org/license/mit/.

#include "gotham_city_app.h"
#include "gotham_city_gui.h"
#include "window_manager.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>

#include <common/system.h>
#include <common/init.h>
#include <common/args.h>

#include <iostream>

GothamCityApp::GothamCityApp() = default;

GothamCityApp::~GothamCityApp()
{
    Shutdown();
}

bool GothamCityApp::Initialize(int argc, char* argv[])
{
    if (m_initialized) {
        return true;
    }

    std::cout << "🦇 Initializing Gotham City Application..." << std::endl;

    // Parse command line arguments
    m_args = std::make_unique<ArgsManager>();
    SetupHelpOptions(*m_args);
    
    std::string error;
    if (!m_args->ParseParameters(argc, argv, error)) {
        std::cerr << "Error parsing arguments: " << error << std::endl;
        return false;
    }

    // Initialize SDL
    if (!InitializeSDL()) {
        std::cerr << "Failed to initialize SDL" << std::endl;
        return false;
    }

    // Create window
    if (!CreateWindow()) {
        std::cerr << "Failed to create window" << std::endl;
        return false;
    }

    // Initialize Gotham Core
    if (!InitGotham(*m_args)) {
        std::cerr << "Failed to initialize Gotham Core" << std::endl;
        return false;
    }

    // Create node
    static interfaces::NodeContext node_context;
    m_node = interfaces::MakeNode(node_context);
    
    if (!m_node->start()) {
        std::cerr << "Failed to start node" << std::endl;
        return false;
    }

    // Initialize GUI
    if (!InitializeGUI()) {
        std::cerr << "Failed to initialize GUI" << std::endl;
        return false;
    }

    m_initialized = true;
    std::cout << "✅ Gotham City Application initialized successfully" << std::endl;
    return true;
}

int GothamCityApp::Run()
{
    if (!m_initialized) {
        std::cerr << "Application not initialized" << std::endl;
        return 1;
    }

    std::cout << "🦇 Starting Gotham City main loop..." << std::endl;

    // Main application loop
    while (!ShouldExit()) {
        HandleEvents();
        Update();
        Render();
        
        // Small delay to prevent 100% CPU usage
        SDL_Delay(1);
    }

    std::cout << "🦇 Gotham City main loop ended" << std::endl;
    return 0;
}

void GothamCityApp::Shutdown()
{
    if (!m_initialized) {
        return;
    }

    std::cout << "🦇 Shutting down Gotham City Application..." << std::endl;

    // Shutdown GUI
    m_gui.reset();

    // Stop node
    if (m_node) {
        m_node->stop();
        m_node.reset();
    }

    // Shutdown Gotham Core
    ShutdownGotham();

    // Shutdown window manager
    m_window_manager.reset();

    // Cleanup SDL
    CleanupSDL();

    m_initialized = false;
    std::cout << "✅ Gotham City Application shutdown complete" << std::endl;
}

bool GothamCityApp::InitializeSDL()
{
    std::cout << "🔧 Initializing SDL..." << std::endl;

    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_EVENTS) < 0) {
        std::cerr << "SDL could not initialize! SDL Error: " << SDL_GetError() << std::endl;
        return false;
    }

    // Initialize SDL_ttf
    if (TTF_Init() == -1) {
        std::cerr << "SDL_ttf could not initialize! SDL_ttf Error: " << TTF_GetError() << std::endl;
        return false;
    }

    // Initialize SDL_image
    int img_flags = IMG_INIT_PNG | IMG_INIT_JPG;
    if (!(IMG_Init(img_flags) & img_flags)) {
        std::cerr << "SDL_image could not initialize! SDL_image Error: " << IMG_GetError() << std::endl;
        return false;
    }

    std::cout << "✅ SDL initialized successfully" << std::endl;
    return true;
}

bool GothamCityApp::CreateWindow()
{
    std::cout << "🪟 Creating window..." << std::endl;

    // Get window dimensions from arguments
    int width = static_cast<int>(m_args->GetIntArg("width", 1200));
    int height = static_cast<int>(m_args->GetIntArg("height", 800));
    bool fullscreen = m_args->GetBoolArg("fullscreen", false);

    m_window_manager = std::make_unique<WindowManager>();
    
    if (!m_window_manager->Initialize("Gotham City", width, height, fullscreen)) {
        std::cerr << "Failed to create window" << std::endl;
        return false;
    }

    std::cout << "✅ Window created successfully" << std::endl;
    return true;
}

bool GothamCityApp::InitializeGUI()
{
    std::cout << "🎨 Initializing GUI..." << std::endl;

    m_gui = std::make_unique<GothamCityGUI>(*m_window_manager, m_node.get());
    
    if (!m_gui->Initialize()) {
        std::cerr << "Failed to initialize GUI" << std::endl;
        return false;
    }

    std::cout << "✅ GUI initialized successfully" << std::endl;
    return true;
}

void GothamCityApp::HandleEvents()
{
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
                RequestExit();
                break;
                
            case SDL_KEYDOWN:
                if (event.key.keysym.sym == SDLK_ESCAPE) {
                    RequestExit();
                }
                break;
        }

        // Pass event to GUI
        if (m_gui) {
            m_gui->HandleEvent(event);
        }
    }
}

void GothamCityApp::Update()
{
    if (m_gui) {
        m_gui->Update();
    }
}

void GothamCityApp::Render()
{
    if (m_gui) {
        m_gui->Render();
    }
}

void GothamCityApp::CleanupSDL()
{
    std::cout << "🧹 Cleaning up SDL..." << std::endl;

    IMG_Quit();
    TTF_Quit();
    SDL_Quit();

    std::cout << "✅ SDL cleanup complete" << std::endl;
}