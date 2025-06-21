// Copyright (c) 2025 The Gotham Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or https://opensource.org/license/mit/.

#ifndef GOTHAM_SDL2_WINDOW_MANAGER_H
#define GOTHAM_SDL2_WINDOW_MANAGER_H

#include <SDL2/SDL.h>
#include <string>

/**
 * Window management for Gotham City SDL2 GUI
 */
class WindowManager
{
public:
    WindowManager();
    ~WindowManager();

    /**
     * Initialize the window
     */
    bool Initialize(const std::string& title = "Gotham City", int width = 1200, int height = 800, bool fullscreen = false);

    /**
     * Get the SDL window
     */
    SDL_Window* GetWindow() const { return m_window; }

    /**
     * Get window width
     */
    int GetWidth() const { return m_width; }

    /**
     * Get window height
     */
    int GetHeight() const { return m_height; }

    /**
     * Check if window is fullscreen
     */
    bool IsFullscreen() const { return m_fullscreen; }

    /**
     * Toggle fullscreen mode
     */
    void ToggleFullscreen();

    /**
     * Set window title
     */
    void SetTitle(const std::string& title);

    /**
     * Get window title
     */
    const std::string& GetTitle() const { return m_title; }

    /**
     * Update window size from SDL
     */
    void UpdateWindowSize();

private:
    SDL_Window* m_window{nullptr};
    std::string m_title{"Gotham City"};
    int m_width{1200};
    int m_height{800};
    bool m_fullscreen{false};
};

#endif // GOTHAM_SDL2_WINDOW_MANAGER_H