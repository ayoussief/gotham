// Copyright (c) 2025 The Gotham Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or https://opensource.org/license/mit/.

#include "font_manager.h"
#include "../renderer.h"
#include <iostream>
#include <filesystem>
#include <vector>

FontManager::FontManager(Renderer& renderer)
    : m_renderer(renderer)
{
}

FontManager::~FontManager()
{
    UnloadAllFonts();
}

bool FontManager::Initialize()
{
    // Try to find a default system font
    std::vector<std::string> font_candidates = {
        "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",
        "/usr/share/fonts/TTF/DejaVuSans.ttf",
        "/System/Library/Fonts/Arial.ttf",
        "/Windows/Fonts/arial.ttf",
        "C:/Windows/Fonts/arial.ttf",
        "./fonts/arial.ttf",
        "./arial.ttf"
    };

    for (const auto& font_path : font_candidates) {
        if (std::filesystem::exists(font_path)) {
            m_default_font_path = font_path;
            std::cout << "Default font found: " << font_path << std::endl;
            break;
        }
    }

    if (m_default_font_path.empty()) {
        std::cerr << "Warning: No default font found. Text rendering may fail." << std::endl;
        // Set a fallback path anyway
        m_default_font_path = "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf";
    }

    return true;
}

TTF_Font* FontManager::LoadFont(const std::string& font_path, int size)
{
    std::string key = MakeFontKey(font_path, size);
    
    // Check if already loaded
    auto it = m_fonts.find(key);
    if (it != m_fonts.end()) {
        return it->second;
    }

    // Try to load the font
    TTF_Font* font = TTF_OpenFont(font_path.c_str(), size);
    if (!font) {
        std::cerr << "Failed to load font " << font_path << " size " << size 
                  << ": " << TTF_GetError() << std::endl;
        return nullptr;
    }

    // Cache the font
    m_fonts[key] = font;
    std::cout << "Loaded font: " << font_path << " size " << size << std::endl;
    
    return font;
}

TTF_Font* FontManager::GetFont(const std::string& font_path, int size)
{
    std::string key = MakeFontKey(font_path, size);
    auto it = m_fonts.find(key);
    return (it != m_fonts.end()) ? it->second : nullptr;
}

TTF_Font* FontManager::GetDefaultFont(int size)
{
    if (m_default_font_path.empty()) {
        return nullptr;
    }

    TTF_Font* font = GetFont(m_default_font_path, size);
    if (!font) {
        font = LoadFont(m_default_font_path, size);
    }
    
    return font;
}

void FontManager::UnloadAllFonts()
{
    for (auto& pair : m_fonts) {
        if (pair.second) {
            TTF_CloseFont(pair.second);
        }
    }
    m_fonts.clear();
    std::cout << "All fonts unloaded" << std::endl;
}

std::string FontManager::MakeFontKey(const std::string& font_path, int size) const
{
    return font_path + "_" + std::to_string(size);
}

std::string FontManager::FindSystemFont(const std::string& font_name) const
{
    // This would implement system-specific font discovery
    // For now, return empty string
    return "";
}