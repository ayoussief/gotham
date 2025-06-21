// Copyright (c) 2025 The Gotham Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or https://opensource.org/license/mit/.

#include "panel.h"
#include "button.h"
#include "label.h"

Panel::Panel(const Rect& bounds)
    : m_bounds(bounds)
{
}

void Panel::HandleEvent(const SDL_Event& event)
{
    if (!m_visible) {
        return;
    }

    // Pass events to child buttons
    for (auto& button : m_buttons) {
        button->HandleEvent(event);
    }
}

void Panel::Update(float delta_time)
{
    if (!m_visible) {
        return;
    }

    // Update child buttons
    for (auto& button : m_buttons) {
        button->Update(delta_time);
    }
}

void Panel::Render(Renderer& renderer)
{
    if (!m_visible) {
        return;
    }

    // Render background
    RenderBackground(renderer);

    // Render border
    if (m_border_width > 0) {
        RenderBorder(renderer);
    }

    // Render child components
    // Note: In a real implementation, you'd want to set up clipping
    // to ensure children don't render outside the panel bounds

    for (auto& label : m_labels) {
        // For now, we'll assume labels have their own font management
        // In a real implementation, you'd pass the appropriate font
        // label->Render(renderer, font);
    }

    for (auto& button : m_buttons) {
        // For now, we'll assume buttons have their own font management
        // In a real implementation, you'd pass the appropriate font
        // button->Render(renderer, font);
    }
}

void Panel::AddButton(std::shared_ptr<Button> button)
{
    if (button) {
        m_buttons.push_back(button);
    }
}

void Panel::AddLabel(std::shared_ptr<Label> label)
{
    if (label) {
        m_labels.push_back(label);
    }
}

void Panel::ClearChildren()
{
    m_buttons.clear();
    m_labels.clear();
}

std::shared_ptr<Button> Panel::GetButton(size_t index)
{
    if (index < m_buttons.size()) {
        return m_buttons[index];
    }
    return nullptr;
}

std::shared_ptr<Label> Panel::GetLabel(size_t index)
{
    if (index < m_labels.size()) {
        return m_labels[index];
    }
    return nullptr;
}

void Panel::RenderBackground(Renderer& renderer)
{
    Color bg_color = m_background_color;
    bg_color.a = static_cast<Uint8>((bg_color.a * m_alpha) / 255);
    
    renderer.SetDrawColor(bg_color);
    renderer.FillRect(m_bounds);
}

void Panel::RenderBorder(Renderer& renderer)
{
    Color border_color = m_border_color;
    border_color.a = static_cast<Uint8>((border_color.a * m_alpha) / 255);
    
    renderer.SetDrawColor(border_color);
    
    // Draw border lines
    for (int i = 0; i < m_border_width; ++i) {
        Rect border_rect = {
            m_bounds.x - i,
            m_bounds.y - i,
            m_bounds.w + 2 * i,
            m_bounds.h + 2 * i
        };
        renderer.DrawRect(border_rect);
    }
}