// Copyright (c) 2025 The Gotham Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or https://opensource.org/license/mit/.

#ifndef GOTHAM_SDL2_GOTHAM_CITY_APP_H
#define GOTHAM_SDL2_GOTHAM_CITY_APP_H

#include <common/args.h>
#include <interfaces/node.h>
#include <memory>
#include <atomic>
#include <string>

class GothamCityGUI;
class WindowManager;

/**
 * Main application class for Gotham City SDL2 GUI
 */
class GothamCityApp
{
public:
    GothamCityApp();
    ~GothamCityApp();

    /**
     * Initialize the application
     */
    bool Initialize(int argc, char* argv[]);

    /**
     * Run the main application loop
     */
    int Run();

    /**
     * Shutdown the application
     */
    void Shutdown();

    /**
     * Request application exit
     */
    void RequestExit() { m_should_exit = true; }

    /**
     * Check if application should exit
     */
    bool ShouldExit() const { return m_should_exit; }

private:
    std::unique_ptr<WindowManager> m_window_manager;
    std::unique_ptr<GothamCityGUI> m_gui;
    std::unique_ptr<interfaces::Node> m_node;
    std::unique_ptr<ArgsManager> m_args;
    
    std::atomic<bool> m_should_exit{false};
    bool m_initialized{false};

    bool InitializeSDL();
    bool CreateWindow();
    bool InitializeGUI();
    void HandleEvents();
    void Update();
    void Render();
    void CleanupSDL();
};

#endif // GOTHAM_SDL2_GOTHAM_CITY_APP_H