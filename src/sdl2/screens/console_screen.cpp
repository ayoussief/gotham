// Copyright (c) 2025 The Gotham Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or https://opensource.org/license/mit/.

#include "console_screen.h"
#include "../gotham_city_gui.h"
#include "../utils/theme.h"
#include "../utils/font_manager.h"
#include <iostream>

ConsoleScreen::ConsoleScreen(GothamCityGUI& gui) : Screen(gui)
{
    if (auto* theme_manager = m_gui.GetThemeManager()) {
        m_ui_factory = std::make_unique<UIFactory>(*theme_manager);
    }
    m_layout_manager = std::make_unique<LayoutManager>();
}

bool ConsoleScreen::Initialize()
{
    if (m_initialized) return true;
    
    CreateContentPanel();
    SetupButtonCallbacks();
    
    AddOutputLine("=== Gotham City Console ===");
    AddOutputLine("Type 'help' for available commands");
    
    m_initialized = true;
    std::cout << "Console screen initialized (content area only)" << std::endl;
    return true;
}

void ConsoleScreen::CreateContentPanel()
{
    if (!m_ui_factory) return;
    
    m_content_panel = m_ui_factory->CreatePanel(Rect(0, 0, 800, 600), PanelStyle::CONTENT);
    CreateOutputPanel();
    CreateInputPanel();
    CreateHistoryPanel();
    CreateQuickCommandsPanel();
}

void ConsoleScreen::CreateOutputPanel()
{
    m_output_panel = m_ui_factory->CreatePanel(Rect(20, 20, 500, 300), PanelStyle::CARD);
    m_output_title_label = m_ui_factory->CreateLabel("Console Output", Point(30, 30), LabelStyle::SUBHEADING);
    
    // Create output lines
    for (int i = 0; i < 15; ++i) {
        auto line_label = m_ui_factory->CreateLabel("", Point(30, 60 + i * 15), LabelStyle::CAPTION);
        m_output_lines.push_back(std::move(line_label));
    }
}

void ConsoleScreen::CreateInputPanel()
{
    m_input_panel = m_ui_factory->CreatePanel(Rect(20, 340, 500, 60), PanelStyle::CARD);
    m_input_label = m_ui_factory->CreateLabel("Command:", Point(30, 350), LabelStyle::BODY);
    m_command_input = m_ui_factory->CreateTextInput(Rect(100, 350, 300, 25), "Enter command...");
    m_execute_button = m_ui_factory->CreateButton("▶ Execute", Rect(410, 350, 80, 25), ButtonStyle::PRIMARY);
    m_clear_button = m_ui_factory->CreateButton("🗑️ Clear", Rect(410, 375, 80, 20), ButtonStyle::GHOST);
}

void ConsoleScreen::CreateHistoryPanel()
{
    m_history_panel = m_ui_factory->CreatePanel(Rect(540, 20, 200, 200), PanelStyle::INFO);
    m_history_title_label = m_ui_factory->CreateLabel("Command History", Point(550, 30), LabelStyle::SUBHEADING);
    
    // Create history lines
    for (int i = 0; i < 10; ++i) {
        auto history_label = m_ui_factory->CreateLabel("", Point(550, 60 + i * 15), LabelStyle::CAPTION);
        m_history_lines.push_back(std::move(history_label));
    }
}

void ConsoleScreen::CreateQuickCommandsPanel()
{
    m_quick_commands_panel = m_ui_factory->CreatePanel(Rect(540, 240, 200, 160), PanelStyle::CARD);
    m_quick_commands_title_label = m_ui_factory->CreateLabel("Quick Commands", Point(550, 250), LabelStyle::SUBHEADING);
    m_getinfo_button = m_ui_factory->CreateButton("getinfo", Rect(550, 280, 80, 25), ButtonStyle::SECONDARY);
    m_getbalance_button = m_ui_factory->CreateButton("getbalance", Rect(550, 310, 80, 25), ButtonStyle::SECONDARY);
    m_getblockcount_button = m_ui_factory->CreateButton("getblockcount", Rect(550, 340, 80, 25), ButtonStyle::SECONDARY);
    m_getconnectioncount_button = m_ui_factory->CreateButton("getconnectioncount", Rect(550, 370, 80, 25), ButtonStyle::SECONDARY);
    m_help_button = m_ui_factory->CreateButton("help", Rect(640, 280, 80, 25), ButtonStyle::GHOST);
}

void ConsoleScreen::SetupButtonCallbacks()
{
    if (m_execute_button) {
        m_execute_button->SetOnClick([this]() { OnExecuteClicked(); });
    }
    if (m_clear_button) {
        m_clear_button->SetOnClick([this]() { OnClearClicked(); });
    }
    if (m_getinfo_button) {
        m_getinfo_button->SetOnClick([this]() { OnGetInfoClicked(); });
    }
    if (m_getbalance_button) {
        m_getbalance_button->SetOnClick([this]() { OnGetBalanceClicked(); });
    }
    if (m_getblockcount_button) {
        m_getblockcount_button->SetOnClick([this]() { OnGetBlockCountClicked(); });
    }
    if (m_getconnectioncount_button) {
        m_getconnectioncount_button->SetOnClick([this]() { OnGetConnectionCountClicked(); });
    }
    if (m_help_button) {
        m_help_button->SetOnClick([this]() { OnHelpClicked(); });
    }
}

void ConsoleScreen::HandleEvent(const SDL_Event& event)
{
    if (event.type == SDL_KEYDOWN) {
        switch (event.key.keysym.sym) {
            case SDLK_RETURN:
                OnExecuteClicked();
                return;
            case SDLK_UP:
                HandleCommandHistory(true);
                return;
            case SDLK_DOWN:
                HandleCommandHistory(false);
                return;
        }
    }
    
    if (m_content_panel) m_content_panel->HandleEvent(event);
    if (m_command_input) m_command_input->HandleEvent(event);
    if (m_execute_button) m_execute_button->HandleEvent(event);
    if (m_clear_button) m_clear_button->HandleEvent(event);
    if (m_getinfo_button) m_getinfo_button->HandleEvent(event);
    if (m_getbalance_button) m_getbalance_button->HandleEvent(event);
    if (m_getblockcount_button) m_getblockcount_button->HandleEvent(event);
    if (m_getconnectioncount_button) m_getconnectioncount_button->HandleEvent(event);
    if (m_help_button) m_help_button->HandleEvent(event);
}

void ConsoleScreen::Update(float delta_time)
{
    m_elapsed_time += delta_time;
    if (m_command_input) m_command_input->Update(delta_time);
    if (m_execute_button) m_execute_button->Update(delta_time);
    if (m_clear_button) m_clear_button->Update(delta_time);
}

void ConsoleScreen::Render(Renderer& renderer)
{
    if (m_content_panel) m_content_panel->Render(renderer);
    if (m_output_panel) m_output_panel->Render(renderer);
    if (m_input_panel) m_input_panel->Render(renderer);
    if (m_history_panel) m_history_panel->Render(renderer);
    if (m_quick_commands_panel) m_quick_commands_panel->Render(renderer);
    
    TTF_Font* subheading_font = m_gui.GetFontManager()->GetDefaultFont(UIStyleGuide::FontSize::SUBHEADING);
    TTF_Font* body_font = m_gui.GetFontManager()->GetDefaultFont(UIStyleGuide::FontSize::BODY);
    TTF_Font* caption_font = m_gui.GetFontManager()->GetDefaultFont(UIStyleGuide::FontSize::CAPTION);
    
    if (m_output_title_label && subheading_font) m_output_title_label->Render(renderer, subheading_font);
    for (auto& line : m_output_lines) {
        if (line && caption_font) line->Render(renderer, caption_font);
    }
    
    if (m_input_label && body_font) m_input_label->Render(renderer, body_font);
    if (m_command_input && body_font) m_command_input->Render(renderer, body_font);
    if (m_execute_button && body_font) m_execute_button->Render(renderer, body_font);
    if (m_clear_button && caption_font) m_clear_button->Render(renderer, caption_font);
    
    if (m_history_title_label && subheading_font) m_history_title_label->Render(renderer, subheading_font);
    for (auto& history : m_history_lines) {
        if (history && caption_font) history->Render(renderer, caption_font);
    }
    
    if (m_quick_commands_title_label && subheading_font) m_quick_commands_title_label->Render(renderer, subheading_font);
    if (m_getinfo_button && caption_font) m_getinfo_button->Render(renderer, caption_font);
    if (m_getbalance_button && caption_font) m_getbalance_button->Render(renderer, caption_font);
    if (m_getblockcount_button && caption_font) m_getblockcount_button->Render(renderer, caption_font);
    if (m_getconnectioncount_button && caption_font) m_getconnectioncount_button->Render(renderer, caption_font);
    if (m_help_button && caption_font) m_help_button->Render(renderer, caption_font);
}

void ConsoleScreen::OnActivate()
{
    std::cout << "Console screen activated" << std::endl;
}

void ConsoleScreen::OnResize(int new_width, int new_height)
{
    RepositionElements(new_width, new_height);
}

void ConsoleScreen::OnNavigatedTo(const NavigationContext& context)
{
    std::cout << "Navigated to Console screen" << std::endl;
    if (m_command_input) {
        m_command_input->SetFocus(true);
    }
}

void ConsoleScreen::OnNavigatedFrom()
{
    std::cout << "Navigated away from Console screen" << std::endl;
}

void ConsoleScreen::ExecuteCommand(const std::string& command)
{
    if (command.empty()) return;
    
    AddOutputLine("> " + command);
    AddToHistory(command);
    
    std::string result = ProcessRPCCommand(command);
    AddOutputLine(result);
    
    if (m_command_input) {
        m_command_input->SetText("");
    }
}

void ConsoleScreen::AddOutputLine(const std::string& line)
{
    m_console_output.push_back(line);
    if (m_console_output.size() > m_max_output_lines) {
        m_console_output.pop_front();
    }
    UpdateOutputDisplay();
}

void ConsoleScreen::AddToHistory(const std::string& command)
{
    m_command_history.push_back(command);
    if (m_command_history.size() > m_max_history_lines) {
        m_command_history.pop_front();
    }
    m_history_index = -1;
    UpdateHistoryDisplay();
}

void ConsoleScreen::ClearOutput()
{
    m_console_output.clear();
    UpdateOutputDisplay();
}

void ConsoleScreen::UpdateOutputDisplay()
{
    for (size_t i = 0; i < m_output_lines.size(); ++i) {
        if (i < m_console_output.size()) {
            size_t output_index = m_console_output.size() - m_output_lines.size() + i;
            if (output_index < m_console_output.size()) {
                m_output_lines[i]->SetText(m_console_output[output_index]);
            }
        } else {
            m_output_lines[i]->SetText("");
        }
    }
}

void ConsoleScreen::UpdateHistoryDisplay()
{
    for (size_t i = 0; i < m_history_lines.size(); ++i) {
        if (i < m_command_history.size()) {
            size_t history_index = m_command_history.size() - m_history_lines.size() + i;
            if (history_index < m_command_history.size()) {
                m_history_lines[i]->SetText(m_command_history[history_index]);
            }
        } else {
            m_history_lines[i]->SetText("");
        }
    }
}

void ConsoleScreen::HandleCommandHistory(bool up)
{
    if (m_command_history.empty()) return;
    
    if (up) {
        if (m_history_index < (int)m_command_history.size() - 1) {
            m_history_index++;
            if (m_command_input) {
                m_command_input->SetText(m_command_history[m_command_history.size() - 1 - m_history_index]);
            }
        }
    } else {
        if (m_history_index > 0) {
            m_history_index--;
            if (m_command_input) {
                m_command_input->SetText(m_command_history[m_command_history.size() - 1 - m_history_index]);
            }
        } else if (m_history_index == 0) {
            m_history_index = -1;
            if (m_command_input) {
                m_command_input->SetText("");
            }
        }
    }
}

void ConsoleScreen::RepositionElements(int content_width, int content_height)
{
    // Responsive repositioning would be implemented here
}

void ConsoleScreen::OnExecuteClicked()
{
    if (m_command_input) {
        std::string command = m_command_input->GetText();
        ExecuteCommand(command);
    }
}

void ConsoleScreen::OnClearClicked()
{
    ClearOutput();
}

void ConsoleScreen::OnGetInfoClicked()
{
    ExecuteCommand("getinfo");
}

void ConsoleScreen::OnGetBalanceClicked()
{
    ExecuteCommand("getbalance");
}

void ConsoleScreen::OnGetBlockCountClicked()
{
    ExecuteCommand("getblockcount");
}

void ConsoleScreen::OnGetConnectionCountClicked()
{
    ExecuteCommand("getconnectioncount");
}

void ConsoleScreen::OnHelpClicked()
{
    ExecuteCommand("help");
}

std::string ConsoleScreen::ProcessRPCCommand(const std::string& command)
{
    // Mock RPC command processing
    if (command == "help") {
        return "Available commands: getinfo, getbalance, getblockcount, getconnectioncount";
    } else if (command == "getinfo") {
        return "Version: 1.0.0, Network: mainnet, Blocks: 750123";
    } else if (command == "getbalance") {
        return "Balance: 0.00000000 BTC";
    } else if (command == "getblockcount") {
        return "Block count: 750123";
    } else if (command == "getconnectioncount") {
        return "Connections: 8";
    } else {
        return "Unknown command: " + command + " (type 'help' for available commands)";
    }
}

void ConsoleScreen::ShowCommandHelp()
{
    AddOutputLine("=== Available Commands ===");
    AddOutputLine("getinfo - Show general information");
    AddOutputLine("getbalance - Show wallet balance");
    AddOutputLine("getblockcount - Show current block count");
    AddOutputLine("getconnectioncount - Show peer connections");
    AddOutputLine("help - Show this help message");
}