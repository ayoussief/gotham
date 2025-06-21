// Copyright (c) 2025 The Gotham Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or https://opensource.org/license/mit/.

#include "gotham_city_gui.h"
#include "window_manager.h"
#include "renderer.h"
#include "utils/theme.h"
#include "utils/font_manager.h"
#include "utils/texture_manager.h"
#include "utils/animation.h"
#include "wallet/wallet_manager.h"
#include "ui/screen.h"
#include "screens/splash_screen.h"
#include "screens/main_screen.h"
#include "screens/wallet_screen.h"
#include "screens/send_screen.h"
#include "screens/receive_screen.h"
#include "screens/transactions_screen.h"
#include "screens/settings_screen.h"
#include "screens/console_screen.h"

#include <iostream>

GothamCityGUI::GothamCityGUI(WindowManager& window_manager, interfaces::Node* node)
    : m_window_manager(window_manager)
    , m_node(node)
{
}

GothamCityGUI::~GothamCityGUI() = default;

bool GothamCityGUI::Initialize()
{
    if (m_initialized) {
        return true;
    }

    // Create renderer
    m_renderer = std::make_unique<Renderer>(m_window_manager);
    if (!m_renderer->Initialize()) {
        std::cerr << "Failed to initialize renderer" << std::endl;
        return false;
    }

    // Create managers
    m_theme_manager = std::make_unique<ThemeManager>();
    m_font_manager = std::make_unique<FontManager>(*m_renderer);
    m_texture_manager = std::make_unique<TextureManager>(*m_renderer);
    m_animation_manager = std::make_unique<AnimationManager>();
    m_wallet_manager = std::make_unique<WalletManager>();

    // Initialize managers
    if (!m_theme_manager->Initialize()) {
        std::cerr << "Failed to initialize theme manager" << std::endl;
        return false;
    }

    if (!m_font_manager->Initialize()) {
        std::cerr << "Failed to initialize font manager" << std::endl;
        return false;
    }

    if (!m_texture_manager->Initialize()) {
        std::cerr << "Failed to initialize texture manager" << std::endl;
        return false;
    }

    if (!m_wallet_manager->Initialize()) {
        std::cerr << "Failed to initialize wallet manager" << std::endl;
        return false;
    }

    // Start with splash screen
    SwitchScreen(ScreenType::SPLASH);

    m_last_update_time = SDL_GetTicks();
    m_initialized = true;

    std::cout << "Gotham City GUI initialized successfully" << std::endl;
    return true;
}

void GothamCityGUI::HandleEvent(const SDL_Event& event)
{
    if (!m_initialized || !m_current_screen) {
        return;
    }

    // Handle global events
    switch (event.type) {
        case SDL_KEYDOWN:
            // Handle global hotkeys
            if (event.key.keysym.sym == SDLK_F11) {
                m_window_manager.ToggleFullscreen();
                return;
            }
            break;
        case SDL_WINDOWEVENT:
            if (event.window.event == SDL_WINDOWEVENT_RESIZED) {
                // Handle window resize
                int new_width = event.window.data1;
                int new_height = event.window.data2;
                std::cout << "Window resized to: " << new_width << "x" << new_height << std::endl;
                
                // Update window manager with new size
                m_window_manager.UpdateWindowSize();
                
                // Update renderer viewport
                if (m_renderer) {
                    m_renderer->HandleResize(new_width, new_height);
                }
                
                // Notify current screen of resize
                if (m_current_screen) {
                    m_current_screen->OnResize(new_width, new_height);
                }
                return;
            }
            break;
    }

    // Pass event to current screen
    m_current_screen->HandleEvent(event);
}

void GothamCityGUI::Update()
{
    if (!m_initialized) {
        return;
    }

    Uint32 current_time = SDL_GetTicks();
    float delta_time = (current_time - m_last_update_time) / 1000.0f;
    m_last_update_time = current_time;

    // Update animation manager
    m_animation_manager->Update(delta_time);

    // Update current screen
    if (m_current_screen) {
        m_current_screen->Update(delta_time);
    }
}

void GothamCityGUI::Render()
{
    if (!m_initialized || !m_renderer) {
        return;
    }

    // Clear screen with dark background
    m_renderer->Clear(m_theme_manager->GetColor(ThemeColor::BACKGROUND));

    // Render current screen
    if (m_current_screen) {
        m_current_screen->Render(*m_renderer);
    }

    // Present the rendered frame
    m_renderer->Present();
}

void GothamCityGUI::SwitchScreen(ScreenType screen_type)
{
    if (screen_type == m_current_screen_type && m_current_screen) {
        return;
    }

    std::cout << "Switching to screen: " << static_cast<int>(screen_type) << std::endl;

    m_current_screen = CreateScreen(screen_type);
    m_current_screen_type = screen_type;

    if (m_current_screen && !m_current_screen->Initialize()) {
        std::cerr << "Failed to initialize screen" << std::endl;
        m_current_screen.reset();
        return;
    }

    // Activate the screen
    if (m_current_screen) {
        m_current_screen->OnActivate();
    }
}

std::unique_ptr<Screen> GothamCityGUI::CreateScreen(ScreenType screen_type)
{
    switch (screen_type) {
        case ScreenType::SPLASH:
            return std::make_unique<SplashScreen>(*this);
        case ScreenType::MAIN:
            return std::make_unique<MainScreen>(*this);
        case ScreenType::WALLET:
            return std::make_unique<WalletScreen>(*this);
        case ScreenType::SEND:
            return std::make_unique<SendScreen>(*this);
        case ScreenType::RECEIVE:
            return std::make_unique<ReceiveScreen>(*this);
        case ScreenType::TRANSACTIONS:
            std::cout << "Creating TransactionsScreen..." << std::endl;
            return std::make_unique<TransactionsScreen>(*this);
        case ScreenType::SETTINGS:
            return std::make_unique<SettingsScreen>(*this);
        case ScreenType::CONSOLE:
            return std::make_unique<ConsoleScreen>(*this);
        default:
            std::cerr << "Unknown screen type: " << static_cast<int>(screen_type) << std::endl;
            return nullptr;
    }
}