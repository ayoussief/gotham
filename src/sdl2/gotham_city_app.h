// Copyright (c) 2025 The Gotham Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or https://opensource.org/license/mit/.

#ifndef GOTHAM_SDL2_GOTHAM_CITY_APP_H
#define GOTHAM_SDL2_GOTHAM_CITY_APP_H

#include <common/args.h>
#include <interfaces/node.h>
#include <interfaces/init.h>
#include <memory>
#include <atomic>
#include <string>
#include <functional>

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

    /**
     * Start the Gotham node (using existing Core functionality)
     */
    bool StartDaemon();

    /**
     * Stop the Gotham node (using existing Core functionality)
     */
    void StopDaemon();

    /**
     * Check if node is running
     */
    bool IsDaemonRunning() const;

    /**
     * Get node status information (using existing Core functionality)
     */
    std::string GetDaemonStatus() const;

    /**
     * Get node interface (for RPC calls, etc.)
     */
    interfaces::Node* GetNode() const { return m_node.get(); }

private:
    std::unique_ptr<WindowManager> m_window_manager;
    std::unique_ptr<GothamCityGUI> m_gui;
    
    // Real Gotham Core node components
    std::unique_ptr<interfaces::Node> m_node;
    std::unique_ptr<interfaces::Init> m_init;
    std::unique_ptr<ArgsManager> m_args;
    
    std::atomic<bool> m_should_exit{false};
    std::atomic<bool> m_daemon_running{false};
    bool m_initialized{false};
    
    // Store command line arguments
    int m_argc{0};
    char** m_argv{nullptr};

    bool InitializeSDL();
    bool CreateWindow();
    bool InitializeGUI();
    void HandleEvents();
    void Update();
    void Render();
    void CleanupSDL();
};

#endif // GOTHAM_SDL2_GOTHAM_CITY_APP_H