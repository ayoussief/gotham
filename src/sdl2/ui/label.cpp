// Copyright (c) 2025 The Gotham Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or https://opensource.org/license/mit/.

#include "label.h"
#include <sstream>
#include <vector>

Label::Label(const std::string& text, const Point& position)
    : m_text(text)
    , m_position(position)
{
}

void Label::Render(Renderer& renderer, TTF_Font* font)
{
    if (!font || m_text.empty()) {
        return;
    }

    if (m_word_wrap && m_max_width > 0) {
        RenderWrappedText(renderer, font);
    } else {
        RenderSingleLine(renderer, font);
    }
}

void Label::SetText(const std::string& text)
{
    m_text = text;
}

void Label::GetTextSize(TTF_Font* font, int& width, int& height)
{
    width = 0;
    height = 0;
    
    if (!font || m_text.empty()) {
        return;
    }

    TTF_SizeText(font, m_text.c_str(), &width, &height);
}

void Label::RenderWrappedText(Renderer& renderer, TTF_Font* font)
{
    // Simple word wrapping implementation
    std::vector<std::string> lines;
    std::istringstream iss(m_text);
    std::string word;
    std::string current_line;
    
    while (iss >> word) {
        std::string test_line = current_line.empty() ? word : current_line + " " + word;
        
        int test_width, test_height;
        TTF_SizeText(font, test_line.c_str(), &test_width, &test_height);
        
        if (test_width <= m_max_width) {
            current_line = test_line;
        } else {
            if (!current_line.empty()) {
                lines.push_back(current_line);
                current_line = word;
            } else {
                // Word is too long, add it anyway
                lines.push_back(word);
            }
        }
    }
    
    if (!current_line.empty()) {
        lines.push_back(current_line);
    }
    
    // Render each line
    int line_height = TTF_FontHeight(font);
    for (size_t i = 0; i < lines.size(); ++i) {
        SDL_Texture* line_texture = renderer.RenderText(lines[i], font, m_color);
        if (line_texture) {
            int line_width, line_h;
            SDL_QueryTexture(line_texture, nullptr, nullptr, &line_width, &line_h);
            
            Point line_pos = GetAlignedPosition(line_width, line_h);
            line_pos.y += static_cast<int>(i) * line_height;
            
            Rect line_rect = {line_pos.x, line_pos.y, line_width, line_h};
            renderer.DrawTexture(line_texture, nullptr, &line_rect);
            SDL_DestroyTexture(line_texture);
        }
    }
}

void Label::RenderSingleLine(Renderer& renderer, TTF_Font* font)
{
    SDL_Texture* text_texture = renderer.RenderText(m_text, font, m_color);
    if (text_texture) {
        int text_width, text_height;
        SDL_QueryTexture(text_texture, nullptr, nullptr, &text_width, &text_height);
        
        Point aligned_pos = GetAlignedPosition(text_width, text_height);
        Rect text_rect = {aligned_pos.x, aligned_pos.y, text_width, text_height};
        
        renderer.DrawTexture(text_texture, nullptr, &text_rect);
        SDL_DestroyTexture(text_texture);
    }
}

Point Label::GetAlignedPosition(int text_width, int text_height) const
{
    Point aligned_pos = m_position;
    
    switch (m_alignment) {
        case TextAlignment::CENTER:
            aligned_pos.x -= text_width / 2;
            break;
        case TextAlignment::RIGHT:
            aligned_pos.x -= text_width;
            break;
        case TextAlignment::LEFT:
        default:
            // No adjustment needed
            break;
    }
    
    return aligned_pos;
}