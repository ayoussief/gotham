// Copyright (c) 2025 The Gotham Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or https://opensource.org/license/mit/.

#include "window_manager.h"
#include <iostream>

WindowManager::WindowManager() = default;

WindowManager::~WindowManager()
{
    if (m_window) {
        SDL_DestroyWindow(m_window);
        m_window = nullptr;
    }
}

bool WindowManager::Initialize(const std::string& title, int width, int height, bool fullscreen)
{
    m_title = title;
    m_width = width;
    m_height = height;
    m_fullscreen = fullscreen;

    // Create window
    Uint32 window_flags = SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI;
    if (fullscreen) {
        window_flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
    }

    m_window = SDL_CreateWindow(
        m_title.c_str(),
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        m_width,
        m_height,
        window_flags
    );

    if (!m_window) {
        std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        return false;
    }

    // Set window icon (would load from resources in real implementation)
    // SDL_SetWindowIcon(m_window, icon_surface);

    UpdateWindowSize();
    
    std::cout << "Window created: " << m_width << "x" << m_height << std::endl;
    return true;
}

void WindowManager::ToggleFullscreen()
{
    if (!m_window) {
        return;
    }

    m_fullscreen = !m_fullscreen;
    
    if (m_fullscreen) {
        SDL_SetWindowFullscreen(m_window, SDL_WINDOW_FULLSCREEN_DESKTOP);
    } else {
        SDL_SetWindowFullscreen(m_window, 0);
    }

    UpdateWindowSize();
    std::cout << "Fullscreen toggled: " << (m_fullscreen ? "ON" : "OFF") << std::endl;
}

void WindowManager::SetTitle(const std::string& title)
{
    m_title = title;
    if (m_window) {
        SDL_SetWindowTitle(m_window, m_title.c_str());
    }
}

void WindowManager::UpdateWindowSize()
{
    if (m_window) {
        SDL_GetWindowSize(m_window, &m_width, &m_height);
    }
}