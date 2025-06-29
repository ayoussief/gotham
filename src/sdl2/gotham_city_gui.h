// Copyright (c) 2025 The Gotham Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or https://opensource.org/license/mit/.

#ifndef GOTHAM_SDL2_GOTHAM_CITY_GUI_H
#define GOTHAM_SDL2_GOTHAM_CITY_GUI_H

#include <SDL2/SDL.h>
#include <memory>
#include <vector>

// Forward declarations
namespace interfaces {
    class Node;
}

class GothamCityApp;
class WindowManager;
class Screen;
class Renderer;
class ThemeManager;
class FontManager;
class TextureManager;
class AnimationManager;
class WalletManager;
class NavigationManager;
class PersistentLayout;

enum class ScreenType {
    SPLASH,
    MAIN,
    WALLET,
    SEND,
    RECEIVE,
    TRANSACTIONS,
    SETTINGS,
    CONSOLE
};

/**
 * Main GUI manager for Gotham City
 */
class GothamCityGUI
{
public:
    GothamCityGUI(WindowManager& window_manager, GothamCityApp& app, interfaces::Node* node = nullptr);
    ~GothamCityGUI();

    /**
     * Initialize the GUI system
     */
    bool Initialize();

    /**
     * Handle SDL events
     */
    void HandleEvent(const SDL_Event& event);

    /**
     * Update GUI state
     */
    void Update();

    /**
     * Render the GUI
     */
    void Render();

    /**
     * Switch to a different screen
     */
    void SwitchScreen(ScreenType screen_type);
    
    /**
     * Get the previous screen type for context-aware navigation
     */
    ScreenType GetPreviousScreen() const { return m_previous_screen_type; }

    /**
     * Get the current renderer
     */
    Renderer* GetRenderer() const { return m_renderer.get(); }

    /**
     * Get the theme manager
     */
    ThemeManager* GetThemeManager() const { return m_theme_manager.get(); }

    /**
     * Get the font manager
     */
    FontManager* GetFontManager() const { return m_font_manager.get(); }

    /**
     * Get the texture manager
     */
    TextureManager* GetTextureManager() const { return m_texture_manager.get(); }

    /**
     * Get the animation manager
     */
    AnimationManager* GetAnimationManager() const { return m_animation_manager.get(); }

    /**
     * Get the wallet manager
     */
    WalletManager* GetWalletManager() const { return m_wallet_manager.get(); }

    /**
     * Get the navigation manager
     */
    NavigationManager* GetNavigationManager() const { return m_navigation_manager.get(); }

    /**
     * Get the persistent layout
     */
    PersistentLayout* GetPersistentLayout() const { return m_persistent_layout.get(); }

    /**
     * Get the app instance
     */
    GothamCityApp& GetApp() const { return m_app; }

private:
    WindowManager& m_window_manager;
    GothamCityApp& m_app;
    interfaces::Node* m_node;

    // Core systems
    std::unique_ptr<Renderer> m_renderer;
    std::unique_ptr<ThemeManager> m_theme_manager;
    std::unique_ptr<FontManager> m_font_manager;
    std::unique_ptr<TextureManager> m_texture_manager;
    std::unique_ptr<AnimationManager> m_animation_manager;
    std::unique_ptr<WalletManager> m_wallet_manager;

    // Modern UI systems
    std::unique_ptr<NavigationManager> m_navigation_manager;
    std::unique_ptr<PersistentLayout> m_persistent_layout;

    // Screen management (legacy - will be replaced by persistent layout)
    std::unique_ptr<Screen> m_current_screen;
    ScreenType m_current_screen_type{ScreenType::SPLASH};
    ScreenType m_previous_screen_type{ScreenType::SPLASH};

    // State
    bool m_initialized{false};
    Uint32 m_last_update_time{0};

    std::unique_ptr<Screen> CreateScreen(ScreenType screen_type);
    std::unique_ptr<Screen> CreateScreenWithContext(ScreenType screen_type, const class NavigationContext& context);
    void SwitchScreenWithContext(ScreenType screen_type, const class NavigationContext& context);
    bool InitializePersistentLayout();
    void SetupNavigationItems();
};

#endif // GOTHAM_SDL2_GOTHAM_CITY_GUI_H