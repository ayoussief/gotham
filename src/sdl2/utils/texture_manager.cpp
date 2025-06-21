// Copyright (c) 2025 The Gotham Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or https://opensource.org/license/mit/.

#include "texture_manager.h"
#include "../renderer.h"
#include <SDL2/SDL_image.h>
#include <iostream>
#include <filesystem>

TextureManager::TextureManager(Renderer& renderer)
    : m_renderer(renderer)
{
}

TextureManager::~TextureManager()
{
    UnloadAllTextures();
}

bool TextureManager::Initialize()
{
    std::cout << "Texture manager initialized" << std::endl;
    return true;
}

SDL_Texture* TextureManager::LoadTexture(const std::string& file_path)
{
    // Check if already loaded
    auto it = m_textures.find(file_path);
    if (it != m_textures.end()) {
        return it->second;
    }

    // Check if file exists
    if (!std::filesystem::exists(file_path)) {
        std::cerr << "Texture file not found: " << file_path << std::endl;
        return nullptr;
    }

    // Load the image
    SDL_Surface* surface = IMG_Load(file_path.c_str());
    if (!surface) {
        std::cerr << "Failed to load image " << file_path << ": " << IMG_GetError() << std::endl;
        return nullptr;
    }

    // Create texture from surface
    SDL_Texture* texture = m_renderer.CreateTextureFromSurface(surface);
    SDL_FreeSurface(surface);

    if (!texture) {
        std::cerr << "Failed to create texture from " << file_path << ": " << SDL_GetError() << std::endl;
        return nullptr;
    }

    // Cache the texture
    m_textures[file_path] = texture;
    std::cout << "Loaded texture: " << file_path << std::endl;
    
    return texture;
}

SDL_Texture* TextureManager::GetTexture(const std::string& file_path)
{
    auto it = m_textures.find(file_path);
    return (it != m_textures.end()) ? it->second : nullptr;
}

SDL_Texture* TextureManager::CreateSolidTexture(int width, int height, Uint8 r, Uint8 g, Uint8 b, Uint8 a)
{
    // Create a surface with the specified color
    SDL_Surface* surface = SDL_CreateRGBSurface(0, width, height, 32, 0, 0, 0, 0);
    if (!surface) {
        std::cerr << "Failed to create surface for solid texture: " << SDL_GetError() << std::endl;
        return nullptr;
    }

    // Fill the surface with the color
    Uint32 color = SDL_MapRGBA(surface->format, r, g, b, a);
    SDL_FillRect(surface, nullptr, color);

    // Create texture from surface
    SDL_Texture* texture = m_renderer.CreateTextureFromSurface(surface);
    SDL_FreeSurface(surface);

    if (!texture) {
        std::cerr << "Failed to create solid texture: " << SDL_GetError() << std::endl;
        return nullptr;
    }

    // Set blend mode for alpha
    SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);

    // Cache with generated key
    std::string key = GenerateTextureKey();
    m_textures[key] = texture;

    return texture;
}

SDL_Texture* TextureManager::CreateGradientTexture(int width, int height, 
                                                   Uint32 start_color, Uint32 end_color, 
                                                   bool vertical)
{
    // Create a surface for the gradient
    SDL_Surface* surface = SDL_CreateRGBSurface(0, width, height, 32, 0, 0, 0, 0);
    if (!surface) {
        std::cerr << "Failed to create surface for gradient texture: " << SDL_GetError() << std::endl;
        return nullptr;
    }

    // Extract color components
    Uint8 start_r = (start_color >> 24) & 0xFF;
    Uint8 start_g = (start_color >> 16) & 0xFF;
    Uint8 start_b = (start_color >> 8) & 0xFF;
    Uint8 start_a = start_color & 0xFF;

    Uint8 end_r = (end_color >> 24) & 0xFF;
    Uint8 end_g = (end_color >> 16) & 0xFF;
    Uint8 end_b = (end_color >> 8) & 0xFF;
    Uint8 end_a = end_color & 0xFF;

    // Lock surface for pixel manipulation
    if (SDL_LockSurface(surface) < 0) {
        std::cerr << "Failed to lock surface: " << SDL_GetError() << std::endl;
        SDL_FreeSurface(surface);
        return nullptr;
    }

    Uint32* pixels = static_cast<Uint32*>(surface->pixels);
    
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            float t = vertical ? (float)y / (height - 1) : (float)x / (width - 1);
            
            Uint8 r = start_r + t * (end_r - start_r);
            Uint8 g = start_g + t * (end_g - start_g);
            Uint8 b = start_b + t * (end_b - start_b);
            Uint8 a = start_a + t * (end_a - start_a);
            
            pixels[y * width + x] = SDL_MapRGBA(surface->format, r, g, b, a);
        }
    }

    SDL_UnlockSurface(surface);

    // Create texture from surface
    SDL_Texture* texture = m_renderer.CreateTextureFromSurface(surface);
    SDL_FreeSurface(surface);

    if (!texture) {
        std::cerr << "Failed to create gradient texture: " << SDL_GetError() << std::endl;
        return nullptr;
    }

    // Set blend mode for alpha
    SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);

    // Cache with generated key
    std::string key = GenerateTextureKey();
    m_textures[key] = texture;

    return texture;
}

void TextureManager::UnloadTexture(const std::string& file_path)
{
    auto it = m_textures.find(file_path);
    if (it != m_textures.end()) {
        SDL_DestroyTexture(it->second);
        m_textures.erase(it);
        std::cout << "Unloaded texture: " << file_path << std::endl;
    }
}

void TextureManager::UnloadAllTextures()
{
    for (auto& pair : m_textures) {
        if (pair.second) {
            SDL_DestroyTexture(pair.second);
        }
    }
    m_textures.clear();
    std::cout << "All textures unloaded" << std::endl;
}

bool TextureManager::GetTextureDimensions(SDL_Texture* texture, int& width, int& height)
{
    if (!texture) {
        return false;
    }

    return SDL_QueryTexture(texture, nullptr, nullptr, &width, &height) == 0;
}

std::string TextureManager::GenerateTextureKey()
{
    return "generated_" + std::to_string(m_next_generated_id++);
}