// Copyright (c) 2025 The Gotham Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or https://opensource.org/license/mit/.

#ifndef GOTHAM_SDL2_UI_NAVIGATION_BAR_H
#define GOTHAM_SDL2_UI_NAVIGATION_BAR_H

#include "button.h"
#include "label.h"
#include "panel.h"
#include "navigation_manager.h"
#include "../renderer.h"
#include <memory>
#include <vector>
#include <functional>

/**
 * Modern navigation bar component with breadcrumbs and contextual actions
 * Provides consistent navigation experience across all screens
 */
class NavigationBar
{
public:
    NavigationBar(NavigationManager& nav_manager);
    ~NavigationBar() = default;

    /**
     * Initialize the navigation bar
     */
    bool Initialize(class UIFactory& ui_factory, class LayoutManager& layout_manager);
    
    /**
     * Handle events
     */
    void HandleEvent(const SDL_Event& event);
    
    /**
     * Update the navigation bar
     */
    void Update(float delta_time);
    
    /**
     * Render the navigation bar
     */
    void Render(class Renderer& renderer, class FontManager& font_manager);
    
    /**
     * Set bounds for the navigation bar
     */
    void SetBounds(const Rect& bounds);
    
    /**
     * Update navigation state (call when screen changes)
     */
    void UpdateNavigationState();
    
    /**
     * Set whether to show breadcrumbs
     */
    void SetShowBreadcrumbs(bool show) { m_show_breadcrumbs = show; }
    
    /**
     * Set whether to show back button
     */
    void SetShowBackButton(bool show) { m_show_back_button = show; }
    
    /**
     * Add contextual action button
     */
    void AddContextualAction(const std::string& text, std::function<void()> callback);
    
    /**
     * Clear contextual actions
     */
    void ClearContextualActions();
    
    /**
     * Set navigation callback for custom navigation handling
     */
    void SetNavigationCallback(std::function<void(ScreenType)> callback);

private:
    NavigationManager& m_nav_manager;
    
    // UI Components
    std::unique_ptr<Panel> m_background_panel;
    std::unique_ptr<Button> m_back_button;
    std::unique_ptr<Label> m_breadcrumb_label;
    std::unique_ptr<Label> m_title_label;
    std::vector<std::unique_ptr<Button>> m_contextual_buttons;
    
    // UI Systems
    UIFactory* m_ui_factory{nullptr};
    LayoutManager* m_layout_manager{nullptr};
    
    // State
    Rect m_bounds;
    bool m_initialized{false};
    bool m_show_breadcrumbs{true};
    bool m_show_back_button{true};
    std::function<void(ScreenType)> m_navigation_callback;
    
    // Layout
    void UpdateLayout();
    void CreateComponents();
    void UpdateBreadcrumbs();
    void UpdateBackButton();
    void UpdateTitle();
    
    // Event handlers
    void OnBackButtonClicked();
    void OnBreadcrumbClicked(ScreenType screen_type);
};

/**
 * Navigation context menu for advanced navigation options
 */
class NavigationContextMenu
{
public:
    NavigationContextMenu(NavigationManager& nav_manager);
    
    /**
     * Show context menu at position
     */
    void Show(const Point& position);
    
    /**
     * Hide context menu
     */
    void Hide();
    
    /**
     * Handle events
     */
    void HandleEvent(const SDL_Event& event);
    
    /**
     * Render context menu
     */
    void Render(Renderer& renderer, FontManager& font_manager);
    
    /**
     * Check if context menu is visible
     */
    bool IsVisible() const { return m_visible; }

private:
    NavigationManager& m_nav_manager;
    
    std::unique_ptr<Panel> m_menu_panel;
    std::vector<std::unique_ptr<Button>> m_menu_items;
    
    Point m_position;
    bool m_visible{false};
    
    void CreateMenuItems();
    void UpdateMenuLayout();
};

#endif // GOTHAM_SDL2_UI_NAVIGATION_BAR_H