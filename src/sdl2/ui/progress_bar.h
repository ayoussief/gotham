// Copyright (c) 2025 The Gotham Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or https://opensource.org/license/mit/.

#ifndef GOTHAM_SDL2_UI_PROGRESS_BAR_H
#define GOTHAM_SDL2_UI_PROGRESS_BAR_H

#include "../renderer.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <string>

/**
 * Progress bar UI component
 */
class ProgressBar
{
public:
    ProgressBar(const Rect& bounds);
    ~ProgressBar() = default;

    /**
     * Update progress bar animation
     */
    void Update(float delta_time);

    /**
     * Render the progress bar
     */
    void Render(Renderer& renderer, TTF_Font* font = nullptr);

    /**
     * Set progress value (0.0 to 1.0)
     */
    void SetProgress(float progress);

    /**
     * Get progress value
     */
    float GetProgress() const { return m_progress; }

    /**
     * Set bounds
     */
    void SetBounds(const Rect& bounds) { m_bounds = bounds; }

    /**
     * Get bounds
     */
    const Rect& GetBounds() const { return m_bounds; }

    /**
     * Set text to display on progress bar
     */
    void SetText(const std::string& text) { m_text = text; }

    /**
     * Get text
     */
    const std::string& GetText() const { return m_text; }

    /**
     * Set whether to show percentage
     */
    void SetShowPercentage(bool show) { m_show_percentage = show; }

    /**
     * Check if showing percentage
     */
    bool IsShowingPercentage() const { return m_show_percentage; }

    /**
     * Set colors
     */
    void SetColors(const Color& background, const Color& fill, const Color& border);

    /**
     * Set text color
     */
    void SetTextColor(const Color& color) { m_text_color = color; }

    /**
     * Set animated mode
     */
    void SetAnimated(bool animated) { m_animated = animated; }

    /**
     * Check if animated
     */
    bool IsAnimated() const { return m_animated; }

    /**
     * Set background color
     */
    void SetBackgroundColor(const Color& color) { m_background_color = color; }

    /**
     * Set foreground color
     */
    void SetForegroundColor(const Color& color) { m_fill_color = color; }

    /**
     * Set border color
     */
    void SetBorderColor(const Color& color) { m_border_color = color; }

private:
    Rect m_bounds;
    float m_progress{0.0f};
    float m_displayed_progress{0.0f};  // For smooth animation
    std::string m_text;
    bool m_show_percentage{true};
    bool m_animated{true};
    
    // Colors
    Color m_background_color{40, 40, 50, 255};
    Color m_fill_color{255, 215, 0, 255};      // Gotham gold
    Color m_border_color{70, 130, 180, 255};   // Steel blue
    Color m_text_color{255, 255, 255, 255};
    
    // Animation
    float m_animation_time{0.0f};
    
    void RenderBackground(Renderer& renderer);
    void RenderFill(Renderer& renderer);
    void RenderBorder(Renderer& renderer);
    void RenderText(Renderer& renderer, TTF_Font* font);
    std::string GetDisplayText() const;
};

#endif // GOTHAM_SDL2_UI_PROGRESS_BAR_H