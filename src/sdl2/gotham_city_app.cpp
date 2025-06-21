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
#include <interfaces/init.h>
#include <init.h>
#include <util/threadnames.h>
#include <chainparams.h>
#include <logging.h>
#include <netbase.h>

#include <iostream>
#include <chrono>
#include <thread>

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

    // Chain parameters and argument parsing are now handled in main.cpp
    // Store argc/argv for later use
    m_argc = argc;
    m_argv = argv;

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

    std::cout << "✅ SDL2 initialized successfully" << std::endl;

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

    // Main application loop with proper frame limiting
    const int TARGET_FPS = 60;
    const int FRAME_DELAY = 1000 / TARGET_FPS; // ~16.67ms per frame
    Uint32 frame_start;
    int frame_time;
    
    while (!ShouldExit()) {
        frame_start = SDL_GetTicks();
        
        HandleEvents();
        Update();
        Render();
        
        // Frame rate limiting
        frame_time = SDL_GetTicks() - frame_start;
        if (FRAME_DELAY > frame_time) {
            SDL_Delay(FRAME_DELAY - frame_time);
        }
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

    // Stop daemon if running
    if (m_daemon_running.load()) {
        StopDaemon();
    }

    // Shutdown node
    if (m_node) {
        m_node->appShutdown();
        m_node.reset();
    }

    // Clean up init interface
    m_init.reset();

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
    int width = static_cast<int>(gArgs.GetIntArg("-width", 1200));
    int height = static_cast<int>(gArgs.GetIntArg("-height", 800));
    bool fullscreen = gArgs.GetBoolArg("-fullscreen", false);

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

    m_gui = std::make_unique<GothamCityGUI>(*m_window_manager, *this, m_node.get());
    
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

// Node control methods - using existing Gotham Core functionality
bool GothamCityApp::StartDaemon()
{
    if (!m_node) {
        std::cerr << "❌ Node interface not available" << std::endl;
        return false;
    }

    if (m_daemon_running.load()) {
        std::cout << "⚠️ Node is already running" << std::endl;
        return true;
    }

    std::cout << "🚀 Starting Gotham node..." << std::endl;

    try {
        // Use the existing node interface methods (following Qt pattern)
        if (!m_node->baseInitialize()) {
            std::cerr << "❌ Failed to initialize node base" << std::endl;
            return false;
        }

        // Start the main application initialization
        interfaces::BlockAndHeaderTipInfo tip_info;
        if (!m_node->appInitMain(&tip_info)) {
            std::cerr << "❌ Failed to start node main application" << std::endl;
            return false;
        }

        m_daemon_running.store(true);
        std::cout << "✅ Gotham node started successfully" << std::endl;
        std::cout << "📊 Block height: " << tip_info.block_height << std::endl;
        std::cout << "🔗 Header height: " << tip_info.header_height << std::endl;
        
        return true;
    }
    catch (const std::exception& e) {
        std::cerr << "❌ Exception starting node: " << e.what() << std::endl;
        return false;
    }
}

void GothamCityApp::StopDaemon()
{
    if (!m_daemon_running.load()) {
        std::cout << "⚠️ Node is not running" << std::endl;
        return;
    }

    std::cout << "🛑 Stopping Gotham node..." << std::endl;

    if (m_node) {
        // Use the existing node shutdown method
        m_node->startShutdown();
        
        // Wait for shutdown to complete (following Qt pattern)
        int timeout = 30; // 30 seconds timeout
        while (!m_node->shutdownRequested() && timeout > 0) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
            timeout--;
        }
        
        if (timeout <= 0) {
            std::cout << "⚠️ Node shutdown timeout" << std::endl;
        }
    }

    m_daemon_running.store(false);
    std::cout << "✅ Gotham node stopped" << std::endl;
}

bool GothamCityApp::IsDaemonRunning() const
{
    return m_daemon_running.load();
}

std::string GothamCityApp::GetDaemonStatus() const
{
    if (!m_node) {
        return "Node interface not available";
    }

    if (!m_daemon_running.load()) {
        return "Node stopped";
    }

    try {
        // Use existing node interface methods to get status
        std::string status = "Node running - ";
        
        int block_count = m_node->getNumBlocks();
        size_t connections = m_node->getNodeCount(ConnectionDirection::Both);
        bool is_ibd = m_node->isInitialBlockDownload();
        double sync_progress = m_node->getVerificationProgress();
        
        status += "Blocks: " + std::to_string(block_count);
        status += ", Connections: " + std::to_string(connections);
        
        if (is_ibd) {
            status += ", Syncing: " + std::to_string(static_cast<int>(sync_progress * 100)) + "%";
        } else {
            status += ", Synced";
        }
        
        return status;
    }
    catch (const std::exception& e) {
        return "Error getting status: " + std::string(e.what());
    }
}