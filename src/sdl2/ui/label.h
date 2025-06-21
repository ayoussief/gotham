// Copyright (c) 2025 The Gotham Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or https://opensource.org/license/mit/.

#ifndef GOTHAM_SDL2_UI_LABEL_H
#define GOTHAM_SDL2_UI_LABEL_H

#include "../renderer.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <string>

enum class TextAlignment {
    LEFT,
    CENTER,
    RIGHT
};

/**
 * Label UI component for displaying text
 */
class Label
{
public:
    Label(const std::string& text, const Point& position);
    ~Label() = default;

    /**
     * Render the label
     */
    void Render(Renderer& renderer, TTF_Font* font);

    /**
     * Set label text
     */
    void SetText(const std::string& text);

    /**
     * Get label text
     */
    const std::string& GetText() const { return m_text; }

    /**
     * Set label position
     */
    void SetPosition(const Point& position) { m_position = position; }

    /**
     * Get label position
     */
    const Point& GetPosition() const { return m_position; }

    /**
     * Set text color
     */
    void SetColor(const Color& color) { m_color = color; }

    /**
     * Get text color
     */
    const Color& GetColor() const { return m_color; }

    /**
     * Set text alignment
     */
    void SetAlignment(TextAlignment alignment) { m_alignment = alignment; }

    /**
     * Get text alignment
     */
    TextAlignment GetAlignment() const { return m_alignment; }

    /**
     * Set maximum width for text wrapping
     */
    void SetMaxWidth(int max_width) { m_max_width = max_width; }

    /**
     * Get maximum width
     */
    int GetMaxWidth() const { return m_max_width; }

    /**
     * Set whether text should wrap
     */
    void SetWordWrap(bool wrap) { m_word_wrap = wrap; }

    /**
     * Check if word wrap is enabled
     */
    bool IsWordWrapEnabled() const { return m_word_wrap; }

    /**
     * Get text dimensions
     */
    void GetTextSize(TTF_Font* font, int& width, int& height);

private:
    std::string m_text;
    Point m_position;
    Color m_color{255, 255, 255, 255};
    TextAlignment m_alignment{TextAlignment::LEFT};
    int m_max_width{0};
    bool m_word_wrap{false};

    void RenderWrappedText(Renderer& renderer, TTF_Font* font);
    void RenderSingleLine(Renderer& renderer, TTF_Font* font);
    Point GetAlignedPosition(int text_width, int text_height) const;
};

#endif // GOTHAM_SDL2_UI_LABEL_H