// Copyright (c) 2025 The Gotham Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or https://opensource.org/license/mit/.

#ifndef GOTHAM_SDL2_UI_PERSISTENT_LAYOUT_H
#define GOTHAM_SDL2_UI_PERSISTENT_LAYOUT_H

#include "panel.h"
#include "button.h"
#include "label.h"
#include "ui_factory.h"
#include "layout_manager.h"
#include "navigation_manager.h"
#include "../renderer.h"
#include <memory>
#include <vector>
#include <functional>

// Forward declarations
enum class ScreenType;

/**
 * Modern Persistent Layout System (2025 UX Standards)
 * 
 * Implements the "Static Frame" navigation pattern where:
 * - Header (navigation bar) remains fixed
 * - Sidebar navigation remains fixed  
 * - Only the main content area changes dynamically
 * - Status bar remains fixed
 * 
 * This creates a consistent, professional experience similar to:
 * - Gmail, Slack, Discord
 * - Admin dashboards and CMS systems
 * - Modern Single-Page Applications (SPAs)
 */
class PersistentLayout
{
public:
    PersistentLayout();
    ~PersistentLayout() = default;

    /**
     * Initialize the persistent layout system
     */
    bool Initialize(UIFactory& ui_factory, LayoutManager& layout_manager, 
                   NavigationManager& nav_manager);
    
    /**
     * Set the viewport bounds (full screen area)
     */
    void SetViewportBounds(const Rect& bounds);
    
    /**
     * Handle events for persistent UI elements
     */
    void HandleEvent(const SDL_Event& event);
    
    /**
     * Update persistent UI elements
     */
    void Update(float delta_time);
    
    /**
     * Render persistent UI elements (header, sidebar, status bar)
     */
    void RenderPersistentElements(class Renderer& renderer, class FontManager& font_manager);
    
    /**
     * Render content area (current screen)
     */
    void RenderContentArea(class Renderer& renderer);
    
    /**
     * Get the content area bounds (where dynamic content is rendered)
     */
    Rect GetContentAreaBounds() const { return m_content_area_bounds; }
    
    /**
     * Set content area for current screen
     */
    void SetContentArea(std::unique_ptr<class Screen> content_screen);
    
    /**
     * Get current content screen
     */
    Screen* GetContentScreen() const { return m_content_screen.get(); }
    
    /**
     * Update layout when window is resized
     */
    void OnResize(int new_width, int new_height);
    
    /**
     * Set sidebar visibility
     */
    void SetSidebarVisible(bool visible);
    
    /**
     * Set status bar visibility
     */
    void SetStatusBarVisible(bool visible);
    
    /**
     * Add navigation item to sidebar
     */
    void AddNavigationItem(const std::string& text, const std::string& icon, 
                          ScreenType target_screen, std::function<void()> callback = nullptr);
    
    /**
     * Set active navigation item
     */
    void SetActiveNavigationItem(ScreenType screen_type);
    
    /**
     * Update wallet information in header
     */
    void UpdateWalletInfo(const std::string& balance, bool connected);
    
    /**
     * Update network information in status bar
     */
    void UpdateNetworkInfo(int connections, int block_height, const std::string& status);
    
    /**
     * Add contextual action to header
     */
    void AddHeaderAction(const std::string& text, std::function<void()> callback);
    
    /**
     * Clear header actions
     */
    void ClearHeaderActions();

private:
    // UI Systems
    UIFactory* m_ui_factory{nullptr};
    LayoutManager* m_layout_manager{nullptr};
    NavigationManager* m_nav_manager{nullptr};
    
    // Layout bounds
    Rect m_viewport_bounds;
    Rect m_header_bounds;
    Rect m_sidebar_bounds;
    Rect m_content_area_bounds;
    Rect m_status_bar_bounds;
    
    // Persistent UI Components  
    std::unique_ptr<Panel> m_header_panel;
    std::unique_ptr<Panel> m_sidebar_panel;
    std::unique_ptr<Panel> m_status_bar_panel;
    
    // Header components
    std::unique_ptr<Label> m_app_title_label;
    std::unique_ptr<Label> m_balance_label;
    std::unique_ptr<Button> m_settings_button;
    std::vector<std::unique_ptr<Button>> m_header_action_buttons;
    
    // Sidebar navigation
    struct NavigationItem {
        std::unique_ptr<Button> button;
        ScreenType target_screen;
        std::function<void()> callback;
        bool is_active{false};
    };
    std::vector<NavigationItem> m_navigation_items;
    
    // Status bar components
    std::unique_ptr<Label> m_connection_label;
    std::unique_ptr<Label> m_block_height_label;
    std::unique_ptr<Label> m_sync_status_label;
    
    // Content area
    std::unique_ptr<Screen> m_content_screen;
    
    // State
    bool m_initialized{false};
    bool m_sidebar_visible{true};
    bool m_status_bar_visible{true};
    ScreenType m_active_screen;
    
    // Layout management
    void CalculateLayoutBounds();
    void CreatePersistentComponents();
    void CreateHeaderComponents();
    void CreateSidebarComponents();
    void CreateStatusBarComponents();
    void UpdateLayout();
    void UpdateNavigationStates();
    
    // Professional layout methods
    void LayoutHeaderComponents();
    void LayoutStatusBarComponents();
    
    // Event handlers
    void OnNavigationItemClicked(ScreenType screen_type, std::function<void()> callback);
    void OnSettingsClicked();

protected:
    // Allow derived classes to access layout update
    void UpdateLayoutProtected() { UpdateLayout(); }
};

/**
 * Layout configuration for different screen sizes
 */
struct LayoutConfiguration {
    int header_height{60};
    int sidebar_width{200};
    int status_bar_height{30};
    bool sidebar_collapsible{true};
    bool responsive_sidebar{true};
    
    static LayoutConfiguration GetDefault() {
        return LayoutConfiguration{};
    }
    
    static LayoutConfiguration GetMobile() {
        LayoutConfiguration config;
        config.sidebar_width = 60; // Collapsed by default
        config.sidebar_collapsible = true;
        config.responsive_sidebar = true;
        return config;
    }
    
    static LayoutConfiguration GetTablet() {
        LayoutConfiguration config;
        config.sidebar_width = 180;
        return config;
    }
};

/**
 * Responsive layout manager for persistent layout
 */
class ResponsivePersistentLayout : public PersistentLayout
{
public:
    ResponsivePersistentLayout(const LayoutConfiguration& config = LayoutConfiguration::GetDefault());
    
    /**
     * Update layout based on screen size
     */
    void UpdateResponsiveLayout(int screen_width, int screen_height);
    
    /**
     * Get current layout configuration
     */
    const LayoutConfiguration& GetConfiguration() const { return m_config; }
    
    /**
     * Set layout configuration
     */
    void SetConfiguration(const LayoutConfiguration& config);

private:
    LayoutConfiguration m_config;
    
    void ApplyResponsiveRules(int screen_width, int screen_height);
};

#endif // GOTHAM_SDL2_UI_PERSISTENT_LAYOUT_H