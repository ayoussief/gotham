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
#include "../ui/navigation_manager.h"
#include <memory>
#include <vector>
#include <deque>

/**
 * Debug console screen - Content Area Only
 * RPC command interface and debug output
 */
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
    
    // Navigation lifecycle methods
    void OnNavigatedTo(const NavigationContext& context) override;
    void OnNavigatedFrom() override;

private:
    // UI Systems
    std::unique_ptr<UIFactory> m_ui_factory;
    std::unique_ptr<LayoutManager> m_layout_manager;
    
    // Content area components only
    std::unique_ptr<Panel> m_content_panel;
    
    // Console output panel
    std::unique_ptr<Panel> m_output_panel;
    std::unique_ptr<Label> m_output_title_label;
    std::vector<std::unique_ptr<Label>> m_output_lines;
    
    // Command input panel
    std::unique_ptr<Panel> m_input_panel;
    std::unique_ptr<Label> m_input_label;
    std::unique_ptr<TextInput> m_command_input;
    std::unique_ptr<Button> m_execute_button;
    std::unique_ptr<Button> m_clear_button;
    
    // Command history panel
    std::unique_ptr<Panel> m_history_panel;
    std::unique_ptr<Label> m_history_title_label;
    std::vector<std::unique_ptr<Label>> m_history_lines;
    
    // Quick commands panel
    std::unique_ptr<Panel> m_quick_commands_panel;
    std::unique_ptr<Label> m_quick_commands_title_label;
    std::unique_ptr<Button> m_getinfo_button;
    std::unique_ptr<Button> m_getbalance_button;
    std::unique_ptr<Button> m_getblockcount_button;
    std::unique_ptr<Button> m_getconnectioncount_button;
    std::unique_ptr<Button> m_help_button;
    
    // Console state
    std::deque<std::string> m_console_output;
    std::deque<std::string> m_command_history;
    int m_history_index{-1};
    int m_max_output_lines{50};
    int m_max_history_lines{20};
    float m_elapsed_time{0.0f};
    
    // Content area methods
    void CreateContentPanel();
    void CreateOutputPanel();
    void CreateInputPanel();
    void CreateHistoryPanel();
    void CreateQuickCommandsPanel();
    void SetupButtonCallbacks();
    void ExecuteCommand(const std::string& command);
    void AddOutputLine(const std::string& line);
    void AddToHistory(const std::string& command);
    void ClearOutput();
    void UpdateOutputDisplay();
    void UpdateHistoryDisplay();
    void HandleCommandHistory(bool up);
    void RepositionElements(int content_width, int content_height);
    
    // Action callbacks
    void OnExecuteClicked();
    void OnClearClicked();
    void OnGetInfoClicked();
    void OnGetBalanceClicked();
    void OnGetBlockCountClicked();
    void OnGetConnectionCountClicked();
    void OnHelpClicked();
    
    // Command processing
    std::string ProcessRPCCommand(const std::string& command);
    void ShowCommandHelp();
};

#endif // GOTHAM_SDL2_SCREENS_CONSOLE_SCREEN_H