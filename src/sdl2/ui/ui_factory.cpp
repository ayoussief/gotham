// Copyright (c) 2025 The Gotham Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or https://opensource.org/license/mit/.

#include "ui_factory.h"
#include <iostream>

UIFactory::UIFactory(ThemeManager& theme_manager)
    : m_theme_manager(theme_manager)
{
}

std::unique_ptr<Button> UIFactory::CreateButton(const std::string& text, const Rect& bounds, ButtonStyle style)
{
    auto button = std::make_unique<Button>(text, bounds);
    ApplyButtonStyle(*button, style);
    return button;
}

std::unique_ptr<Label> UIFactory::CreateLabel(const std::string& text, const Point& position, LabelStyle style)
{
    auto label = std::make_unique<Label>(text, position);
    ApplyLabelStyle(*label, style);
    return label;
}

std::unique_ptr<Panel> UIFactory::CreatePanel(const Rect& bounds, PanelStyle style)
{
    auto panel = std::make_unique<Panel>(bounds);
    ApplyPanelStyle(*panel, style);
    return panel;
}

std::unique_ptr<TextInput> UIFactory::CreateTextInput(const Rect& bounds, const std::string& placeholder)
{
    auto input = std::make_unique<TextInput>(bounds);
    input->SetPlaceholder(placeholder);
    
    // Apply consistent styling
    input->SetBackgroundColor(m_theme_manager.GetColor(ThemeColor::SURFACE));
    input->SetBorderColor(m_theme_manager.GetColor(ThemeColor::BORDER));
    input->SetTextColor(m_theme_manager.GetColor(ThemeColor::TEXT_PRIMARY));
    input->SetBorderWidth(1);
    
    return input;
}

std::unique_ptr<ProgressBar> UIFactory::CreateProgressBar(const Rect& bounds)
{
    auto progress_bar = std::make_unique<ProgressBar>(bounds);
    
    // Apply consistent styling
    progress_bar->SetBackgroundColor(m_theme_manager.GetColor(ThemeColor::SURFACE));
    progress_bar->SetForegroundColor(m_theme_manager.GetColor(ThemeColor::PRIMARY));
    progress_bar->SetBorderColor(m_theme_manager.GetColor(ThemeColor::BORDER));
    
    return progress_bar;
}

int UIFactory::GetSpacing(const std::string& size) const
{
    return m_theme_manager.GetSpacing(size);
}

int UIFactory::GetStandardHeight(const std::string& component) const
{
    if (component == "button") return 40;
    if (component == "input") return 36;
    if (component == "header") return 80;
    if (component == "status") return 30;
    if (component == "sidebar_button") return 50;
    return 32; // default
}

std::unique_ptr<Button> UIFactory::CreateNavigationButton(const std::string& text, const Rect& bounds, bool active)
{
    auto button = std::make_unique<Button>(text, bounds);
    
    if (active) {
        // Active navigation button styling
        button->SetColors(
            m_theme_manager.GetColor(ThemeColor::PRIMARY),
            m_theme_manager.GetColor(ThemeColor::ACCENT),
            m_theme_manager.GetColor(ThemeColor::GOTHAM_DARK_BLUE)
        );
        button->SetTextColor(m_theme_manager.GetColor(ThemeColor::BACKGROUND));
    } else {
        // Inactive navigation button styling
        button->SetColors(
            Color(0, 0, 0, 0), // Transparent
            m_theme_manager.GetColor(ThemeColor::SURFACE),
            m_theme_manager.GetColor(ThemeColor::BORDER)
        );
        button->SetTextColor(m_theme_manager.GetColor(ThemeColor::TEXT_SECONDARY));
    }
    
    return button;
}

UIFactory::FormField UIFactory::CreateFormField(const std::string& label_text, const Rect& input_bounds, 
                                               const std::string& placeholder)
{
    FormField field;
    
    // Create label above input
    Point label_pos(input_bounds.x, input_bounds.y - 25);
    field.label = CreateLabel(label_text, label_pos, LabelStyle::BODY);
    
    // Create input
    field.input = CreateTextInput(input_bounds, placeholder);
    
    return field;
}

void UIFactory::ApplyButtonStyle(Button& button, ButtonStyle style)
{
    switch (style) {
        case ButtonStyle::PRIMARY:
            button.SetColors(
                m_theme_manager.GetColor(ThemeColor::PRIMARY),
                m_theme_manager.GetColor(ThemeColor::ACCENT),
                m_theme_manager.GetColor(ThemeColor::GOTHAM_DARK_BLUE)
            );
            button.SetTextColor(m_theme_manager.GetColor(ThemeColor::BACKGROUND));
            button.SetPrimary(true);
            break;
            
        case ButtonStyle::SECONDARY:
            button.SetColors(
                m_theme_manager.GetColor(ThemeColor::SECONDARY),
                m_theme_manager.GetColor(ThemeColor::GOTHAM_LIGHT_BLUE),
                m_theme_manager.GetColor(ThemeColor::GOTHAM_DARK_BLUE)
            );
            button.SetTextColor(m_theme_manager.GetColor(ThemeColor::TEXT_PRIMARY));
            break;
            
        case ButtonStyle::SUCCESS:
            button.SetColors(
                m_theme_manager.GetColor(ThemeColor::SUCCESS),
                Color(56, 180, 77, 255), // Lighter green
                Color(36, 140, 57, 255)  // Darker green
            );
            button.SetTextColor(Color(255, 255, 255, 255));
            break;
            
        case ButtonStyle::WARNING:
            button.SetColors(
                m_theme_manager.GetColor(ThemeColor::WARNING),
                Color(255, 213, 47, 255), // Lighter yellow
                Color(235, 173, 0, 255)   // Darker yellow
            );
            button.SetTextColor(m_theme_manager.GetColor(ThemeColor::BACKGROUND));
            break;
            
        case ButtonStyle::ERROR:
            button.SetColors(
                m_theme_manager.GetColor(ThemeColor::ERROR),
                Color(240, 73, 89, 255), // Lighter red
                Color(200, 33, 49, 255)  // Darker red
            );
            button.SetTextColor(Color(255, 255, 255, 255));
            break;
            
        case ButtonStyle::GHOST:
            button.SetColors(
                Color(0, 0, 0, 0), // Transparent
                m_theme_manager.GetColor(ThemeColor::SURFACE),
                m_theme_manager.GetColor(ThemeColor::BORDER)
            );
            button.SetTextColor(m_theme_manager.GetColor(ThemeColor::TEXT_PRIMARY));
            break;
    }
}

void UIFactory::ApplyLabelStyle(Label& label, LabelStyle style)
{
    switch (style) {
        case LabelStyle::TITLE:
            label.SetColor(m_theme_manager.GetColor(ThemeColor::PRIMARY));
            break;
            
        case LabelStyle::HEADING:
            label.SetColor(m_theme_manager.GetColor(ThemeColor::TEXT_PRIMARY));
            break;
            
        case LabelStyle::SUBHEADING:
            label.SetColor(m_theme_manager.GetColor(ThemeColor::TEXT_PRIMARY));
            break;
            
        case LabelStyle::BODY:
            label.SetColor(m_theme_manager.GetColor(ThemeColor::TEXT_PRIMARY));
            break;
            
        case LabelStyle::CAPTION:
            label.SetColor(m_theme_manager.GetColor(ThemeColor::TEXT_SECONDARY));
            break;
            
        case LabelStyle::STATUS:
            label.SetColor(m_theme_manager.GetColor(ThemeColor::TEXT_SECONDARY));
            break;
            
        case LabelStyle::ERROR:
            label.SetColor(m_theme_manager.GetColor(ThemeColor::ERROR));
            break;
    }
}

void UIFactory::ApplyPanelStyle(Panel& panel, PanelStyle style)
{
    switch (style) {
        case PanelStyle::HEADER:
            panel.SetBackgroundColor(m_theme_manager.GetColor(ThemeColor::SURFACE));
            panel.SetBorderColor(m_theme_manager.GetColor(ThemeColor::PRIMARY));
            panel.SetBorderWidth(2);
            break;
            
        case PanelStyle::SIDEBAR:
            panel.SetBackgroundColor(Color(20, 20, 30, 255));
            panel.SetBorderColor(m_theme_manager.GetColor(ThemeColor::BORDER));
            panel.SetBorderWidth(1);
            break;
            
        case PanelStyle::MAIN:
            panel.SetBackgroundColor(m_theme_manager.GetColor(ThemeColor::BACKGROUND));
            panel.SetBorderColor(Color(0, 0, 0, 0)); // No border
            panel.SetBorderWidth(0);
            break;
            
        case PanelStyle::STATUS:
            panel.SetBackgroundColor(Color(30, 30, 40, 255));
            panel.SetBorderColor(m_theme_manager.GetColor(ThemeColor::BORDER));
            panel.SetBorderWidth(1);
            break;
            
        case PanelStyle::CARD:
            panel.SetBackgroundColor(m_theme_manager.GetColor(ThemeColor::SURFACE));
            panel.SetBorderColor(m_theme_manager.GetColor(ThemeColor::BORDER));
            panel.SetBorderWidth(1);
            break;
            
        case PanelStyle::CONTENT:
            // Make content panel more visible with a subtle background
            panel.SetBackgroundColor(Color(25, 35, 45, 180)); // Semi-transparent dark blue
            panel.SetBorderColor(Color(70, 130, 180, 100)); // Subtle border
            panel.SetBorderWidth(1);
            break;
            
        case PanelStyle::INFO:
            panel.SetBackgroundColor(Color(25, 35, 45, 255)); // Slightly different from STATUS
            panel.SetBorderColor(m_theme_manager.GetColor(ThemeColor::BORDER));
            panel.SetBorderWidth(1);
            break;
            
        case PanelStyle::MODAL:
            panel.SetBackgroundColor(m_theme_manager.GetColor(ThemeColor::SURFACE));
            panel.SetBorderColor(m_theme_manager.GetColor(ThemeColor::PRIMARY));
            panel.SetBorderWidth(2);
            panel.SetAlpha(240); // Slightly transparent
            break;
    }
}