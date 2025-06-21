// Copyright (c) 2025 The Gotham Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or https://opensource.org/license/mit/.

#ifndef GOTHAM_SDL2_UI_PANEL_H
#define GOTHAM_SDL2_UI_PANEL_H

#include "../renderer.h"
#include <SDL2/SDL.h>
#include <vector>
#include <memory>

class Button;
class Label;

/**
 * Panel UI component that can contain other UI elements
 */
class Panel
{
public:
    Panel(const Rect& bounds);
    ~Panel() = default;

    /**
     * Handle SDL events
     */
    void HandleEvent(const SDL_Event& event);

    /**
     * Update panel and child components
     */
    void Update(float delta_time);

    /**
     * Render the panel and child components
     */
    void Render(Renderer& renderer);

    /**
     * Set panel bounds
     */
    void SetBounds(const Rect& bounds) { m_bounds = bounds; }

    /**
     * Get panel bounds
     */
    const Rect& GetBounds() const { return m_bounds; }

    /**
     * Set background color
     */
    void SetBackgroundColor(const Color& color) { m_background_color = color; }

    /**
     * Get background color
     */
    const Color& GetBackgroundColor() const { return m_background_color; }

    /**
     * Set border color
     */
    void SetBorderColor(const Color& color) { m_border_color = color; }

    /**
     * Get border color
     */
    const Color& GetBorderColor() const { return m_border_color; }

    /**
     * Set border width
     */
    void SetBorderWidth(int width) { m_border_width = width; }

    /**
     * Get border width
     */
    int GetBorderWidth() const { return m_border_width; }

    /**
     * Set visibility
     */
    void SetVisible(bool visible) { m_visible = visible; }

    /**
     * Check if panel is visible
     */
    bool IsVisible() const { return m_visible; }

    /**
     * Set transparency
     */
    void SetAlpha(Uint8 alpha) { m_alpha = alpha; }

    /**
     * Get transparency
     */
    Uint8 GetAlpha() const { return m_alpha; }

    /**
     * Add a button to the panel
     */
    void AddButton(std::shared_ptr<Button> button);

    /**
     * Add a label to the panel
     */
    void AddLabel(std::shared_ptr<Label> label);

    /**
     * Add a nested panel to the panel
     */
    void AddPanel(std::shared_ptr<Panel> panel);

    /**
     * Clear all child components
     */
    void ClearChildren();

    /**
     * Get button by index
     */
    std::shared_ptr<Button> GetButton(size_t index);

    /**
     * Get label by index
     */
    std::shared_ptr<Label> GetLabel(size_t index);

    /**
     * Get panel by index
     */
    std::shared_ptr<Panel> GetPanel(size_t index);

    /**
     * Get number of buttons
     */
    size_t GetButtonCount() const { return m_buttons.size(); }

    /**
     * Get number of labels
     */
    size_t GetLabelCount() const { return m_labels.size(); }

    /**
     * Get number of panels
     */
    size_t GetPanelCount() const { return m_panels.size(); }

private:
    Rect m_bounds;
    Color m_background_color{25, 25, 35, 200};  // Semi-transparent dark
    Color m_border_color{70, 130, 180, 255};    // Steel blue
    int m_border_width{1};
    bool m_visible{true};
    Uint8 m_alpha{255};

    std::vector<std::shared_ptr<Button>> m_buttons;
    std::vector<std::shared_ptr<Label>> m_labels;
    std::vector<std::shared_ptr<Panel>> m_panels;

    void RenderBackground(Renderer& renderer);
    void RenderBorder(Renderer& renderer);
};

#endif // GOTHAM_SDL2_UI_PANEL_H