// Copyright (c) 2025 The Gotham Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or https://opensource.org/license/mit/.

#ifndef GOTHAM_SDL2_UI_LAYOUT_MANAGER_H
#define GOTHAM_SDL2_UI_LAYOUT_MANAGER_H

#include "../renderer.h"
#include <vector>
#include <functional>

enum class LayoutType {
    VERTICAL,
    HORIZONTAL,
    GRID,
    STACK,
    ABSOLUTE
};

enum class Alignment {
    START,
    CENTER,
    END,
    STRETCH
};

struct LayoutConstraints {
    int min_width{0};
    int min_height{0};
    int max_width{-1}; // -1 means no limit
    int max_height{-1};
    int preferred_width{-1};
    int preferred_height{-1};
    float weight{1.0f}; // For flexible layouts
    
    LayoutConstraints() = default;
    LayoutConstraints(int min_w, int min_h, int max_w = -1, int max_h = -1)
        : min_width(min_w), min_height(min_h), max_width(max_w), max_height(max_h) {}
};

struct Padding {
    int top, right, bottom, left;
    
    Padding(int all = 0) : top(all), right(all), bottom(all), left(all) {}
    Padding(int vertical, int horizontal) : top(vertical), right(horizontal), bottom(vertical), left(horizontal) {}
    Padding(int t, int r, int b, int l) : top(t), right(r), bottom(b), left(l) {}
};

struct Margin {
    int top, right, bottom, left;
    
    Margin(int all = 0) : top(all), right(all), bottom(all), left(all) {}
    Margin(int vertical, int horizontal) : top(vertical), right(horizontal), bottom(vertical), left(horizontal) {}
    Margin(int t, int r, int b, int l) : top(t), right(r), bottom(b), left(l) {}
};

/**
 * Layout item that can be positioned by the layout manager
 */
struct LayoutItem {
    Rect bounds;
    LayoutConstraints constraints;
    Margin margin;
    Padding padding;
    bool visible{true};
    std::function<void(const Rect&)> on_bounds_changed;
    
    LayoutItem() = default;
    LayoutItem(const Rect& initial_bounds) : bounds(initial_bounds) {}
};

/**
 * Manages layout and positioning of UI components
 * Provides responsive and consistent layouts
 */
class LayoutManager
{
public:
    LayoutManager();
    ~LayoutManager() = default;

    /**
     * Create a vertical layout
     */
    void CreateVerticalLayout(const Rect& container, std::vector<LayoutItem>& items, 
                            int spacing = 8, Alignment alignment = Alignment::STRETCH);

    /**
     * Create a horizontal layout
     */
    void CreateHorizontalLayout(const Rect& container, std::vector<LayoutItem>& items, 
                              int spacing = 8, Alignment alignment = Alignment::STRETCH);

    /**
     * Create a grid layout
     */
    void CreateGridLayout(const Rect& container, std::vector<LayoutItem>& items, 
                        int columns, int spacing = 8);

    /**
     * Create a form layout (labels and inputs)
     */
    void CreateFormLayout(const Rect& container, std::vector<LayoutItem>& items, 
                        int label_width = 120, int spacing = 16);

    /**
     * Create a header layout (title, actions, etc.)
     */
    void CreateHeaderLayout(const Rect& container, std::vector<LayoutItem>& items);

    /**
     * Create a sidebar layout (navigation buttons)
     */
    void CreateSidebarLayout(const Rect& container, std::vector<LayoutItem>& items, int spacing = 8);

    /**
     * Create a status bar layout
     */
    void CreateStatusBarLayout(const Rect& container, std::vector<LayoutItem>& items);

    /**
     * Create a stack layout (overlapping elements)
     */
    void CreateStackLayout(const Rect& container, std::vector<LayoutItem>& items, 
                          Alignment horizontal_align = Alignment::CENTER, 
                          Alignment vertical_align = Alignment::CENTER);

    /**
     * Create a card layout (content with padding and margins)
     */
    void CreateCardLayout(const Rect& container, std::vector<LayoutItem>& items, 
                         const Padding& padding = Padding(16));

    /**
     * Create a responsive content layout that fills the available space
     */
    void CreateContentLayout(const Rect& container, std::vector<LayoutItem>& items, 
                           int max_width = 800, Alignment alignment = Alignment::START);

    /**
     * Calculate preferred size for a container with given items
     */
    Rect CalculatePreferredSize(const std::vector<LayoutItem>& items, LayoutType layout_type, 
                              int spacing = 8, int columns = 1);

    /**
     * Apply responsive breakpoints
     */
    void ApplyResponsiveLayout(const Rect& container, std::vector<LayoutItem>& items, 
                             int screen_width, int screen_height);

    /**
     * Standard layout presets
     */
    struct StandardLayouts {
        static Rect GetHeaderBounds(int screen_width, int screen_height);
        static Rect GetSidebarBounds(int screen_width, int screen_height);
        static Rect GetMainContentBounds(int screen_width, int screen_height);
        static Rect GetStatusBarBounds(int screen_width, int screen_height);
        static Rect GetModalBounds(int screen_width, int screen_height, int modal_width, int modal_height);
    };

private:
    void DistributeSpace(std::vector<LayoutItem>& items, int available_space, bool horizontal);
    void ApplyAlignment(std::vector<LayoutItem>& items, const Rect& container, 
                       Alignment alignment, bool horizontal);
    int CalculateRequiredSpace(const std::vector<LayoutItem>& items, int spacing, bool horizontal);
    void ApplyConstraints(LayoutItem& item);
};

#endif // GOTHAM_SDL2_UI_LAYOUT_MANAGER_H