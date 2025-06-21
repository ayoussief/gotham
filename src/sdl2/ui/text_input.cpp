// Copyright (c) 2025 The Gotham Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or https://opensource.org/license/mit/.

#include "text_input.h"
#include <algorithm>
#include <cmath>

TextInput::TextInput(const Rect& bounds, const std::string& placeholder)
    : m_bounds(bounds)
    , m_placeholder(placeholder)
{
}

void TextInput::HandleEvent(const SDL_Event& event)
{
    if (!m_enabled) {
        return;
    }

    switch (event.type) {
        case SDL_MOUSEBUTTONDOWN:
            if (event.button.button == SDL_BUTTON_LEFT) {
                bool clicked_inside = IsPointInBounds(event.button.x, event.button.y);
                SetFocus(clicked_inside);
            }
            break;
            
        case SDL_TEXTINPUT:
            if (m_focused) {
                HandleTextInput(event.text.text);
            }
            break;
            
        case SDL_KEYDOWN:
            if (m_focused) {
                HandleKeyDown(event.key.keysym.sym);
            }
            break;
    }
}

void TextInput::Update(float delta_time)
{
    if (m_focused) {
        m_cursor_blink_time += delta_time;
        if (m_cursor_blink_time >= 1.0f) {
            m_cursor_visible = !m_cursor_visible;
            m_cursor_blink_time = 0.0f;
        }
    }
}

void TextInput::Render(Renderer& renderer, TTF_Font* font)
{
    // Draw background
    renderer.SetDrawColor(m_background_color);
    renderer.FillRect(m_bounds);
    
    // Draw border
    Color border_color = m_focused ? m_focused_border_color : m_border_color;
    if (!m_enabled) {
        border_color.a = 128;
    }
    renderer.SetDrawColor(border_color);
    renderer.DrawRect(m_bounds);
    
    // Draw text or placeholder
    if (font) {
        std::string display_text = GetDisplayText();
        bool show_placeholder = m_text.empty() && !m_placeholder.empty();
        
        if (show_placeholder) {
            display_text = m_placeholder;
        }
        
        if (!display_text.empty()) {
            Color text_color = show_placeholder ? m_placeholder_color : m_text_color;
            if (!m_enabled) {
                text_color.a = 128;
            }
            
            SDL_Texture* text_texture = renderer.RenderText(display_text, font, text_color);
            if (text_texture) {
                int text_w, text_h;
                SDL_QueryTexture(text_texture, nullptr, nullptr, &text_w, &text_h);
                
                // Position text with padding
                int padding = 8;
                Rect text_rect = {
                    m_bounds.x + padding,
                    m_bounds.y + (m_bounds.h - text_h) / 2,
                    std::min(text_w, m_bounds.w - 2 * padding),
                    text_h
                };
                
                renderer.DrawTexture(text_texture, nullptr, &text_rect);
                SDL_DestroyTexture(text_texture);
            }
        }
        
        // Draw cursor
        if (m_focused && m_cursor_visible && !show_placeholder) {
            int cursor_x = m_bounds.x + 8 + GetCursorPixelPosition(font);
            int cursor_y = m_bounds.y + 4;
            int cursor_height = m_bounds.h - 8;
            
            renderer.SetDrawColor(m_cursor_color);
            renderer.DrawLine(Point(cursor_x, cursor_y), Point(cursor_x, cursor_y + cursor_height));
        }
    }
}

void TextInput::SetText(const std::string& text)
{
    m_text = text;
    if (m_text.length() > m_max_length) {
        m_text = m_text.substr(0, m_max_length);
    }
    m_cursor_position = std::min(m_cursor_position, m_text.length());
    
    if (m_on_text_changed) {
        m_on_text_changed(m_text);
    }
}

void TextInput::SetFocus(bool focused)
{
    if (m_focused != focused) {
        m_focused = focused;
        m_cursor_blink_time = 0.0f;
        m_cursor_visible = true;
        
        if (m_focused) {
            SDL_StartTextInput();
        } else {
            SDL_StopTextInput();
        }
    }
}

bool TextInput::IsPointInBounds(int x, int y) const
{
    return x >= m_bounds.x && x < m_bounds.x + m_bounds.w &&
           y >= m_bounds.y && y < m_bounds.y + m_bounds.h;
}

void TextInput::HandleTextInput(const char* text)
{
    if (m_text.length() >= m_max_length) {
        return;
    }
    
    std::string new_text = text;
    
    // Insert text at cursor position
    m_text.insert(m_cursor_position, new_text);
    
    // Truncate if too long
    if (m_text.length() > m_max_length) {
        m_text = m_text.substr(0, m_max_length);
    }
    
    m_cursor_position += new_text.length();
    m_cursor_position = std::min(m_cursor_position, m_text.length());
    
    if (m_on_text_changed) {
        m_on_text_changed(m_text);
    }
}

void TextInput::HandleKeyDown(SDL_Keycode key)
{
    switch (key) {
        case SDLK_BACKSPACE:
            DeleteCharacter(false);
            break;
            
        case SDLK_DELETE:
            DeleteCharacter(true);
            break;
            
        case SDLK_LEFT:
            MoveCursor(-1);
            break;
            
        case SDLK_RIGHT:
            MoveCursor(1);
            break;
            
        case SDLK_HOME:
            m_cursor_position = 0;
            break;
            
        case SDLK_END:
            m_cursor_position = m_text.length();
            break;
            
        case SDLK_RETURN:
        case SDLK_KP_ENTER:
            if (m_on_enter_pressed) {
                m_on_enter_pressed();
            }
            break;
    }
}

void TextInput::MoveCursor(int delta)
{
    if (delta < 0) {
        m_cursor_position = (m_cursor_position > 0) ? m_cursor_position - 1 : 0;
    } else if (delta > 0) {
        m_cursor_position = std::min(m_cursor_position + 1, m_text.length());
    }
}

void TextInput::DeleteCharacter(bool forward)
{
    if (forward) {
        // Delete character after cursor
        if (m_cursor_position < m_text.length()) {
            m_text.erase(m_cursor_position, 1);
        }
    } else {
        // Delete character before cursor (backspace)
        if (m_cursor_position > 0) {
            m_text.erase(m_cursor_position - 1, 1);
            m_cursor_position--;
        }
    }
    
    if (m_on_text_changed) {
        m_on_text_changed(m_text);
    }
}

std::string TextInput::GetDisplayText() const
{
    if (m_password_mode && !m_text.empty()) {
        return std::string(m_text.length(), '*');
    }
    return m_text;
}

int TextInput::GetCursorPixelPosition(TTF_Font* font) const
{
    if (!font || m_cursor_position == 0) {
        return 0;
    }
    
    std::string text_before_cursor = GetDisplayText().substr(0, m_cursor_position);
    int width, height;
    TTF_SizeText(font, text_before_cursor.c_str(), &width, &height);
    return width;
}