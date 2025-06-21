// Copyright (c) 2025 The Gotham Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or https://opensource.org/license/mit/.

#ifndef GOTHAM_SDL2_SCREENS_CONSOLE_SCREEN_H
#define GOTHAM_SDL2_SCREENS_CONSOLE_SCREEN_H

#include "../ui/screen.h"
#include "../ui/button.h"
#include "../ui/label.h"
#include "../ui/panel.h"
#include "../ui/text_input.h"
#include "../ui/ui_factory.h"
#include "../ui/layout_manager.h"
#include "../ui/ui_style_guide.h"
#include <memory>
#include <vector>
#include <string>

class ConsoleScreen : public Screen
{
public:
    explicit ConsoleScreen(GothamCityGUI& gui);
    bool Initialize() override;
    void HandleEvent(const SDL_Event& event) override;
    void Update(float delta_time) override;
    void Render(Renderer& renderer) override;
    void OnActivate() override;
    void OnResize(int new_width, int new_height) override;

private:
    // UI Systems
    std::unique_ptr<UIFactory> m_ui_factory;
    std::unique_ptr<LayoutManager> m_layout_manager;
    
    // UI Components
    std::unique_ptr<Panel> m_header_panel;
    std::unique_ptr<Panel> m_console_panel;
    std::unique_ptr<Panel> m_input_panel;
    
    // Header components
    std::unique_ptr<Label> m_title_label;
    std::unique_ptr<Button> m_back_button;
    std::unique_ptr<Button> m_clear_button;
    
    // Console components
    std::vector<std::unique_ptr<Label>> m_console_lines;
    std::unique_ptr<TextInput> m_command_input;
    std::unique_ptr<Button> m_execute_button;
    std::unique_ptr<Label> m_prompt_label;
    
    // Console state
    std::vector<std::string> m_console_history;
    std::vector<std::string> m_command_history;
    int m_command_history_index{-1};
    int m_max_console_lines{20};
    float m_elapsed_time{0.0f};
    
    // Methods
    void CreateLayout();
    void CreateHeaderPanel();
    void CreateConsolePanel();
    void CreateInputPanel();
    void SetupButtonCallbacks();
    void ExecuteCommand(const std::string& command);
    void AddConsoleOutput(const std::string& output, bool is_error = false);
    void ClearConsole();
    void ScrollConsole();
};

#endif // GOTHAM_SDL2_SCREENS_CONSOLE_SCREEN_H