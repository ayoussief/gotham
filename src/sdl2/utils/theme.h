// Copyright (c) 2025 The Gotham Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or https://opensource.org/license/mit/.

#ifndef GOTHAM_SDL2_UTILS_THEME_H
#define GOTHAM_SDL2_UTILS_THEME_H

#include "../renderer.h"
#include <unordered_map>
#include <string>

enum class ThemeColor {
    BACKGROUND,
    SURFACE,
    PRIMARY,
    SECONDARY,
    ACCENT,
    TEXT_PRIMARY,
    TEXT_SECONDARY,
    TEXT_DISABLED,
    SUCCESS,
    WARNING,
    ERROR,
    BORDER,
    SHADOW,
    HIGHLIGHT,
    GOTHAM_GOLD,
    GOTHAM_DARK_BLUE,
    GOTHAM_LIGHT_BLUE
};

enum class ThemeFont {
    TITLE,
    HEADING,
    BODY,
    CAPTION,
    MONOSPACE
};

/**
 * Manages the visual theme for Gotham City
 * Inspired by Batman/Gotham City aesthetics
 */
class ThemeManager
{
public:
    ThemeManager();
    ~ThemeManager() = default;

    /**
     * Initialize the theme
     */
    bool Initialize();

    /**
     * Get color by theme color type
     */
    Color GetColor(ThemeColor color_type) const;

    /**
     * Get font size for theme font type
     */
    int GetFontSize(ThemeFont font_type) const;

    /**
     * Get font name for theme font type
     */
    std::string GetFontName(ThemeFont font_type) const;

    /**
     * Get spacing value
     */
    int GetSpacing(const std::string& spacing_type) const;

    /**
     * Get border radius
     */
    int GetBorderRadius() const { return m_border_radius; }

    /**
     * Get shadow offset
     */
    int GetShadowOffset() const { return m_shadow_offset; }

    /**
     * Switch between light and dark themes
     */
    void SetDarkMode(bool dark_mode);

    /**
     * Check if dark mode is enabled
     */
    bool IsDarkMode() const { return m_dark_mode; }

private:
    std::unordered_map<ThemeColor, Color> m_colors;
    std::unordered_map<ThemeFont, int> m_font_sizes;
    std::unordered_map<ThemeFont, std::string> m_font_names;
    std::unordered_map<std::string, int> m_spacing;
    
    bool m_dark_mode{true};
    int m_border_radius{8};
    int m_shadow_offset{4};

    void InitializeColors();
    void InitializeFonts();
    void InitializeSpacing();
};

#endif // GOTHAM_SDL2_UTILS_THEME_H// Copyright (c) 2025 The Gotham Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or https://opensource.org/license/mit/.

#ifndef GOTHAM_SDL2_UTILS_THEME_H
#define GOTHAM_SDL2_UTILS_THEME_H

#include "../renderer.h"
#include <unordered_map>
#include <string>

enum class ThemeColor {
    BACKGROUND,
    SURFACE,
    PRIMARY,
    SECONDARY,
    ACCENT,
    TEXT_PRIMARY,
    TEXT_SECONDARY,
    TEXT_DISABLED,
    SUCCESS,
    WARNING,
    ERROR,
    BORDER,
    SHADOW,
    HIGHLIGHT,
    GOTHAM_GOLD,
    GOTHAM_DARK_BLUE,
    GOTHAM_LIGHT_BLUE
};

enum class ThemeFont {
    TITLE,
    HEADING,
    BODY,
    CAPTION,
    MONOSPACE
};

/**
 * Manages the visual theme for Gotham City
 * Inspired by Batman/Gotham City aesthetics
 */
class ThemeManager
{
public:
    ThemeManager();
    ~ThemeManager() = default;

    /**
     * Initialize the theme
     */
    bool Initialize();

    /**
     * Get color by theme color type
     */
    Color GetColor(ThemeColor color_type) const;

    /**
     * Get font size for theme font type
     */
    int GetFontSize(ThemeFont font_type) const;

    /**
     * Get font name for theme font type
     */
    std::string GetFontName(ThemeFont font_type) const;

    /**
     * Get spacing value
     */
    int GetSpacing(const std::string& spacing_type) const;

    /**
     * Get border radius
     */
    int GetBorderRadius() const { return m_border_radius; }

    /**
     * Get shadow offset
     */
    int GetShadowOffset() const { return m_shadow_offset; }

    /**
     * Switch between light and dark themes
     */
    void SetDarkMode(bool dark_mode);

    /**
     * Check if dark mode is enabled
     */
    bool IsDarkMode() const { return m_dark_mode; }

private:
    std::unordered_map<ThemeColor, Color> m_colors;
    std::unordered_map<ThemeFont, int> m_font_sizes;
    std::unordered_map<ThemeFont, std::string> m_font_names;
    std::unordered_map<std::string, int> m_spacing;
    
    bool m_dark_mode{true};
    int m_border_radius{8};
    int m_shadow_offset{4};

    void InitializeColors();
    void InitializeFonts();
    void InitializeSpacing();
};

#endif // GOTHAM_SDL2_UTILS_THEME_H