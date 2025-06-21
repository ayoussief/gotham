// Copyright (c) 2025 The Gotham Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or https://opensource.org/license/mit/.

#ifndef GOTHAM_SDL2_RENDERER_H
#define GOTHAM_SDL2_RENDERER_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <string>
#include <memory>

class WindowManager;

struct Color {
    Uint8 r, g, b, a;
    
    Color(Uint8 red = 0, Uint8 green = 0, Uint8 blue = 0, Uint8 alpha = 255)
        : r(red), g(green), b(blue), a(alpha) {}
};

struct Rect {
    int x, y, w, h;
    
    Rect(int x = 0, int y = 0, int w = 0, int h = 0)
        : x(x), y(y), w(w), h(h) {}
};

struct Point {
    int x, y;
    
    Point(int x = 0, int y = 0) : x(x), y(y) {}
};

/**
 * Handles all rendering operations
 */
class Renderer
{
public:
    explicit Renderer(WindowManager& window_manager);
    ~Renderer();

    /**
     * Initialize the renderer
     */
    bool Initialize();

    /**
     * Clear the screen with specified color
     */
    void Clear(const Color& color = Color(0, 0, 0, 255));

    /**
     * Present the rendered frame
     */
    void Present();

    /**
     * Set draw color
     */
    void SetDrawColor(const Color& color);

    /**
     * Draw a filled rectangle
     */
    void FillRect(const Rect& rect);

    /**
     * Draw a rectangle outline
     */
    void DrawRect(const Rect& rect);

    /**
     * Draw a line
     */
    void DrawLine(const Point& start, const Point& end);

    /**
     * Draw a point
     */
    void DrawPoint(const Point& point);

    /**
     * Render text
     */
    SDL_Texture* RenderText(const std::string& text, TTF_Font* font, const Color& color);

    /**
     * Draw texture
     */
    void DrawTexture(SDL_Texture* texture, const Rect* src_rect = nullptr, const Rect* dst_rect = nullptr);

    /**
     * Draw texture with rotation and center point
     */
    void DrawTextureEx(SDL_Texture* texture, const Rect* src_rect, const Rect* dst_rect, 
                       double angle, const Point* center = nullptr, SDL_RendererFlip flip = SDL_FLIP_NONE);

    /**
     * Set texture blend mode
     */
    void SetTextureBlendMode(SDL_Texture* texture, SDL_BlendMode blend_mode);

    /**
     * Set texture alpha modulation
     */
    void SetTextureAlphaMod(SDL_Texture* texture, Uint8 alpha);

    /**
     * Set texture color modulation
     */
    void SetTextureColorMod(SDL_Texture* texture, Uint8 r, Uint8 g, Uint8 b);

    /**
     * Create texture from surface
     */
    SDL_Texture* CreateTextureFromSurface(SDL_Surface* surface);

    /**
     * Get SDL renderer
     */
    SDL_Renderer* GetSDLRenderer() const { return m_renderer; }

    /**
     * Get window dimensions
     */
    int GetWidth() const;
    int GetHeight() const;

    /**
     * Draw text directly to screen with font size
     */
    void DrawText(const std::string& text, int x, int y, const Color& color, int font_size = 16);

    /**
     * Get text dimensions
     */
    void GetTextSize(const std::string& text, int font_size, int& width, int& height);

    /**
     * Handle window resize
     */
    void HandleResize(int new_width, int new_height);

private:
    WindowManager& m_window_manager;
    SDL_Renderer* m_renderer{nullptr};
};

#endif // GOTHAM_SDL2_RENDERER_H