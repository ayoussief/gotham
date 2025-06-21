// Copyright (c) 2025 The Gotham Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or https://opensource.org/license/mit/.

#ifndef GOTHAM_SDL2_UI_TEXT_INPUT_H
#define GOTHAM_SDL2_UI_TEXT_INPUT_H

#include "../renderer.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <string>
#include <functional>

/**
 * Text input UI component
 */
class TextInput
{
public:
    TextInput(const Rect& bounds, const std::string& placeholder = "");
    ~TextInput() = default;

    /**
     * Handle SDL events
     */
    void HandleEvent(const SDL_Event& event);

    /**
     * Update text input state
     */
    void Update(float delta_time);

    /**
     * Render the text input
     */
    void Render(Renderer& renderer, TTF_Font* font);

    /**
     * Set text content
     */
    void SetText(const std::string& text);

    /**
     * Get text content
     */
    const std::string& GetText() const { return m_text; }

    /**
     * Set placeholder text
     */
    void SetPlaceholder(const std::string& placeholder) { m_placeholder = placeholder; }

    /**
     * Get placeholder text
     */
    const std::string& GetPlaceholder() const { return m_placeholder; }

    /**
     * Set bounds
     */
    void SetBounds(const Rect& bounds) { m_bounds = bounds; }

    /**
     * Get bounds
     */
    const Rect& GetBounds() const { return m_bounds; }

    /**
     * Set focus
     */
    void SetFocus(bool focused);

    /**
     * Check if focused
     */
    bool IsFocused() const { return m_focused; }

    /**
     * Set enabled state
     */
    void SetEnabled(bool enabled) { m_enabled = enabled; }

    /**
     * Check if enabled
     */
    bool IsEnabled() const { return m_enabled; }

    /**
     * Set password mode (hide text)
     */
    void SetPasswordMode(bool password) { m_password_mode = password; }

    /**
     * Check if in password mode
     */
    bool IsPasswordMode() const { return m_password_mode; }

    /**
     * Set maximum text length
     */
    void SetMaxLength(size_t max_length) { m_max_length = max_length; }

    /**
     * Get maximum text length
     */
    size_t GetMaxLength() const { return m_max_length; }

    /**
     * Set text change callback
     */
    void SetOnTextChanged(std::function<void(const std::string&)> callback) { m_on_text_changed = callback; }

    /**
     * Set enter key callback
     */
    void SetOnEnterPressed(std::function<void()> callback) { m_on_enter_pressed = callback; }

private:
    std::string m_text;
    std::string m_placeholder;
    Rect m_bounds;
    bool m_focused{false};
    bool m_enabled{true};
    bool m_password_mode{false};
    size_t m_max_length{256};
    
    // Cursor
    size_t m_cursor_position{0};
    float m_cursor_blink_time{0.0f};
    bool m_cursor_visible{true};
    
    // Colors
    Color m_background_color{40, 40, 50, 255};
    Color m_border_color{70, 130, 180, 255};
    Color m_focused_border_color{255, 215, 0, 255};
    Color m_text_color{255, 255, 255, 255};
    Color m_placeholder_color{150, 150, 150, 255};
    Color m_cursor_color{255, 215, 0, 255};
    
    // Callbacks
    std::function<void(const std::string&)> m_on_text_changed;
    std::function<void()> m_on_enter_pressed;
    
    bool IsPointInBounds(int x, int y) const;
    void HandleTextInput(const char* text);
    void HandleKeyDown(SDL_Keycode key);
    void MoveCursor(int delta);
    void DeleteCharacter(bool forward = true);
    std::string GetDisplayText() const;
    int GetCursorPixelPosition(TTF_Font* font) const;
};

#endif // GOTHAM_SDL2_UI_TEXT_INPUT_H