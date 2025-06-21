// Copyright (c) 2025 The Gotham Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or https://opensource.org/license/mit/.

#ifndef GOTHAM_SDL2_UI_BUTTON_H
#define GOTHAM_SDL2_UI_BUTTON_H

#include "../renderer.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <string>
#include <functional>

enum class ButtonState {
    NORMAL,
    HOVERED,
    PRESSED,
    DISABLED
};

/**
 * Button UI component with Gotham City styling
 */
class Button
{
public:
    Button(const std::string& text, const Rect& bounds);
    ~Button() = default;

    /**
     * Handle SDL events
     */
    void HandleEvent(const SDL_Event& event);

    /**
     * Update button state
     */
    void Update(float delta_time);

    /**
     * Render the button
     */
    void Render(Renderer& renderer, TTF_Font* font);

    /**
     * Set click callback
     */
    void SetOnClick(std::function<void()> callback) { m_on_click = callback; }

    /**
     * Set button text
     */
    void SetText(const std::string& text);

    /**
     * Get button text
     */
    const std::string& GetText() const { return m_text; }

    /**
     * Set button bounds
     */
    void SetBounds(const Rect& bounds) { m_bounds = bounds; }

    /**
     * Get button bounds
     */
    const Rect& GetBounds() const { return m_bounds; }

    /**
     * Set enabled state
     */
    void SetEnabled(bool enabled) { m_enabled = enabled; }

    /**
     * Check if button is enabled
     */
    bool IsEnabled() const { return m_enabled; }

    /**
     * Set button style
     */
    void SetPrimary(bool primary) { m_primary = primary; }

    /**
     * Check if button is primary style
     */
    bool IsPrimary() const { return m_primary; }

    /**
     * Set button colors
     */
    void SetColors(const Color& normal, const Color& hovered, const Color& pressed);

    /**
     * Set text color
     */
    void SetTextColor(const Color& color) { m_text_color = color; }

private:
    std::string m_text;
    Rect m_bounds;
    ButtonState m_state{ButtonState::NORMAL};
    bool m_enabled{true};
    bool m_primary{false};
    
    Color m_normal_color{70, 130, 180, 255};      // Steel blue
    Color m_hovered_color{100, 149, 237, 255};   // Cornflower blue
    Color m_pressed_color{25, 25, 112, 255};     // Midnight blue
    Color m_disabled_color{128, 128, 128, 255};  // Gray
    Color m_text_color{255, 255, 255, 255};      // White
    
    std::function<void()> m_on_click;
    
    bool m_mouse_over{false};
    bool m_mouse_down{false};
    float m_animation_time{0.0f};

    bool IsPointInBounds(int x, int y) const;
    Color GetCurrentBackgroundColor() const;
    void HandleMouseMotion(int x, int y);
    void HandleMouseButton(const SDL_MouseButtonEvent& event);
};

#endif // GOTHAM_SDL2_UI_BUTTON_H