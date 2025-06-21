// Copyright (c) 2025 The Gotham Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or https://opensource.org/license/mit/.

#ifndef GOTHAM_SDL2_UI_NAVIGATION_MANAGER_H
#define GOTHAM_SDL2_UI_NAVIGATION_MANAGER_H

#include <vector>
#include <stack>
#include <unordered_map>
#include <string>
#include <functional>
#include <memory>

// Forward declarations
enum class ScreenType;
class Screen;

/**
 * Navigation context for maintaining state between screens
 */
struct NavigationContext {
    ScreenType from_screen;
    ScreenType to_screen;
    std::unordered_map<std::string, std::string> parameters;
    std::unordered_map<std::string, std::string> state_data;
    
    // Helper methods
    void SetParameter(const std::string& key, const std::string& value) {
        parameters[key] = value;
    }
    
    std::string GetParameter(const std::string& key, const std::string& default_value = "") const {
        auto it = parameters.find(key);
        return (it != parameters.end()) ? it->second : default_value;
    }
    
    bool HasParameter(const std::string& key) const {
        return parameters.find(key) != parameters.end();
    }
    
    void SetState(const std::string& key, const std::string& value) {
        state_data[key] = value;
    }
    
    std::string GetState(const std::string& key, const std::string& default_value = "") const {
        auto it = state_data.find(key);
        return (it != state_data.end()) ? it->second : default_value;
    }
};

/**
 * Navigation entry for history tracking
 */
struct NavigationEntry {
    ScreenType screen_type;
    NavigationContext context;
    std::string title;
    bool can_go_back{true};
    
    NavigationEntry(ScreenType type, const std::string& screen_title = "")
        : screen_type(type), title(screen_title) {}
};

/**
 * Modern navigation manager for connected app experience
 * Provides navigation stack, state persistence, and contextual navigation
 */
class NavigationManager
{
public:
    NavigationManager();
    ~NavigationManager() = default;

    /**
     * Navigate to a screen with context
     */
    void NavigateTo(ScreenType screen_type, const NavigationContext& context = {});
    
    /**
     * Navigate back in history
     */
    bool NavigateBack();
    
    /**
     * Navigate forward in history (if available)
     */
    bool NavigateForward();
    
    /**
     * Replace current screen (no history entry)
     */
    void ReplaceScreen(ScreenType screen_type, const NavigationContext& context = {});
    
    /**
     * Clear navigation history
     */
    void ClearHistory();
    
    /**
     * Get current navigation context
     */
    const NavigationContext& GetCurrentContext() const { return m_current_context; }
    
    /**
     * Get navigation history for breadcrumbs
     */
    std::vector<NavigationEntry> GetBreadcrumbs() const;
    
    /**
     * Check if can navigate back
     */
    bool CanGoBack() const;
    
    /**
     * Check if can navigate forward
     */
    bool CanGoForward() const;
    
    /**
     * Get current screen type
     */
    ScreenType GetCurrentScreen() const;
    
    /**
     * Get previous screen type
     */
    ScreenType GetPreviousScreen() const;
    
    /**
     * Set navigation callback for screen switching
     */
    void SetNavigationCallback(std::function<void(ScreenType, const NavigationContext&)> callback);
    
    /**
     * Get screen title for display
     */
    std::string GetScreenTitle(ScreenType screen_type) const;
    
    /**
     * Set custom screen title
     */
    void SetScreenTitle(ScreenType screen_type, const std::string& title);
    
    /**
     * Navigation shortcuts for common patterns
     */
    void NavigateToWalletFromMain();
    void NavigateToSendFromWallet(const std::string& address = "");
    void NavigateToReceiveFromWallet();
    void NavigateToTransactionsFromWallet();
    void NavigateToSettingsFromMain();
    void NavigateBackToWallet();
    void NavigateBackToMain();
    
    /**
     * State persistence helpers
     */
    void SaveScreenState(ScreenType screen_type, const std::string& key, const std::string& value);
    std::string GetScreenState(ScreenType screen_type, const std::string& key, const std::string& default_value = "") const;
    void ClearScreenState(ScreenType screen_type);

private:
    std::vector<NavigationEntry> m_history;
    int m_current_index{-1};
    NavigationContext m_current_context;
    std::function<void(ScreenType, const NavigationContext&)> m_navigation_callback;
    
    // Screen titles
    std::unordered_map<ScreenType, std::string> m_screen_titles;
    
    // Persistent state storage
    std::unordered_map<ScreenType, std::unordered_map<std::string, std::string>> m_screen_states;
    
    void InitializeScreenTitles();
    void AddToHistory(ScreenType screen_type, const NavigationContext& context);
    void TriggerNavigation(ScreenType screen_type, const NavigationContext& context);
};

/**
 * Navigation breadcrumb component for UI display
 */
struct NavigationBreadcrumb {
    std::string title;
    ScreenType screen_type;
    bool is_current{false};
    bool is_clickable{true};
};

/**
 * Helper class for building navigation breadcrumbs UI
 */
class BreadcrumbBuilder
{
public:
    static std::vector<NavigationBreadcrumb> BuildBreadcrumbs(const NavigationManager& nav_manager);
    static std::string BuildBreadcrumbText(const NavigationManager& nav_manager, const std::string& separator = " > ");
};

#endif // GOTHAM_SDL2_UI_NAVIGATION_MANAGER_H