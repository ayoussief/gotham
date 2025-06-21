// Copyright (c) 2025 The Gotham Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or https://opensource.org/license/mit/.

#include "button.h"
#include <algorithm>
#include <cmath>

Button::Button(const std::string& text, const Rect& bounds)
    : m_text(text)
    , m_bounds(bounds)
{
}

void Button::HandleEvent(const SDL_Event& event)
{
    if (!m_enabled) {
        return;
    }

    switch (event.type) {
        case SDL_MOUSEMOTION:
            HandleMouseMotion(event.motion.x, event.motion.y);
            break;
            
        case SDL_MOUSEBUTTONDOWN:
        case SDL_MOUSEBUTTONUP:
            HandleMouseButton(event.button);
            break;
    }
}

void Button::Update(float delta_time)
{
    m_animation_time += delta_time;
    
    // Update button state based on mouse interaction
    if (!m_enabled) {
        m_state = ButtonState::DISABLED;
    } else if (m_mouse_down && m_mouse_over) {
        m_state = ButtonState::PRESSED;
    } else if (m_mouse_over) {
        m_state = ButtonState::HOVERED;
    } else {
        m_state = ButtonState::NORMAL;
    }
}

void Button::Render(Renderer& renderer, TTF_Font* font)
{
    // Get current background color
    Color bg_color = GetCurrentBackgroundColor();
    
    // Add subtle animation effect
    float pulse = std::sin(m_animation_time * 2.0f) * 0.1f + 0.9f;
    if (m_state == ButtonState::HOVERED) {
        bg_color.r = std::min(255, static_cast<int>(bg_color.r * pulse));
        bg_color.g = std::min(255, static_cast<int>(bg_color.g * pulse));
        bg_color.b = std::min(255, static_cast<int>(bg_color.b * pulse));
    }
    
    // Draw button background with rounded corners effect
    renderer.SetDrawColor(bg_color);
    renderer.FillRect(m_bounds);
    
    // Draw border
    Color border_color = m_primary ? Color(255, 215, 0, 255) : Color(100, 100, 100, 255);
    if (m_state == ButtonState::PRESSED) {
        border_color.a = 200;
    }
    renderer.SetDrawColor(border_color);
    renderer.DrawRect(m_bounds);
    
    // Draw inner highlight for 3D effect
    if (m_state != ButtonState::PRESSED) {
        Rect highlight_rect = {m_bounds.x + 1, m_bounds.y + 1, m_bounds.w - 2, 2};
        Color highlight_color = bg_color;
        highlight_color.r = std::min(255, highlight_color.r + 40);
        highlight_color.g = std::min(255, highlight_color.g + 40);
        highlight_color.b = std::min(255, highlight_color.b + 40);
        renderer.SetDrawColor(highlight_color);
        renderer.FillRect(highlight_rect);
    }
    
    // Render text
    if (font && !m_text.empty()) {
        Color text_color = m_text_color;
        if (m_state == ButtonState::DISABLED) {
            text_color.a = 128;
        }
        
        SDL_Texture* text_texture = renderer.RenderText(m_text, font, text_color);
        if (text_texture) {
            int text_w, text_h;
            SDL_QueryTexture(text_texture, nullptr, nullptr, &text_w, &text_h);
            
            // Center text in button
            Rect text_rect = {
                m_bounds.x + (m_bounds.w - text_w) / 2,
                m_bounds.y + (m_bounds.h - text_h) / 2,
                text_w,
                text_h
            };
            
            // Offset text slightly when pressed
            if (m_state == ButtonState::PRESSED) {
                text_rect.x += 1;
                text_rect.y += 1;
            }
            
            renderer.DrawTexture(text_texture, nullptr, &text_rect);
            SDL_DestroyTexture(text_texture);
        }
    }
}

void Button::SetText(const std::string& text)
{
    m_text = text;
}

void Button::SetColors(const Color& normal, const Color& hovered, const Color& pressed)
{
    m_normal_color = normal;
    m_hovered_color = hovered;
    m_pressed_color = pressed;
}

bool Button::IsPointInBounds(int x, int y) const
{
    return x >= m_bounds.x && x < m_bounds.x + m_bounds.w &&
           y >= m_bounds.y && y < m_bounds.y + m_bounds.h;
}

Color Button::GetCurrentBackgroundColor() const
{
    switch (m_state) {
        case ButtonState::HOVERED:
            return m_hovered_color;
        case ButtonState::PRESSED:
            return m_pressed_color;
        case ButtonState::DISABLED:
            return m_disabled_color;
        default:
            return m_normal_color;
    }
}

void Button::HandleMouseMotion(int x, int y)
{
    bool was_over = m_mouse_over;
    m_mouse_over = IsPointInBounds(x, y);
    
    // Reset animation when mouse enters/leaves
    if (was_over != m_mouse_over) {
        m_animation_time = 0.0f;
    }
}

void Button::HandleMouseButton(const SDL_MouseButtonEvent& event)
{
    if (event.button != SDL_BUTTON_LEFT) {
        return;
    }
    
    bool in_bounds = IsPointInBounds(event.x, event.y);
    
    if (event.type == SDL_MOUSEBUTTONDOWN) {
        if (in_bounds) {
            m_mouse_down = true;
        }
    } else if (event.type == SDL_MOUSEBUTTONUP) {
        if (m_mouse_down && in_bounds && m_on_click) {
            m_on_click();
        }
        m_mouse_down = false;
    }
}