// Copyright (c) 2025 The Gotham Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or https://opensource.org/license/mit/.

#include "console_screen.h"
#include "../renderer.h"
#include "../gotham_city_gui.h"
#include "../utils/font_manager.h"
#include <iostream>

ConsoleScreen::ConsoleScreen(GothamCityGUI& gui) : Screen(gui)
{
    // Initialize UI systems
    if (auto* theme_manager = m_gui.GetThemeManager()) {
        m_ui_factory = std::make_unique<UIFactory>(*theme_manager);
    }
    m_layout_manager = std::make_unique<LayoutManager>();
}

bool ConsoleScreen::Initialize()
{
    if (m_initialized) {
        return true;
    }

    std::cout << "🖥️ Initializing Console Screen..." << std::endl;
    
    CreateLayout();
    SetupButtonCallbacks();
    
    // Add welcome message
    AddConsoleOutput("=== Gotham City Console ===");
    AddConsoleOutput("Welcome to the Gotham City debug console.");
    AddConsoleOutput("Type 'help' for available commands.");
    AddConsoleOutput("");
    
    m_initialized = true;
    return true;
}

void ConsoleScreen::HandleEvent(const SDL_Event& event)
{
    // Handle keyboard shortcuts
    if (event.type == SDL_KEYDOWN) {
        switch (event.key.keysym.sym) {
            case SDLK_ESCAPE:
                m_gui.SwitchScreen(ScreenType::MAIN);
                return;
            case SDLK_RETURN:
                if (m_command_input) {
                    std::string command = m_command_input->GetText();
                    if (!command.empty()) {
                        ExecuteCommand(command);
                        m_command_input->SetText("");
                    }
                }
                return;
            case SDLK_UP:
                // Navigate command history up
                if (!m_command_history.empty() && m_command_history_index < (int)m_command_history.size() - 1) {
                    m_command_history_index++;
                    if (m_command_input) {
                        m_command_input->SetText(m_command_history[m_command_history.size() - 1 - m_command_history_index]);
                    }
                }
                return;
            case SDLK_DOWN:
                // Navigate command history down
                if (m_command_history_index > 0) {
                    m_command_history_index--;
                    if (m_command_input) {
                        m_command_input->SetText(m_command_history[m_command_history.size() - 1 - m_command_history_index]);
                    }
                } else if (m_command_history_index == 0) {
                    m_command_history_index = -1;
                    if (m_command_input) {
                        m_command_input->SetText("");
                    }
                }
                return;
        }
    }
    
    // Pass events to UI components
    if (m_back_button) m_back_button->HandleEvent(event);
    if (m_clear_button) m_clear_button->HandleEvent(event);
    if (m_execute_button) m_execute_button->HandleEvent(event);
    if (m_command_input) m_command_input->HandleEvent(event);
}

void ConsoleScreen::Update(float delta_time)
{
    m_elapsed_time += delta_time;
}

void ConsoleScreen::Render(Renderer& renderer)
{
    // Render background with consistent theme
    renderer.Clear(UIStyleGuide::Colors::BACKGROUND_DARK);
    
    // Render UI panels
    if (m_header_panel) m_header_panel->Render(renderer);
    if (m_console_panel) m_console_panel->Render(renderer);
    if (m_input_panel) m_input_panel->Render(renderer);
    
    // Get fonts from font manager
    FontManager* font_manager = m_gui.GetFontManager();
    if (font_manager) {
        TTF_Font* title_font = font_manager->GetDefaultFont(24);
        TTF_Font* body_font = font_manager->GetDefaultFont(16);
        TTF_Font* mono_font = font_manager->GetDefaultFont(14); // TODO: Use monospace font
        
        // Render header components
        if (m_title_label) m_title_label->Render(renderer, title_font);
        if (m_back_button) m_back_button->Render(renderer, body_font);
        if (m_clear_button) m_clear_button->Render(renderer, body_font);
        
        // Render console lines
        for (const auto& line : m_console_lines) {
            if (line) line->Render(renderer, mono_font);
        }
        
        // Render input components
        if (m_prompt_label) m_prompt_label->Render(renderer, mono_font);
        if (m_command_input) m_command_input->Render(renderer, mono_font);
        if (m_execute_button) m_execute_button->Render(renderer, body_font);
    }
}

void ConsoleScreen::OnActivate()
{
    std::cout << "Console screen activated" << std::endl;
}

void ConsoleScreen::OnResize(int new_width, int new_height)
{
    CreateLayout();
}

void ConsoleScreen::CreateLayout()
{
    CreateHeaderPanel();
    CreateConsolePanel();
    CreateInputPanel();
}

void ConsoleScreen::CreateHeaderPanel()
{
    if (!m_ui_factory || !m_layout_manager) return;
    
    int width = m_gui.GetRenderer()->GetWidth();
    
    // Create header panel with consistent styling
    Rect header_bounds = Rect(0, 0, width, UIStyleGuide::Dimensions::HEADER_HEIGHT);
    m_header_panel = m_ui_factory->CreatePanel(header_bounds, PanelStyle::HEADER);
    
    // Create header components with factory
    m_title_label = m_ui_factory->CreateLabel("🖥️ Debug Console", Point(0, 0), LabelStyle::TITLE);
    m_back_button = m_ui_factory->CreateButton("← Back", Rect(0, 0, 100, 40), ButtonStyle::SECONDARY);
    m_clear_button = m_ui_factory->CreateButton("🗑️ Clear", Rect(0, 0, 100, 40), ButtonStyle::WARNING);
    
    // Setup layout for header components
    std::vector<LayoutItem> header_items(3);
    
    // Title item
    header_items[0].constraints.preferred_width = 300;
    header_items[0].margin = Margin(0, UIStyleGuide::Spacing::LG);
    header_items[0].on_bounds_changed = [this](const Rect& bounds) {
        if (m_title_label) {
            m_title_label->SetPosition(Point(bounds.x, bounds.y + bounds.h / 2 - 12));
        }
    };
    
    // Spacer
    header_items[1].constraints.weight = 1.0f;
    
    // Action buttons
    header_items[2].constraints.preferred_width = 220;
    header_items[2].margin = Margin(UIStyleGuide::Spacing::SM);
    header_items[2].on_bounds_changed = [this](const Rect& bounds) {
        if (m_clear_button && m_back_button) {
            Rect clear_bounds = bounds;
            clear_bounds.w = 100;
            m_clear_button->SetBounds(clear_bounds);
            
            Rect back_bounds = bounds;
            back_bounds.x += 110;
            back_bounds.w = 100;
            m_back_button->SetBounds(back_bounds);
        }
    };
    
    // Apply horizontal layout for header
    m_layout_manager->CreateHorizontalLayout(header_bounds, header_items, 
                                           UIStyleGuide::Spacing::MD, Alignment::CENTER);
}

void ConsoleScreen::CreateConsolePanel()
{
    if (!m_ui_factory || !m_layout_manager) return;
    
    int width = m_gui.GetRenderer()->GetWidth();
    int height = m_gui.GetRenderer()->GetHeight();
    
    // Create console panel with consistent styling
    Rect console_bounds = Rect(UIStyleGuide::Spacing::LG, 
                              UIStyleGuide::Dimensions::HEADER_HEIGHT + UIStyleGuide::Spacing::MD,
                              width - 2 * UIStyleGuide::Spacing::LG, 
                              height - UIStyleGuide::Dimensions::HEADER_HEIGHT - 80 - 2 * UIStyleGuide::Spacing::MD);
    m_console_panel = m_ui_factory->CreatePanel(console_bounds, PanelStyle::MAIN);
}

void ConsoleScreen::CreateInputPanel()
{
    if (!m_ui_factory || !m_layout_manager) return;
    
    int width = m_gui.GetRenderer()->GetWidth();
    int height = m_gui.GetRenderer()->GetHeight();
    
    // Create input panel with consistent styling
    Rect input_bounds = Rect(UIStyleGuide::Spacing::LG, 
                            height - 80 - UIStyleGuide::Spacing::MD,
                            width - 2 * UIStyleGuide::Spacing::LG, 
                            60);
    m_input_panel = m_ui_factory->CreatePanel(input_bounds, PanelStyle::STATUS);
    
    // Create input components
    m_prompt_label = m_ui_factory->CreateLabel("gotham>", Point(0, 0), LabelStyle::BODY);
    m_prompt_label->SetColor(UIStyleGuide::Colors::GOTHAM_GOLD);
    
    m_command_input = std::make_unique<TextInput>(Rect(0, 0, input_bounds.w - 200, UIStyleGuide::Dimensions::INPUT_HEIGHT));
    m_command_input->SetPlaceholder("Enter command...");
    m_command_input->SetBackgroundColor(UIStyleGuide::Colors::SURFACE_DARK);
    m_command_input->SetBorderColor(UIStyleGuide::Colors::BORDER_DARK);
    m_command_input->SetTextColor(UIStyleGuide::Colors::TEXT_PRIMARY);
    
    m_execute_button = m_ui_factory->CreateButton("Execute", Rect(0, 0, 100, UIStyleGuide::Dimensions::BUTTON_HEIGHT), ButtonStyle::PRIMARY);
    
    // Setup layout for input panel
    std::vector<LayoutItem> input_items(3);
    
    // Prompt
    input_items[0].constraints.preferred_width = 80;
    input_items[0].margin = Margin(UIStyleGuide::Spacing::MD);
    input_items[0].on_bounds_changed = [this](const Rect& bounds) {
        if (m_prompt_label) {
            m_prompt_label->SetPosition(Point(bounds.x, bounds.y + bounds.h / 2 - 8));
        }
    };
    
    // Command input
    input_items[1].constraints.weight = 1.0f;
    input_items[1].margin = Margin(UIStyleGuide::Spacing::SM);
    input_items[1].on_bounds_changed = [this](const Rect& bounds) {
        if (m_command_input) {
            m_command_input->SetBounds(bounds);
        }
    };
    
    // Execute button
    input_items[2].constraints.preferred_width = 100;
    input_items[2].margin = Margin(UIStyleGuide::Spacing::SM);
    input_items[2].on_bounds_changed = [this](const Rect& bounds) {
        if (m_execute_button) {
            m_execute_button->SetBounds(bounds);
        }
    };
    
    // Apply horizontal layout for input panel
    m_layout_manager->CreateHorizontalLayout(input_bounds, input_items, 
                                           UIStyleGuide::Spacing::SM, Alignment::CENTER);
}

void ConsoleScreen::SetupButtonCallbacks()
{
    if (m_back_button) {
        m_back_button->SetOnClick([this]() { 
            m_gui.SwitchScreen(ScreenType::MAIN); 
        });
    }
    
    if (m_clear_button) {
        m_clear_button->SetOnClick([this]() {
            ClearConsole();
        });
    }
    
    if (m_execute_button) {
        m_execute_button->SetOnClick([this]() {
            if (m_command_input) {
                std::string command = m_command_input->GetText();
                if (!command.empty()) {
                    ExecuteCommand(command);
                    m_command_input->SetText("");
                }
            }
        });
    }
}

void ConsoleScreen::ExecuteCommand(const std::string& command)
{
    // Add command to history
    m_command_history.push_back(command);
    m_command_history_index = -1;
    
    // Display command in console
    AddConsoleOutput("gotham> " + command);
    
    // Process command
    if (command == "help") {
        AddConsoleOutput("Available commands:");
        AddConsoleOutput("  help          - Show this help message");
        AddConsoleOutput("  clear         - Clear console output");
        AddConsoleOutput("  getinfo       - Get basic node information");
        AddConsoleOutput("  getblockcount - Get current block count");
        AddConsoleOutput("  getbalance    - Get wallet balance");
        AddConsoleOutput("  getnewaddress - Generate new address");
        AddConsoleOutput("  listaccounts  - List wallet accounts");
        AddConsoleOutput("  exit          - Return to main screen");
    } else if (command == "clear") {
        ClearConsole();
        return;
    } else if (command == "exit") {
        m_gui.SwitchScreen(ScreenType::MAIN);
        return;
    } else if (command == "getinfo") {
        AddConsoleOutput("Node Information:");
        AddConsoleOutput("  Version: Gotham Core 1.0.0");
        AddConsoleOutput("  Network: testnet");
        AddConsoleOutput("  Connections: 8");
        AddConsoleOutput("  Block Height: 12345");
    } else if (command == "getblockcount") {
        AddConsoleOutput("12345");
    } else if (command == "getbalance") {
        AddConsoleOutput("1.23456789");
    } else if (command == "getnewaddress") {
        AddConsoleOutput("bc1qxy2kgdygjrsqtzq2n0yrf2493p83kkfjhx0wlh");
    } else if (command == "listaccounts") {
        AddConsoleOutput("Wallet Accounts:");
        AddConsoleOutput("  \"\" : 1.23456789");
        AddConsoleOutput("  \"savings\" : 0.50000000");
    } else {
        AddConsoleOutput("Unknown command: " + command, true);
        AddConsoleOutput("Type 'help' for available commands.");
    }
    
    AddConsoleOutput("");
}

void ConsoleScreen::AddConsoleOutput(const std::string& output, bool is_error)
{
    m_console_history.push_back(output);
    
    // Limit console history
    if (m_console_history.size() > 100) {
        m_console_history.erase(m_console_history.begin());
    }
    
    ScrollConsole();
}

void ConsoleScreen::ClearConsole()
{
    m_console_history.clear();
    m_console_lines.clear();
    AddConsoleOutput("Console cleared.");
    AddConsoleOutput("");
}

void ConsoleScreen::ScrollConsole()
{
    if (!m_ui_factory) return;
    
    m_console_lines.clear();
    
    // Calculate how many lines can fit in console panel
    int console_height = m_gui.GetRenderer()->GetHeight() - UIStyleGuide::Dimensions::HEADER_HEIGHT - 80 - 4 * UIStyleGuide::Spacing::MD;
    int line_height = 20;
    int max_visible_lines = console_height / line_height;
    
    // Show the most recent lines
    int start_index = std::max(0, (int)m_console_history.size() - max_visible_lines);
    
    for (int i = start_index; i < (int)m_console_history.size(); ++i) {
        int y_pos = UIStyleGuide::Dimensions::HEADER_HEIGHT + UIStyleGuide::Spacing::LG + 
                   (i - start_index) * line_height;
        
        auto line_label = m_ui_factory->CreateLabel(m_console_history[i], 
                                                   Point(UIStyleGuide::Spacing::LG + UIStyleGuide::Spacing::MD, y_pos), 
                                                   LabelStyle::BODY);
        
        // Color coding for different types of output
        if (m_console_history[i].find("gotham>") == 0) {
            line_label->SetColor(UIStyleGuide::Colors::GOTHAM_GOLD);
        } else if (m_console_history[i].find("Error:") != std::string::npos || 
                   m_console_history[i].find("Unknown command:") != std::string::npos) {
            line_label->SetColor(UIStyleGuide::Colors::ERROR);
        } else if (m_console_history[i].find("===") != std::string::npos) {
            line_label->SetColor(UIStyleGuide::Colors::GOTHAM_GOLD);
        } else {
            line_label->SetColor(UIStyleGuide::Colors::TEXT_PRIMARY);
        }
        
        m_console_lines.push_back(std::move(line_label));
    }
}