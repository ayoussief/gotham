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
#include "ui/navigation_manager.h"
#include "ui/persistent_layout.h"
#include "ui/ui_factory.h"
#include "ui/layout_manager.h"
#include "screens/splash_screen.h"
#include "screens/main_screen.h"
#include "screens/wallet_screen.h"
#include "screens/send_screen.h"
#include "screens/receive_screen.h"
#include "screens/transactions_screen.h"
#include "screens/settings_screen.h"
#include "screens/console_screen.h"

#include <iostream>

GothamCityGUI::GothamCityGUI(WindowManager& window_manager, GothamCityApp& app, interfaces::Node* node)
    : m_window_manager(window_manager)
    , m_app(app)
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

    // Create modern UI systems
    m_navigation_manager = std::make_unique<NavigationManager>();
    m_persistent_layout = std::make_unique<PersistentLayout>();

    // Initialize modern UI systems
    if (!InitializePersistentLayout()) {
        std::cerr << "Failed to initialize persistent layout" << std::endl;
        return false;
    }

    // Start with splash screen (legacy mode for splash, then switch to persistent layout)
    SwitchScreen(ScreenType::SPLASH);

    m_last_update_time = SDL_GetTicks();
    m_initialized = true;

    std::cout << "Gotham City GUI initialized successfully" << std::endl;
    return true;
}

void GothamCityGUI::HandleEvent(const SDL_Event& event)
{
    if (!m_initialized) {
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
                
                // Update persistent layout for new size
                if (m_persistent_layout) {
                    m_persistent_layout->OnResize(new_width, new_height);
                }
                
                // Notify current screen of resize (legacy support)
                if (m_current_screen) {
                    m_current_screen->OnResize(new_width, new_height);
                }
                return;
            }
            break;
    }

    // If using persistent layout (modern mode), handle events there first
    if (m_persistent_layout && m_current_screen_type != ScreenType::SPLASH) {
        m_persistent_layout->HandleEvent(event);
    }
    
    // Pass event to current screen (for splash screen or content area)
    if (m_current_screen) {
        m_current_screen->HandleEvent(event);
    }
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

    // Update persistent layout (modern mode)
    if (m_persistent_layout && m_current_screen_type != ScreenType::SPLASH) {
        m_persistent_layout->Update(delta_time);
    }

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

    // Modern persistent layout rendering
    if (m_persistent_layout && m_current_screen_type != ScreenType::SPLASH) {
        // Render persistent elements (header, sidebar, status bar)
        m_persistent_layout->RenderPersistentElements(*m_renderer, *m_font_manager);
        
        // Render content area (current screen)
        m_persistent_layout->RenderContentArea(*m_renderer);
    } else {
        // Legacy rendering for splash screen
        if (m_current_screen) {
            m_current_screen->Render(*m_renderer);
        }
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

    // Track previous screen for context-aware navigation
    m_previous_screen_type = m_current_screen_type;
    m_current_screen_type = screen_type;

    // Handle splash screen with legacy mode
    if (screen_type == ScreenType::SPLASH) {
        m_current_screen = CreateScreen(screen_type);
        if (m_current_screen && !m_current_screen->Initialize()) {
            std::cerr << "Failed to initialize screen" << std::endl;
            m_current_screen.reset();
            return;
        }
        if (m_current_screen) {
            m_current_screen->OnActivate();
        }
        return;
    }

    // Modern persistent layout mode for all other screens
    if (m_persistent_layout) {
        // Create screen for content area only
        auto content_screen = CreateScreen(screen_type);
        if (content_screen && !content_screen->Initialize()) {
            std::cerr << "Failed to initialize content screen" << std::endl;
            return;
        }

        // Set content area in persistent layout
        m_persistent_layout->SetContentArea(std::move(content_screen));
        m_persistent_layout->SetActiveNavigationItem(screen_type);
        
        // Update current screen reference for compatibility (raw pointer, not ownership)
        m_current_screen.reset(); // Clear the unique_ptr since persistent layout owns the screen now
        
        // Activate the content screen through persistent layout
        if (auto* content_screen_ptr = m_persistent_layout->GetContentScreen()) {
            content_screen_ptr->OnActivate();
        }
    } else {
        // Fallback to legacy mode if persistent layout not available
        m_current_screen = CreateScreen(screen_type);
        if (m_current_screen && !m_current_screen->Initialize()) {
            std::cerr << "Failed to initialize screen" << std::endl;
            m_current_screen.reset();
            return;
        }
        if (m_current_screen) {
            m_current_screen->OnActivate();
        }
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

std::unique_ptr<Screen> GothamCityGUI::CreateScreenWithContext(ScreenType screen_type, const NavigationContext& context)
{
    // For now, create screen normally and pass context during initialization
    auto screen = CreateScreen(screen_type);
    // TODO: Pass context to screen initialization when NavigationContext is fully implemented
    return screen;
}

void GothamCityGUI::SwitchScreenWithContext(ScreenType screen_type, const NavigationContext& context)
{
    // For now, use regular screen switching
    // TODO: Implement full context-aware navigation
    SwitchScreen(screen_type);
}

bool GothamCityGUI::InitializePersistentLayout()
{
    if (!m_persistent_layout) {
        return false;
    }

    // Create UI factory and layout manager for persistent layout
    auto ui_factory = std::make_unique<UIFactory>(*m_theme_manager);
    auto layout_manager = std::make_unique<LayoutManager>();

    // Initialize persistent layout with systems
    if (!m_persistent_layout->Initialize(*ui_factory, *layout_manager, *m_navigation_manager)) {
        std::cerr << "Failed to initialize persistent layout" << std::endl;
        return false;
    }

    // Set viewport bounds
    int width = m_window_manager.GetWidth();
    int height = m_window_manager.GetHeight();
    m_persistent_layout->SetViewportBounds(Rect(0, 0, width, height));

    // Setup navigation items
    SetupNavigationItems();

    std::cout << "Persistent layout initialized successfully" << std::endl;
    return true;
}

void GothamCityGUI::SetupNavigationItems()
{
    if (!m_persistent_layout) {
        return;
    }

    // Add navigation items to sidebar
    m_persistent_layout->AddNavigationItem("Main", "🏠", ScreenType::MAIN, 
        [this]() { SwitchScreen(ScreenType::MAIN); });
    
    m_persistent_layout->AddNavigationItem("Wallet", "💰", ScreenType::WALLET, 
        [this]() { SwitchScreen(ScreenType::WALLET); });
    
    m_persistent_layout->AddNavigationItem("Send", "📤", ScreenType::SEND, 
        [this]() { SwitchScreen(ScreenType::SEND); });
    
    m_persistent_layout->AddNavigationItem("Receive", "📥", ScreenType::RECEIVE, 
        [this]() { SwitchScreen(ScreenType::RECEIVE); });
    
    m_persistent_layout->AddNavigationItem("History", "📋", ScreenType::TRANSACTIONS, 
        [this]() { SwitchScreen(ScreenType::TRANSACTIONS); });
    
    m_persistent_layout->AddNavigationItem("Settings", "⚙️", ScreenType::SETTINGS, 
        [this]() { SwitchScreen(ScreenType::SETTINGS); });
    
    m_persistent_layout->AddNavigationItem("Console", "💻", ScreenType::CONSOLE, 
        [this]() { SwitchScreen(ScreenType::CONSOLE); });

    std::cout << "Navigation items setup complete" << std::endl;
}