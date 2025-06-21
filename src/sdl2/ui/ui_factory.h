// Copyright (c) 2025 The Gotham Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or https://opensource.org/license/mit/.

#ifndef GOTHAM_SDL2_UI_UI_FACTORY_H
#define GOTHAM_SDL2_UI_UI_FACTORY_H

#include "button.h"
#include "label.h"
#include "panel.h"
#include "text_input.h"
#include "progress_bar.h"
#include "../utils/theme.h"
#include <memory>
#include <string>

enum class ButtonStyle {
    PRIMARY,
    SECONDARY,
    SUCCESS,
    WARNING,
    ERROR,
    GHOST
};

enum class PanelStyle {
    HEADER,
    SIDEBAR,
    MAIN,
    STATUS,
    CARD,
    MODAL
};

enum class LabelStyle {
    TITLE,
    HEADING,
    BODY,
    CAPTION,
    STATUS,
    ERROR
};

/**
 * Factory for creating consistently styled UI components
 * Ensures all components follow the Gotham City design system
 */
class UIFactory
{
public:
    explicit UIFactory(ThemeManager& theme_manager);
    ~UIFactory() = default;

    /**
     * Create a styled button
     */
    std::unique_ptr<Button> CreateButton(const std::string& text, const Rect& bounds, 
                                       ButtonStyle style = ButtonStyle::PRIMARY);

    /**
     * Create a styled label
     */
    std::unique_ptr<Label> CreateLabel(const std::string& text, const Point& position,
                                     LabelStyle style = LabelStyle::BODY);

    /**
     * Create a styled panel
     */
    std::unique_ptr<Panel> CreatePanel(const Rect& bounds, PanelStyle style = PanelStyle::MAIN);

    /**
     * Create a styled text input
     */
    std::unique_ptr<TextInput> CreateTextInput(const Rect& bounds, const std::string& placeholder = "");

    /**
     * Create a styled progress bar
     */
    std::unique_ptr<ProgressBar> CreateProgressBar(const Rect& bounds);

    /**
     * Get standard spacing values
     */
    int GetSpacing(const std::string& size) const;

    /**
     * Get standard component heights
     */
    int GetStandardHeight(const std::string& component) const;

    /**
     * Create a navigation button (for sidebar)
     */
    std::unique_ptr<Button> CreateNavigationButton(const std::string& text, const Rect& bounds, bool active = false);

    /**
     * Create a form field set (label + input)
     */
    struct FormField {
        std::unique_ptr<Label> label;
        std::unique_ptr<TextInput> input;
    };
    FormField CreateFormField(const std::string& label_text, const Rect& input_bounds, 
                            const std::string& placeholder = "");

private:
    ThemeManager& m_theme_manager;

    void ApplyButtonStyle(Button& button, ButtonStyle style);
    void ApplyLabelStyle(Label& label, LabelStyle style);
    void ApplyPanelStyle(Panel& panel, PanelStyle style);
};

#endif // GOTHAM_SDL2_UI_UI_FACTORY_H