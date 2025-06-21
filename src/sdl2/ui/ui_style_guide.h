// Copyright (c) 2025 The Gotham Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or https://opensource.org/license/mit/.

#ifndef GOTHAM_SDL2_UI_UI_STYLE_GUIDE_H
#define GOTHAM_SDL2_UI_UI_STYLE_GUIDE_H

#include "../renderer.h"
#include "../utils/theme.h"

/**
 * UI Style Guide for Gotham City
 * 
 * This class provides constants and guidelines for consistent UI design
 * across all screens and components in the Gotham City application.
 * 
 * Design Philosophy:
 * - Dark theme inspired by Batman/Gotham City aesthetics
 * - Gold accents (Gotham Gold) for primary actions and highlights
 * - Steel blue for secondary elements
 * - Consistent spacing and typography
 * - Responsive layout support
 */
namespace UIStyleGuide {

    // SPACING CONSTANTS
    namespace Spacing {
        constexpr int XS = 4;   // Extra small spacing
        constexpr int SM = 8;   // Small spacing
        constexpr int MD = 16;  // Medium spacing (default)
        constexpr int LG = 24;  // Large spacing
        constexpr int XL = 32;  // Extra large spacing
        constexpr int XXL = 48; // Extra extra large spacing
    }

    // COMPONENT DIMENSIONS
    namespace Dimensions {
        constexpr int BUTTON_HEIGHT = 40;
        constexpr int INPUT_HEIGHT = 36;
        constexpr int HEADER_HEIGHT = 80;
        constexpr int STATUS_BAR_HEIGHT = 30;
        constexpr int SIDEBAR_WIDTH = 200;
        constexpr int SIDEBAR_BUTTON_HEIGHT = 50;
        constexpr int MODAL_MIN_WIDTH = 400;
        constexpr int MODAL_MIN_HEIGHT = 300;
    }

    // BORDER AND RADIUS
    namespace Border {
        constexpr int RADIUS = 8;
        constexpr int WIDTH_THIN = 1;
        constexpr int WIDTH_MEDIUM = 2;
        constexpr int WIDTH_THICK = 3;
        constexpr int SHADOW_OFFSET = 4;
    }

    // FONT SIZES
    namespace FontSize {
        constexpr int TITLE = 32;
        constexpr int HEADING = 24;
        constexpr int BODY = 16;
        constexpr int CAPTION = 12;
        constexpr int MONOSPACE = 14;
    }

    // LAYOUT BREAKPOINTS
    namespace Breakpoints {
        constexpr int MOBILE = 768;
        constexpr int TABLET = 1024;
        constexpr int DESKTOP = 1440;
    }

    // ANIMATION TIMING
    namespace Animation {
        constexpr float FAST = 0.15f;     // 150ms
        constexpr float NORMAL = 0.25f;   // 250ms
        constexpr float SLOW = 0.35f;     // 350ms
    }

    // COLOR PALETTE (complementing ThemeManager)
    namespace Colors {
        // Gotham City Signature Colors
        inline const Color GOTHAM_GOLD = Color(255, 215, 0, 255);
        inline const Color GOTHAM_DARK_BLUE = Color(25, 25, 112, 255);
        inline const Color GOTHAM_LIGHT_BLUE = Color(135, 206, 235, 255);
        
        // Dark Theme Base
        inline const Color BACKGROUND_DARK = Color(15, 15, 20, 255);
        inline const Color SURFACE_DARK = Color(25, 25, 35, 255);
        inline const Color BORDER_DARK = Color(60, 60, 75, 255);
        
        // Status Colors
        inline const Color SUCCESS = Color(46, 160, 67, 255);
        inline const Color WARNING = Color(255, 193, 7, 255);
        inline const Color ERROR = Color(220, 53, 69, 255);
        inline const Color INFO = Color(70, 130, 180, 255);
        
        // Text Colors
        inline const Color TEXT_PRIMARY = Color(240, 240, 245, 255);
        inline const Color TEXT_SECONDARY = Color(180, 180, 190, 255);
        inline const Color TEXT_DISABLED = Color(120, 120, 130, 255);
        
        // Transparency Levels
        constexpr Uint8 ALPHA_FULL = 255;
        constexpr Uint8 ALPHA_HIGH = 200;
        constexpr Uint8 ALPHA_MEDIUM = 150;
        constexpr Uint8 ALPHA_LOW = 100;
        constexpr Uint8 ALPHA_MINIMAL = 50;
    }

    // COMPONENT STYLE PRESETS
    namespace Presets {
        
        // Button Styles
        struct ButtonStyle {
            Color normal_color;
            Color hovered_color;
            Color pressed_color;
            Color text_color;
            int border_radius;
            int border_width;
        };

        inline const ButtonStyle PRIMARY_BUTTON = {
            Colors::GOTHAM_GOLD,
            Color(255, 235, 50, 255),  // Lighter gold
            Colors::GOTHAM_DARK_BLUE,
            Colors::BACKGROUND_DARK,
            Border::RADIUS,
            Border::WIDTH_THIN
        };

        inline const ButtonStyle SECONDARY_BUTTON = {
            Colors::INFO,
            Colors::GOTHAM_LIGHT_BLUE,
            Colors::GOTHAM_DARK_BLUE,
            Colors::TEXT_PRIMARY,
            Border::RADIUS,
            Border::WIDTH_THIN
        };

        inline const ButtonStyle SUCCESS_BUTTON = {
            Colors::SUCCESS,
            Color(56, 180, 77, 255),
            Color(36, 140, 57, 255),
            Color(255, 255, 255, 255),
            Border::RADIUS,
            Border::WIDTH_THIN
        };

        inline const ButtonStyle WARNING_BUTTON = {
            Colors::WARNING,
            Color(255, 213, 47, 255),
            Color(235, 173, 0, 255),
            Colors::BACKGROUND_DARK,
            Border::RADIUS,
            Border::WIDTH_THIN
        };

        inline const ButtonStyle ERROR_BUTTON = {
            Colors::ERROR,
            Color(240, 73, 89, 255),
            Color(200, 33, 49, 255),
            Color(255, 255, 255, 255),
            Border::RADIUS,
            Border::WIDTH_THIN
        };

        // Panel Styles
        struct PanelStyle {
            Color background_color;
            Color border_color;
            int border_width;
            Uint8 alpha;
        };

        inline const PanelStyle HEADER_PANEL = {
            Colors::SURFACE_DARK,
            Colors::GOTHAM_GOLD,
            Border::WIDTH_MEDIUM,
            Colors::ALPHA_FULL
        };

        inline const PanelStyle SIDEBAR_PANEL = {
            Color(20, 20, 30, 255),
            Colors::BORDER_DARK,
            Border::WIDTH_THIN,
            Colors::ALPHA_FULL
        };

        inline const PanelStyle MAIN_PANEL = {
            Colors::BACKGROUND_DARK,
            Color(0, 0, 0, 0), // Transparent border
            0,
            Colors::ALPHA_FULL
        };

        inline const PanelStyle STATUS_PANEL = {
            Color(30, 30, 40, 255),
            Colors::BORDER_DARK,
            Border::WIDTH_THIN,
            Colors::ALPHA_FULL
        };

        inline const PanelStyle CARD_PANEL = {
            Colors::SURFACE_DARK,
            Colors::BORDER_DARK,
            Border::WIDTH_THIN,
            Colors::ALPHA_FULL
        };

        inline const PanelStyle MODAL_PANEL = {
            Colors::SURFACE_DARK,
            Colors::GOTHAM_GOLD,
            Border::WIDTH_MEDIUM,
            Colors::ALPHA_HIGH
        };
    }

    // LAYOUT GUIDELINES
    namespace Layout {
        
        // Standard screen layout areas
        struct ScreenLayout {
            static Rect GetHeaderArea(int screen_width, int screen_height) {
                return Rect(0, 0, screen_width, Dimensions::HEADER_HEIGHT);
            }
            
            static Rect GetSidebarArea(int screen_width, int screen_height) {
                return Rect(0, Dimensions::HEADER_HEIGHT, 
                           Dimensions::SIDEBAR_WIDTH, 
                           screen_height - Dimensions::HEADER_HEIGHT - Dimensions::STATUS_BAR_HEIGHT);
            }
            
            static Rect GetMainContentArea(int screen_width, int screen_height) {
                return Rect(Dimensions::SIDEBAR_WIDTH, Dimensions::HEADER_HEIGHT,
                           screen_width - Dimensions::SIDEBAR_WIDTH,
                           screen_height - Dimensions::HEADER_HEIGHT - Dimensions::STATUS_BAR_HEIGHT);
            }
            
            static Rect GetStatusBarArea(int screen_width, int screen_height) {
                return Rect(0, screen_height - Dimensions::STATUS_BAR_HEIGHT,
                           screen_width, Dimensions::STATUS_BAR_HEIGHT);
            }
            
            static Rect GetModalArea(int screen_width, int screen_height, int modal_width, int modal_height) {
                int x = (screen_width - modal_width) / 2;
                int y = (screen_height - modal_height) / 2;
                return Rect(x, y, modal_width, modal_height);
            }
        };

        // Form layout helpers
        struct FormLayout {
            static constexpr int LABEL_WIDTH = 120;
            static constexpr int FIELD_HEIGHT = 40;
            static constexpr int FIELD_SPACING = 16;
            
            static Rect GetLabelRect(int x, int y) {
                return Rect(x, y, LABEL_WIDTH, FIELD_HEIGHT);
            }
            
            static Rect GetInputRect(int x, int y, int width) {
                return Rect(x + LABEL_WIDTH + Spacing::SM, y, width - LABEL_WIDTH - Spacing::SM, Dimensions::INPUT_HEIGHT);
            }
        };
    }

    // ACCESSIBILITY GUIDELINES
    namespace Accessibility {
        constexpr int MIN_TOUCH_TARGET = 44;  // Minimum touch target size
        constexpr float MIN_CONTRAST_RATIO = 4.5f;  // WCAG AA standard
        constexpr int FOCUS_OUTLINE_WIDTH = 2;
        inline const Color FOCUS_COLOR = Colors::GOTHAM_GOLD;
    }

    // RESPONSIVE DESIGN HELPERS
    namespace Responsive {
        
        enum class ScreenSize {
            MOBILE,
            TABLET,
            DESKTOP
        };
        
        static ScreenSize GetScreenSize(int width) {
            if (width < Breakpoints::MOBILE) return ScreenSize::MOBILE;
            if (width < Breakpoints::TABLET) return ScreenSize::TABLET;
            return ScreenSize::DESKTOP;
        }
        
        static int GetSidebarWidth(ScreenSize size) {
            switch (size) {
                case ScreenSize::MOBILE: return 0;  // Hidden on mobile
                case ScreenSize::TABLET: return 150;
                case ScreenSize::DESKTOP: return Dimensions::SIDEBAR_WIDTH;
            }
            return Dimensions::SIDEBAR_WIDTH;
        }
        
        static int GetSpacing(ScreenSize size) {
            switch (size) {
                case ScreenSize::MOBILE: return Spacing::SM;
                case ScreenSize::TABLET: return Spacing::MD;
                case ScreenSize::DESKTOP: return Spacing::LG;
            }
            return Spacing::MD;
        }
    }
}

#endif // GOTHAM_SDL2_UI_UI_STYLE_GUIDE_H