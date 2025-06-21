// Copyright (c) 2025 The Gotham Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or https://opensource.org/license/mit/.

#ifndef GOTHAM_SDL2_UTILS_FONT_MANAGER_H
#define GOTHAM_SDL2_UTILS_FONT_MANAGER_H

#include <SDL2/SDL_ttf.h>
#include <string>
#include <unordered_map>
#include <memory>

class Renderer;

/**
 * Manages font loading and caching
 */
class FontManager
{
public:
    explicit FontManager(Renderer& renderer);
    ~FontManager();

    /**
     * Initialize the font manager
     */
    bool Initialize();

    /**
     * Load a font
     * @param font_path Path to the font file
     * @param size Font size
     * @return Font pointer or nullptr if failed
     */
    TTF_Font* LoadFont(const std::string& font_path, int size);

    /**
     * Get a cached font
     * @param font_path Path to the font file
     * @param size Font size
     * @return Font pointer or nullptr if not found
     */
    TTF_Font* GetFont(const std::string& font_path, int size);

    /**
     * Get default font
     */
    TTF_Font* GetDefaultFont(int size = 16);

    /**
     * Unload all fonts
     */
    void UnloadAllFonts();

private:
    Renderer& m_renderer;
    std::unordered_map<std::string, TTF_Font*> m_fonts;
    std::string m_default_font_path;

    std::string MakeFontKey(const std::string& font_path, int size) const;
    std::string FindSystemFont(const std::string& font_name) const;
};

#endif // GOTHAM_SDL2_UTILS_FONT_MANAGER_H// Copyright (c) 2025 The Gotham Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or https://opensource.org/license/mit/.

#ifndef GOTHAM_SDL2_UTILS_FONT_MANAGER_H
#define GOTHAM_SDL2_UTILS_FONT_MANAGER_H

#include <SDL2/SDL_ttf.h>
#include <string>
#include <unordered_map>
#include <memory>

class Renderer;

/**
 * Manages font loading and caching
 */
class FontManager
{
public:
    explicit FontManager(Renderer& renderer);
    ~FontManager();

    /**
     * Initialize the font manager
     */
    bool Initialize();

    /**
     * Load a font
     * @param font_path Path to the font file
     * @param size Font size
     * @return Font pointer or nullptr if failed
     */
    TTF_Font* LoadFont(const std::string& font_path, int size);

    /**
     * Get a cached font
     * @param font_path Path to the font file
     * @param size Font size
     * @return Font pointer or nullptr if not found
     */
    TTF_Font* GetFont(const std::string& font_path, int size);

    /**
     * Get default font
     */
    TTF_Font* GetDefaultFont(int size = 16);

    /**
     * Unload all fonts
     */
    void UnloadAllFonts();

private:
    Renderer& m_renderer;
    std::unordered_map<std::string, TTF_Font*> m_fonts;
    std::string m_default_font_path;

    std::string MakeFontKey(const std::string& font_path, int size) const;
    std::string FindSystemFont(const std::string& font_name) const;
};

#endif // GOTHAM_SDL2_UTILS_FONT_MANAGER_H