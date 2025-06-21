// Copyright (c) 2025 The Gotham Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or https://opensource.org/license/mit/.

#include "renderer.h"
#include "window_manager.h"
#include <iostream>

Renderer::Renderer(WindowManager& window_manager)
    : m_window_manager(window_manager)
{
}

Renderer::~Renderer()
{
    if (m_renderer) {
        SDL_DestroyRenderer(m_renderer);
        m_renderer = nullptr;
    }
}

bool Renderer::Initialize()
{
    SDL_Window* window = m_window_manager.GetWindow();
    if (!window) {
        std::cerr << "Cannot create renderer: window is null" << std::endl;
        return false;
    }

    // Create renderer with hardware acceleration and VSync
    m_renderer = SDL_CreateRenderer(window, -1, 
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    if (!m_renderer) {
        std::cerr << "Renderer could not be created! SDL Error: " << SDL_GetError() << std::endl;
        return false;
    }

    // Set renderer blend mode for alpha blending
    SDL_SetRenderDrawBlendMode(m_renderer, SDL_BLENDMODE_BLEND);

    std::cout << "Renderer initialized successfully" << std::endl;
    return true;
}

void Renderer::Clear(const Color& color)
{
    SetDrawColor(color);
    SDL_RenderClear(m_renderer);
}

void Renderer::Present()
{
    SDL_RenderPresent(m_renderer);
}

void Renderer::SetDrawColor(const Color& color)
{
    SDL_SetRenderDrawColor(m_renderer, color.r, color.g, color.b, color.a);
}

void Renderer::FillRect(const Rect& rect)
{
    SDL_Rect sdl_rect = {rect.x, rect.y, rect.w, rect.h};
    SDL_RenderFillRect(m_renderer, &sdl_rect);
}

void Renderer::DrawRect(const Rect& rect)
{
    SDL_Rect sdl_rect = {rect.x, rect.y, rect.w, rect.h};
    SDL_RenderDrawRect(m_renderer, &sdl_rect);
}

void Renderer::DrawLine(const Point& start, const Point& end)
{
    SDL_RenderDrawLine(m_renderer, start.x, start.y, end.x, end.y);
}

void Renderer::DrawPoint(const Point& point)
{
    SDL_RenderDrawPoint(m_renderer, point.x, point.y);
}

SDL_Texture* Renderer::RenderText(const std::string& text, TTF_Font* font, const Color& color)
{
    if (!font) {
        return nullptr;
    }

    SDL_Color sdl_color = {color.r, color.g, color.b, color.a};
    SDL_Surface* text_surface = TTF_RenderText_Blended(font, text.c_str(), sdl_color);
    
    if (!text_surface) {
        std::cerr << "Unable to render text surface! SDL_ttf Error: " << TTF_GetError() << std::endl;
        return nullptr;
    }

    SDL_Texture* texture = CreateTextureFromSurface(text_surface);
    SDL_FreeSurface(text_surface);

    return texture;
}

void Renderer::DrawTexture(SDL_Texture* texture, const Rect* src_rect, const Rect* dst_rect)
{
    if (!texture) {
        return;
    }

    SDL_Rect* src = nullptr;
    SDL_Rect* dst = nullptr;
    SDL_Rect src_sdl, dst_sdl;

    if (src_rect) {
        src_sdl = {src_rect->x, src_rect->y, src_rect->w, src_rect->h};
        src = &src_sdl;
    }

    if (dst_rect) {
        dst_sdl = {dst_rect->x, dst_rect->y, dst_rect->w, dst_rect->h};
        dst = &dst_sdl;
    }

    SDL_RenderCopy(m_renderer, texture, src, dst);
}

void Renderer::DrawTextureEx(SDL_Texture* texture, const Rect* src_rect, const Rect* dst_rect,
                             double angle, const Point* center, SDL_RendererFlip flip)
{
    if (!texture) {
        return;
    }

    SDL_Rect* src = nullptr;
    SDL_Rect* dst = nullptr;
    SDL_Point* center_point = nullptr;
    SDL_Rect src_sdl, dst_sdl;
    SDL_Point center_sdl;

    if (src_rect) {
        src_sdl = {src_rect->x, src_rect->y, src_rect->w, src_rect->h};
        src = &src_sdl;
    }

    if (dst_rect) {
        dst_sdl = {dst_rect->x, dst_rect->y, dst_rect->w, dst_rect->h};
        dst = &dst_sdl;
    }

    if (center) {
        center_sdl = {center->x, center->y};
        center_point = &center_sdl;
    }

    SDL_RenderCopyEx(m_renderer, texture, src, dst, angle, center_point, flip);
}

void Renderer::SetTextureBlendMode(SDL_Texture* texture, SDL_BlendMode blend_mode)
{
    if (texture) {
        SDL_SetTextureBlendMode(texture, blend_mode);
    }
}

void Renderer::SetTextureAlphaMod(SDL_Texture* texture, Uint8 alpha)
{
    if (texture) {
        SDL_SetTextureAlphaMod(texture, alpha);
    }
}

void Renderer::SetTextureColorMod(SDL_Texture* texture, Uint8 r, Uint8 g, Uint8 b)
{
    if (texture) {
        SDL_SetTextureColorMod(texture, r, g, b);
    }
}

SDL_Texture* Renderer::CreateTextureFromSurface(SDL_Surface* surface)
{
    if (!surface) {
        return nullptr;
    }

    return SDL_CreateTextureFromSurface(m_renderer, surface);
}

int Renderer::GetWidth() const
{
    return m_window_manager.GetWidth();
}

int Renderer::GetHeight() const
{
    return m_window_manager.GetHeight();
}

void Renderer::DrawText(const std::string& text, int x, int y, const Color& color, int font_size)
{
    // Create a simple text surface using SDL_ttf
    // This is a basic implementation - a full app would cache textures
    
    // For now, create a simple visual representation
    SetDrawColor(color);
    
    // Draw text background
    Rect bg_rect(x - 2, y - 2, static_cast<int>(text.length() * font_size * 0.6) + 4, font_size + 8);
    SetDrawColor(Color(0, 0, 0, 100)); // Semi-transparent background
    FillRect(bg_rect);
    
    // Draw text border
    SetDrawColor(color);
    DrawRect(bg_rect);
    
    // Draw character blocks to represent text
    int char_width = font_size * 0.6;
    for (size_t i = 0; i < text.length() && i < 50; ++i) { // Limit to prevent overflow
        if (text[i] != ' ') {
            Rect char_rect(x + i * char_width, y + 2, char_width - 1, font_size);
            FillRect(char_rect);
        }
    }
}

void Renderer::GetTextSize(const std::string& text, int font_size, int& width, int& height)
{
    // Simple estimation - real implementation would use TTF_SizeText
    width = static_cast<int>(text.length() * font_size * 0.6);
    height = font_size + 4;
}

void Renderer::HandleResize(int new_width, int new_height)
{
    // Update renderer viewport if needed
    // SDL automatically handles most resize operations
    std::cout << "Renderer handling resize to: " << new_width << "x" << new_height << std::endl;
}