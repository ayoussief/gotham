// Copyright (c) 2025 The Gotham Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or https://opensource.org/license/mit/.

#include "progress_bar.h"
#include <algorithm>
#include <cmath>
#include <sstream>
#include <iomanip>

ProgressBar::ProgressBar(const Rect& bounds)
    : m_bounds(bounds)
{
}

void ProgressBar::Update(float delta_time)
{
    m_animation_time += delta_time;
    
    if (m_animated) {
        // Smooth progress animation
        float diff = m_progress - m_displayed_progress;
        if (std::abs(diff) > 0.001f) {
            m_displayed_progress += diff * delta_time * 5.0f; // Animation speed
            m_displayed_progress = std::clamp(m_displayed_progress, 0.0f, 1.0f);
        }
    } else {
        m_displayed_progress = m_progress;
    }
}

void ProgressBar::Render(Renderer& renderer, TTF_Font* font)
{
    RenderBackground(renderer);
    RenderFill(renderer);
    RenderBorder(renderer);
    
    if (font) {
        RenderText(renderer, font);
    }
}

void ProgressBar::SetProgress(float progress)
{
    m_progress = std::clamp(progress, 0.0f, 1.0f);
}

void ProgressBar::SetColors(const Color& background, const Color& fill, const Color& border)
{
    m_background_color = background;
    m_fill_color = fill;
    m_border_color = border;
}

void ProgressBar::RenderBackground(Renderer& renderer)
{
    renderer.SetDrawColor(m_background_color);
    renderer.FillRect(m_bounds);
}

void ProgressBar::RenderFill(Renderer& renderer)
{
    if (m_displayed_progress <= 0.0f) {
        return;
    }
    
    int fill_width = static_cast<int>(m_bounds.w * m_displayed_progress);
    Rect fill_rect = {m_bounds.x, m_bounds.y, fill_width, m_bounds.h};
    
    // Add subtle gradient effect
    Color fill_color = m_fill_color;
    if (m_animated) {
        float pulse = std::sin(m_animation_time * 3.0f) * 0.1f + 0.9f;
        fill_color.r = std::min(255, static_cast<int>(fill_color.r * pulse));
        fill_color.g = std::min(255, static_cast<int>(fill_color.g * pulse));
        fill_color.b = std::min(255, static_cast<int>(fill_color.b * pulse));
    }
    
    renderer.SetDrawColor(fill_color);
    renderer.FillRect(fill_rect);
    
    // Add highlight at the top
    if (fill_width > 0) {
        Color highlight_color = fill_color;
        highlight_color.r = std::min(255, highlight_color.r + 40);
        highlight_color.g = std::min(255, highlight_color.g + 40);
        highlight_color.b = std::min(255, highlight_color.b + 40);
        
        Rect highlight_rect = {fill_rect.x, fill_rect.y, fill_rect.w, 2};
        renderer.SetDrawColor(highlight_color);
        renderer.FillRect(highlight_rect);
    }
}

void ProgressBar::RenderBorder(Renderer& renderer)
{
    renderer.SetDrawColor(m_border_color);
    renderer.DrawRect(m_bounds);
}

void ProgressBar::RenderText(Renderer& renderer, TTF_Font* font)
{
    std::string display_text = GetDisplayText();
    if (display_text.empty()) {
        return;
    }
    
    SDL_Texture* text_texture = renderer.RenderText(display_text, font, m_text_color);
    if (text_texture) {
        int text_w, text_h;
        SDL_QueryTexture(text_texture, nullptr, nullptr, &text_w, &text_h);
        
        // Center text in progress bar
        Rect text_rect = {
            m_bounds.x + (m_bounds.w - text_w) / 2,
            m_bounds.y + (m_bounds.h - text_h) / 2,
            text_w,
            text_h
        };
        
        renderer.DrawTexture(text_texture, nullptr, &text_rect);
        SDL_DestroyTexture(text_texture);
    }
}

std::string ProgressBar::GetDisplayText() const
{
    if (!m_text.empty()) {
        if (m_show_percentage) {
            std::ostringstream oss;
            oss << m_text << " " << std::fixed << std::setprecision(1) 
                << (m_displayed_progress * 100.0f) << "%";
            return oss.str();
        } else {
            return m_text;
        }
    } else if (m_show_percentage) {
        std::ostringstream oss;
        oss << std::fixed << std::setprecision(1) << (m_displayed_progress * 100.0f) << "%";
        return oss.str();
    }
    
    return "";
}