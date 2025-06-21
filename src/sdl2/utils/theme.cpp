// Copyright (c) 2025 The Gotham Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or https://opensource.org/license/mit/.

#include "theme.h"
#include <iostream>

ThemeManager::ThemeManager() = default;

bool ThemeManager::Initialize()
{
    InitializeColors();
    InitializeFonts();
    InitializeSpacing();
    
    std::cout << "Theme manager initialized with " << (m_dark_mode ? "dark" : "light") << " theme" << std::endl;
    return true;
}

Color ThemeManager::GetColor(ThemeColor color_type) const
{
    auto it = m_colors.find(color_type);
    if (it != m_colors.end()) {
        return it->second;
    }
    
    // Default to white if color not found
    return Color(255, 255, 255, 255);
}

int ThemeManager::GetFontSize(ThemeFont font_type) const
{
    auto it = m_font_sizes.find(font_type);
    if (it != m_font_sizes.end()) {
        return it->second;
    }
    
    // Default font size
    return 16;
}

std::string ThemeManager::GetFontName(ThemeFont font_type) const
{
    auto it = m_font_names.find(font_type);
    if (it != m_font_names.end()) {
        return it->second;
    }
    
    // Default font
    return "arial.ttf";
}

int ThemeManager::GetSpacing(const std::string& spacing_type) const
{
    auto it = m_spacing.find(spacing_type);
    if (it != m_spacing.end()) {
        return it->second;
    }
    
    // Default spacing
    return 8;
}

void ThemeManager::SetDarkMode(bool dark_mode)
{
    if (m_dark_mode != dark_mode) {
        m_dark_mode = dark_mode;
        InitializeColors(); // Reinitialize colors for new theme
        std::cout << "Theme switched to " << (m_dark_mode ? "dark" : "light") << " mode" << std::endl;
    }
}

void ThemeManager::InitializeColors()
{
    if (m_dark_mode) {
        // Dark theme - Gotham City night aesthetic
        m_colors[ThemeColor::BACKGROUND] = Color(15, 15, 20, 255);        // Very dark blue-black
        m_colors[ThemeColor::SURFACE] = Color(25, 25, 35, 255);           // Dark surface
        m_colors[ThemeColor::PRIMARY] = Color(255, 215, 0, 255);          // Gotham gold
        m_colors[ThemeColor::SECONDARY] = Color(70, 130, 180, 255);       // Steel blue
        m_colors[ThemeColor::ACCENT] = Color(255, 140, 0, 255);           // Dark orange
        m_colors[ThemeColor::TEXT_PRIMARY] = Color(240, 240, 245, 255);   // Light text
        m_colors[ThemeColor::TEXT_SECONDARY] = Color(180, 180, 190, 255); // Muted text
        m_colors[ThemeColor::TEXT_DISABLED] = Color(120, 120, 130, 255);  // Disabled text
        m_colors[ThemeColor::SUCCESS] = Color(46, 160, 67, 255);          // Green
        m_colors[ThemeColor::WARNING] = Color(255, 193, 7, 255);          // Yellow
        m_colors[ThemeColor::ERROR] = Color(220, 53, 69, 255);            // Red
        m_colors[ThemeColor::BORDER] = Color(60, 60, 75, 255);            // Border color
        m_colors[ThemeColor::SHADOW] = Color(0, 0, 0, 100);               // Shadow
        m_colors[ThemeColor::HIGHLIGHT] = Color(255, 215, 0, 50);         // Highlight overlay
        m_colors[ThemeColor::GOTHAM_GOLD] = Color(255, 215, 0, 255);      // Signature gold
        m_colors[ThemeColor::GOTHAM_DARK_BLUE] = Color(25, 25, 112, 255); // Midnight blue
        m_colors[ThemeColor::GOTHAM_LIGHT_BLUE] = Color(135, 206, 235, 255); // Sky blue
    } else {
        // Light theme - Daytime Gotham
        m_colors[ThemeColor::BACKGROUND] = Color(248, 249, 250, 255);     // Light gray
        m_colors[ThemeColor::SURFACE] = Color(255, 255, 255, 255);        // White
        m_colors[ThemeColor::PRIMARY] = Color(0, 123, 255, 255);          // Blue
        m_colors[ThemeColor::SECONDARY] = Color(108, 117, 125, 255);      // Gray
        m_colors[ThemeColor::ACCENT] = Color(255, 193, 7, 255);           // Yellow
        m_colors[ThemeColor::TEXT_PRIMARY] = Color(33, 37, 41, 255);      // Dark text
        m_colors[ThemeColor::TEXT_SECONDARY] = Color(108, 117, 125, 255); // Gray text
        m_colors[ThemeColor::TEXT_DISABLED] = Color(173, 181, 189, 255);  // Light gray text
        m_colors[ThemeColor::SUCCESS] = Color(40, 167, 69, 255);          // Green
        m_colors[ThemeColor::WARNING] = Color(255, 193, 7, 255);          // Yellow
        m_colors[ThemeColor::ERROR] = Color(220, 53, 69, 255);            // Red
        m_colors[ThemeColor::BORDER] = Color(222, 226, 230, 255);         // Light border
        m_colors[ThemeColor::SHADOW] = Color(0, 0, 0, 50);                // Light shadow
        m_colors[ThemeColor::HIGHLIGHT] = Color(0, 123, 255, 50);         // Blue highlight
        m_colors[ThemeColor::GOTHAM_GOLD] = Color(255, 193, 7, 255);      // Gold accent
        m_colors[ThemeColor::GOTHAM_DARK_BLUE] = Color(0, 123, 255, 255); // Primary blue
        m_colors[ThemeColor::GOTHAM_LIGHT_BLUE] = Color(173, 216, 230, 255); // Light blue
    }
}

void ThemeManager::InitializeFonts()
{
    // Font sizes
    m_font_sizes[ThemeFont::TITLE] = 32;
    m_font_sizes[ThemeFont::HEADING] = 24;
    m_font_sizes[ThemeFont::BODY] = 16;
    m_font_sizes[ThemeFont::CAPTION] = 12;
    m_font_sizes[ThemeFont::MONOSPACE] = 14;

    // Font names (would be actual font files in real implementation)
    m_font_names[ThemeFont::TITLE] = "gotham-bold.ttf";
    m_font_names[ThemeFont::HEADING] = "gotham-medium.ttf";
    m_font_names[ThemeFont::BODY] = "gotham-regular.ttf";
    m_font_names[ThemeFont::CAPTION] = "gotham-light.ttf";
    m_font_names[ThemeFont::MONOSPACE] = "courier-new.ttf";
}

void ThemeManager::InitializeSpacing()
{
    m_spacing["xs"] = 4;
    m_spacing["sm"] = 8;
    m_spacing["md"] = 16;
    m_spacing["lg"] = 24;
    m_spacing["xl"] = 32;
    m_spacing["xxl"] = 48;
}