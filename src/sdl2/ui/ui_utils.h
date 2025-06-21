// Copyright (c) 2025 The Gotham Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or https://opensource.org/license/mit/.

#ifndef GOTHAM_SDL2_UI_UI_UTILS_H
#define GOTHAM_SDL2_UI_UI_UTILS_H

#include "ui_style_guide.h"
#include "layout_manager.h"
#include "../renderer.h"
#include <vector>
#include <memory>

/**
 * UI Utilities for common layout patterns and improvements
 * Provides helper functions for consistent UI implementation
 */
namespace UIUtils {

    /**
     * Create a responsive button layout
     * Automatically adjusts between horizontal and vertical layouts based on screen size
     */
    struct ResponsiveButtonLayout {
        static void Apply(LayoutManager& layout_manager, const Rect& container, 
                         std::vector<LayoutItem>& button_items, int screen_width);
    };

    /**
     * Create consistent form layouts
     */
    struct FormLayoutHelper {
        static std::vector<LayoutItem> CreateFormFields(
            const std::vector<std::string>& field_names,
            const std::vector<std::function<void(const Rect&)>>& callbacks,
            int container_width
        );
    };

    /**
     * Accessibility validation helpers
     */
    struct AccessibilityValidator {
        static bool ValidateMinimumTouchTargets(const std::vector<Rect>& button_bounds);
        static bool ValidateColorContrast(const Color& foreground, const Color& background);
        static void LogAccessibilityWarnings(const std::string& component_name, const Rect& bounds);
    };

    /**
     * Alignment helpers for consistent positioning
     */
    struct AlignmentHelper {
        static Point CenterInContainer(const Rect& container, const Rect& element);
        static Point AlignToGrid(const Point& position, int grid_size = UIStyleGuide::Spacing::SM);
        static Rect ConstrainToContainer(const Rect& element, const Rect& container, 
                                       const Margin& margin = Margin(0));
    };

    /**
     * Animation and transition helpers
     */
    struct AnimationHelper {
        static Color InterpolateColor(const Color& start, const Color& end, float progress);
        static int InterpolateInt(int start, int end, float progress);
        static float EaseInOut(float t);
    };

    /**
     * Layout debugging and validation
     */
    struct LayoutDebugger {
        static void LogLayoutInfo(const std::string& name, const Rect& bounds);
        static void ValidateLayoutConstraints(const std::vector<LayoutItem>& items, 
                                            const Rect& container);
        static void DrawDebugBounds(Renderer& renderer, const Rect& bounds, 
                                  const Color& color = Color(255, 0, 0, 128));
    };

    /**
     * Common UI patterns
     */
    struct CommonPatterns {
        // Create a card-style container with consistent padding and styling
        static Rect CreateCardContainer(const Rect& available_space, 
                                      const Margin& margin = Margin(UIStyleGuide::Spacing::MD));
        
        // Create a header section with title and optional actions
        static std::vector<LayoutItem> CreateHeaderSection(
            const std::string& title,
            const std::vector<std::function<void(const Rect&)>>& action_callbacks = {}
        );
        
        // Create a status panel with consistent styling
        static std::vector<LayoutItem> CreateStatusPanel(
            const std::vector<std::pair<std::string, std::string>>& status_items
        );
    };

    /**
     * Error handling and recovery
     */
    struct ErrorHandler {
        static void HandleLayoutError(const std::string& error_message, 
                                    const std::string& component_name);
        static bool RecoverFromInvalidBounds(Rect& bounds, const Rect& fallback);
        static void LogUIError(const std::string& error, const std::string& context);
    };
}

#endif // GOTHAM_SDL2_UI_UI_UTILS_H