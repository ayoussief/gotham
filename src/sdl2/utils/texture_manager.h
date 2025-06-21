// Copyright (c) 2025 The Gotham Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or https://opensource.org/license/mit/.

#ifndef GOTHAM_SDL2_UTILS_TEXTURE_MANAGER_H
#define GOTHAM_SDL2_UTILS_TEXTURE_MANAGER_H

#include <SDL2/SDL.h>
#include <string>
#include <unordered_map>
#include <memory>

class Renderer;

/**
 * Manages texture loading and caching
 */
class TextureManager
{
public:
    explicit TextureManager(Renderer& renderer);
    ~TextureManager();

    /**
     * Initialize the texture manager
     */
    bool Initialize();

    /**
     * Load a texture from file
     * @param file_path Path to the image file
     * @return Texture pointer or nullptr if failed
     */
    SDL_Texture* LoadTexture(const std::string& file_path);

    /**
     * Get a cached texture
     * @param file_path Path to the image file
     * @return Texture pointer or nullptr if not found
     */
    SDL_Texture* GetTexture(const std::string& file_path);

    /**
     * Create a solid color texture
     * @param width Texture width
     * @param height Texture height
     * @param r Red component (0-255)
     * @param g Green component (0-255)
     * @param b Blue component (0-255)
     * @param a Alpha component (0-255)
     * @return Texture pointer or nullptr if failed
     */
    SDL_Texture* CreateSolidTexture(int width, int height, Uint8 r, Uint8 g, Uint8 b, Uint8 a = 255);

    /**
     * Create a gradient texture
     * @param width Texture width
     * @param height Texture height
     * @param start_color Starting color (RGBA)
     * @param end_color Ending color (RGBA)
     * @param vertical True for vertical gradient, false for horizontal
     * @return Texture pointer or nullptr if failed
     */
    SDL_Texture* CreateGradientTexture(int width, int height, 
                                       Uint32 start_color, Uint32 end_color, 
                                       bool vertical = true);

    /**
     * Unload a specific texture
     * @param file_path Path to the texture file
     */
    void UnloadTexture(const std::string& file_path);

    /**
     * Unload all textures
     */
    void UnloadAllTextures();

    /**
     * Get texture dimensions
     * @param texture Texture to query
     * @param width Output width
     * @param height Output height
     * @return true if successful
     */
    bool GetTextureDimensions(SDL_Texture* texture, int& width, int& height);

private:
    Renderer& m_renderer;
    std::unordered_map<std::string, SDL_Texture*> m_textures;
    int m_next_generated_id{0};

    std::string GenerateTextureKey();
};

#endif // GOTHAM_SDL2_UTILS_TEXTURE_MANAGER_H// Copyright (c) 2025 The Gotham Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or https://opensource.org/license/mit/.

#ifndef GOTHAM_SDL2_UTILS_TEXTURE_MANAGER_H
#define GOTHAM_SDL2_UTILS_TEXTURE_MANAGER_H

#include <SDL2/SDL.h>
#include <string>
#include <unordered_map>
#include <memory>

class Renderer;

/**
 * Manages texture loading and caching
 */
class TextureManager
{
public:
    explicit TextureManager(Renderer& renderer);
    ~TextureManager();

    /**
     * Initialize the texture manager
     */
    bool Initialize();

    /**
     * Load a texture from file
     * @param file_path Path to the image file
     * @return Texture pointer or nullptr if failed
     */
    SDL_Texture* LoadTexture(const std::string& file_path);

    /**
     * Get a cached texture
     * @param file_path Path to the image file
     * @return Texture pointer or nullptr if not found
     */
    SDL_Texture* GetTexture(const std::string& file_path);

    /**
     * Create a solid color texture
     * @param width Texture width
     * @param height Texture height
     * @param r Red component (0-255)
     * @param g Green component (0-255)
     * @param b Blue component (0-255)
     * @param a Alpha component (0-255)
     * @return Texture pointer or nullptr if failed
     */
    SDL_Texture* CreateSolidTexture(int width, int height, Uint8 r, Uint8 g, Uint8 b, Uint8 a = 255);

    /**
     * Create a gradient texture
     * @param width Texture width
     * @param height Texture height
     * @param start_color Starting color (RGBA)
     * @param end_color Ending color (RGBA)
     * @param vertical True for vertical gradient, false for horizontal
     * @return Texture pointer or nullptr if failed
     */
    SDL_Texture* CreateGradientTexture(int width, int height, 
                                       Uint32 start_color, Uint32 end_color, 
                                       bool vertical = true);

    /**
     * Unload a specific texture
     * @param file_path Path to the texture file
     */
    void UnloadTexture(const std::string& file_path);

    /**
     * Unload all textures
     */
    void UnloadAllTextures();

    /**
     * Get texture dimensions
     * @param texture Texture to query
     * @param width Output width
     * @param height Output height
     * @return true if successful
     */
    bool GetTextureDimensions(SDL_Texture* texture, int& width, int& height);

private:
    Renderer& m_renderer;
    std::unordered_map<std::string, SDL_Texture*> m_textures;
    int m_next_generated_id{0};

    std::string GenerateTextureKey();
};

#endif // GOTHAM_SDL2_UTILS_TEXTURE_MANAGER_H