// Copyright (c) 2025 The Gotham Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or https://opensource.org/license/mit/.

#include "main_screen.h"
#include "../gotham_city_gui.h"
#include "../utils/theme.h"
#include "../utils/font_manager.h"
#include <iostream>
#include <iomanip>
#include <sstream>

MainScreen::MainScreen(GothamCityGUI& gui)
    : Screen(gui)
{
    // Initialize UI systems
    if (auto* theme_manager = m_gui.GetThemeManager()) {
        m_ui_factory = std::make_unique<UIFactory>(*theme_manager);
    }
    m_layout_manager = std::make_unique<LayoutManager>();
}

bool MainScreen::Initialize()
{
    if (m_initialized) {
        return true;
    }

    CreateLayout();
    SetupButtonCallbacks();

    m_initialized = true;
    std::cout << "Main screen initialized" << std::endl;
    return true;
}

void MainScreen::CreateLayout()
{
    CreateHeaderPanel();
    CreateSidebarPanel();
    CreateMainPanel();
    CreateStatusPanel();
}

void MainScreen::HandleEvent(const SDL_Event& event)
{
    // Pass events to UI panels
    if (m_header_panel) m_header_panel->HandleEvent(event);
    if (m_sidebar_panel) m_sidebar_panel->HandleEvent(event);
    if (m_main_panel) m_main_panel->HandleEvent(event);
    if (m_status_panel) m_status_panel->HandleEvent(event);
    
    // Handle individual buttons
    if (m_settings_button) m_settings_button->HandleEvent(event);
    if (m_wallet_button) m_wallet_button->HandleEvent(event);
    if (m_send_button) m_send_button->HandleEvent(event);
    if (m_receive_button) m_receive_button->HandleEvent(event);
    if (m_transactions_button) m_transactions_button->HandleEvent(event);
    if (m_console_button) m_console_button->HandleEvent(event);
}

void MainScreen::Update(float delta_time)
{
    m_elapsed_time += delta_time;
    
    // Update UI panels
    if (m_header_panel) m_header_panel->Update(delta_time);
    if (m_sidebar_panel) m_sidebar_panel->Update(delta_time);
    if (m_main_panel) m_main_panel->Update(delta_time);
    if (m_status_panel) m_status_panel->Update(delta_time);
    
    // Update individual buttons
    if (m_settings_button) m_settings_button->Update(delta_time);
    if (m_wallet_button) m_wallet_button->Update(delta_time);
    if (m_send_button) m_send_button->Update(delta_time);
    if (m_receive_button) m_receive_button->Update(delta_time);
    if (m_transactions_button) m_transactions_button->Update(delta_time);
    if (m_console_button) m_console_button->Update(delta_time);
    
    // Update wallet and network info periodically
    if (static_cast<int>(m_elapsed_time) % 5 == 0) {
        UpdateWalletInfo();
        UpdateNetworkInfo();
    }
}

void MainScreen::Render(Renderer& renderer)
{
    // Render background
    RenderBackground(renderer);
    
    // Render UI panels
    if (m_header_panel) m_header_panel->Render(renderer);
    if (m_sidebar_panel) m_sidebar_panel->Render(renderer);
    if (m_main_panel) m_main_panel->Render(renderer);
    if (m_status_panel) m_status_panel->Render(renderer);
    
    // Render individual components with fonts
    FontManager* font_manager = m_gui.GetFontManager();
    if (font_manager) {
        // Header components
        TTF_Font* title_font = font_manager->GetDefaultFont(24);
        TTF_Font* balance_font = font_manager->GetDefaultFont(20);
        TTF_Font* button_font = font_manager->GetDefaultFont(16);
        TTF_Font* body_font = font_manager->GetDefaultFont(14);
        
        if (m_title_label && title_font) {
            m_title_label->Render(renderer, title_font);
        }
        
        if (m_balance_label && balance_font) {
            m_balance_label->Render(renderer, balance_font);
        }
        
        if (m_settings_button && button_font) {
            m_settings_button->Render(renderer, button_font);
        }
        
        // Sidebar buttons
        if (m_wallet_button && button_font) {
            m_wallet_button->Render(renderer, button_font);
        }
        if (m_send_button && button_font) {
            m_send_button->Render(renderer, button_font);
        }
        if (m_receive_button && button_font) {
            m_receive_button->Render(renderer, button_font);
        }
        if (m_transactions_button && button_font) {
            m_transactions_button->Render(renderer, button_font);
        }
        if (m_console_button && button_font) {
            m_console_button->Render(renderer, button_font);
        }
        
        // Main content labels
        if (m_welcome_label && title_font) {
            m_welcome_label->Render(renderer, title_font);
        }
        if (m_network_status_label && body_font) {
            m_network_status_label->Render(renderer, body_font);
        }
        if (m_sync_status_label && body_font) {
            m_sync_status_label->Render(renderer, body_font);
        }
        
        // Status bar labels
        if (m_connection_label && body_font) {
            m_connection_label->Render(renderer, body_font);
        }
        if (m_block_height_label && body_font) {
            m_block_height_label->Render(renderer, body_font);
        }
    }
}

void MainScreen::OnActivate()
{
    std::cout << "Main screen activated" << std::endl;
    UpdateWalletInfo();
    UpdateNetworkInfo();
}

void MainScreen::OnResize(int new_width, int new_height)
{
    // Recreate layout with new dimensions
    std::cout << "Main screen resizing to: " << new_width << "x" << new_height << std::endl;
    CreateLayout();
    SetupButtonCallbacks();
}

void MainScreen::CreateHeaderPanel()
{
    if (!m_ui_factory || !m_layout_manager) return;
    
    int width = m_gui.GetRenderer()->GetWidth();
    int height = m_gui.GetRenderer()->GetHeight();
    
    // Create header panel with consistent styling
    Rect header_bounds = LayoutManager::StandardLayouts::GetHeaderBounds(width, height);
    m_header_panel = m_ui_factory->CreatePanel(header_bounds, PanelStyle::HEADER);
    
    // Create header components with factory
    m_title_label = m_ui_factory->CreateLabel("GOTHAM CITY", Point(0, 0), LabelStyle::TITLE);
    m_balance_label = m_ui_factory->CreateLabel("Balance: 0.00000000 GTC", Point(0, 0), LabelStyle::BODY);
    m_settings_button = m_ui_factory->CreateButton("Settings", Rect(0, 0, 100, 40), ButtonStyle::SECONDARY);
    
    // Setup layout items for responsive positioning
    m_header_items.clear();
    m_header_items.resize(3);
    
    // Title item
    m_header_items[0].constraints.preferred_width = 200;
    m_header_items[0].on_bounds_changed = [this](const Rect& bounds) {
        if (m_title_label) {
            m_title_label->SetPosition(Point(bounds.x, bounds.y + bounds.h / 2 - 12));
        }
    };
    
    // Balance item
    m_header_items[1].constraints.preferred_width = 250;
    m_header_items[1].on_bounds_changed = [this](const Rect& bounds) {
        if (m_balance_label) {
            m_balance_label->SetPosition(Point(bounds.x, bounds.y + bounds.h / 2 - 8));
        }
    };
    
    // Settings button item
    m_header_items[2].constraints.preferred_width = 100;
    m_header_items[2].on_bounds_changed = [this](const Rect& bounds) {
        if (m_settings_button) {
            m_settings_button->SetBounds(Rect(bounds.x, bounds.y + 20, bounds.w, 40));
        }
    };
    
    // Apply header layout
    m_layout_manager->CreateHeaderLayout(header_bounds, m_header_items);
}

void MainScreen::CreateSidebarPanel()
{
    if (!m_ui_factory || !m_layout_manager) return;
    
    int width = m_gui.GetRenderer()->GetWidth();
    int height = m_gui.GetRenderer()->GetHeight();
    
    // Create sidebar panel with consistent styling
    Rect sidebar_bounds = LayoutManager::StandardLayouts::GetSidebarBounds(width, height);
    m_sidebar_panel = m_ui_factory->CreatePanel(sidebar_bounds, PanelStyle::SIDEBAR);
    
    // Create navigation buttons with factory
    m_wallet_button = m_ui_factory->CreateNavigationButton("Wallet", Rect(0, 0, 180, 50), m_active_screen == "wallet");
    m_send_button = m_ui_factory->CreateNavigationButton("Send", Rect(0, 0, 180, 50), m_active_screen == "send");
    m_receive_button = m_ui_factory->CreateNavigationButton("Receive", Rect(0, 0, 180, 50), m_active_screen == "receive");
    m_transactions_button = m_ui_factory->CreateNavigationButton("Transactions", Rect(0, 0, 180, 50), m_active_screen == "transactions");
    m_console_button = m_ui_factory->CreateNavigationButton("Console", Rect(0, 0, 180, 50), m_active_screen == "console");
    
    // Setup layout items for responsive positioning
    m_sidebar_items.clear();
    m_sidebar_items.resize(5);
    
    // Setup button layout items
    std::vector<std::unique_ptr<Button>*> buttons = {
        &m_wallet_button, &m_send_button, &m_receive_button, &m_transactions_button, &m_console_button
    };
    
    for (size_t i = 0; i < buttons.size(); ++i) {
        m_sidebar_items[i].constraints.preferred_height = m_ui_factory->GetStandardHeight("sidebar_button");
        m_sidebar_items[i].margin = Margin(4, 10, 4, 10); // Add consistent margins
        
        // Capture button pointer for callback
        auto* button_ptr = buttons[i]->get();
        m_sidebar_items[i].on_bounds_changed = [button_ptr](const Rect& bounds) {
            if (button_ptr) {
                button_ptr->SetBounds(bounds);
            }
        };
    }
    
    // Apply sidebar layout with consistent spacing
    int spacing = m_ui_factory->GetSpacing("sm");
    m_layout_manager->CreateSidebarLayout(sidebar_bounds, m_sidebar_items, spacing);
}

void MainScreen::CreateMainPanel()
{
    if (!m_ui_factory || !m_layout_manager) return;
    
    int width = m_gui.GetRenderer()->GetWidth();
    int height = m_gui.GetRenderer()->GetHeight();
    
    // Create main panel with consistent styling
    Rect main_bounds = LayoutManager::StandardLayouts::GetMainContentBounds(width, height);
    m_main_panel = m_ui_factory->CreatePanel(main_bounds, PanelStyle::MAIN);
    
    // Create main content labels with factory and consistent styling
    m_welcome_label = m_ui_factory->CreateLabel("Welcome to Gotham City", Point(0, 0), LabelStyle::TITLE);
    m_network_status_label = m_ui_factory->CreateLabel("Network: Connecting...", Point(0, 0), LabelStyle::STATUS);
    m_sync_status_label = m_ui_factory->CreateLabel("Synchronization: Starting...", Point(0, 0), LabelStyle::STATUS);
    
    // Setup layout for main content with consistent spacing
    std::vector<LayoutItem> main_items(3);
    
    // Welcome label
    main_items[0].constraints.preferred_height = 40;
    main_items[0].margin = Margin(m_ui_factory->GetSpacing("lg"));
    main_items[0].on_bounds_changed = [this](const Rect& bounds) {
        if (m_welcome_label) {
            m_welcome_label->SetPosition(Point(bounds.x, bounds.y));
        }
    };
    
    // Network status label
    main_items[1].constraints.preferred_height = 30;
    main_items[1].margin = Margin(m_ui_factory->GetSpacing("md"), m_ui_factory->GetSpacing("lg"));
    main_items[1].on_bounds_changed = [this](const Rect& bounds) {
        if (m_network_status_label) {
            m_network_status_label->SetPosition(Point(bounds.x, bounds.y));
        }
    };
    
    // Sync status label
    main_items[2].constraints.preferred_height = 30;
    main_items[2].margin = Margin(m_ui_factory->GetSpacing("sm"), m_ui_factory->GetSpacing("lg"));
    main_items[2].on_bounds_changed = [this](const Rect& bounds) {
        if (m_sync_status_label) {
            m_sync_status_label->SetPosition(Point(bounds.x, bounds.y));
        }
    };
    
    // Apply vertical layout for main content
    m_layout_manager->CreateVerticalLayout(main_bounds, main_items, m_ui_factory->GetSpacing("md"), Alignment::START);
}

void MainScreen::CreateStatusPanel()
{
    if (!m_ui_factory || !m_layout_manager) return;
    
    int width = m_gui.GetRenderer()->GetWidth();
    int height = m_gui.GetRenderer()->GetHeight();
    
    // Create status panel with consistent styling
    Rect status_bounds = LayoutManager::StandardLayouts::GetStatusBarBounds(width, height);
    m_status_panel = m_ui_factory->CreatePanel(status_bounds, PanelStyle::STATUS);
    
    // Create status labels with factory
    m_connection_label = m_ui_factory->CreateLabel("Connections: 0", Point(0, 0), LabelStyle::CAPTION);
    m_block_height_label = m_ui_factory->CreateLabel("Block: 0", Point(0, 0), LabelStyle::CAPTION);
    
    // Setup layout items for status bar
    m_status_items.clear();
    m_status_items.resize(2);
    
    // Connection label
    m_status_items[0].constraints.preferred_width = 150;
    m_status_items[0].margin = Margin(0, m_ui_factory->GetSpacing("sm"));
    m_status_items[0].on_bounds_changed = [this](const Rect& bounds) {
        if (m_connection_label) {
            m_connection_label->SetPosition(Point(bounds.x, bounds.y + bounds.h / 2 - 6));
        }
    };
    
    // Block height label
    m_status_items[1].constraints.preferred_width = 150;
    m_status_items[1].margin = Margin(0, m_ui_factory->GetSpacing("sm"));
    m_status_items[1].on_bounds_changed = [this](const Rect& bounds) {
        if (m_block_height_label) {
            m_block_height_label->SetPosition(Point(bounds.x, bounds.y + bounds.h / 2 - 6));
        }
    };
    
    // Apply status bar layout
    m_layout_manager->CreateStatusBarLayout(status_bounds, m_status_items);
}

void MainScreen::SetupButtonCallbacks()
{
    if (m_wallet_button) {
        m_wallet_button->SetOnClick([this]() { OnWalletClicked(); });
    }
    if (m_send_button) {
        m_send_button->SetOnClick([this]() { OnSendClicked(); });
    }
    if (m_receive_button) {
        m_receive_button->SetOnClick([this]() { OnReceiveClicked(); });
    }
    if (m_transactions_button) {
        m_transactions_button->SetOnClick([this]() { OnTransactionsClicked(); });
    }
    if (m_console_button) {
        m_console_button->SetOnClick([this]() { OnConsoleClicked(); });
    }
    if (m_settings_button) {
        m_settings_button->SetOnClick([this]() { OnSettingsClicked(); });
    }
}

void MainScreen::UpdateWalletInfo()
{
    // In a real implementation, this would query the actual wallet
    // For now, simulate some data
    static double balance = 0.12345678;
    balance += 0.00000001; // Simulate small changes
    
    std::ostringstream oss;
    oss << "Balance: " << std::fixed << std::setprecision(8) << balance << " GTC";
    
    if (m_balance_label) {
        m_balance_label->SetText(oss.str());
    }
}

void MainScreen::UpdateNetworkInfo()
{
    // Simulate network info updates
    m_connection_count = (m_connection_count + 1) % 8 + 1;
    m_current_block_height += 1;
    
    if (m_connection_label) {
        m_connection_label->SetText("Connections: " + std::to_string(m_connection_count));
    }
    
    if (m_block_height_label) {
        m_block_height_label->SetText("Block: " + std::to_string(m_current_block_height));
    }
    
    if (m_network_status_label) {
        std::string status = m_connection_count > 0 ? "Network: Connected" : "Network: Disconnected";
        m_network_status_label->SetText(status);
        m_network_status_label->SetColor(m_connection_count > 0 ? 
            Color(46, 160, 67, 255) : Color(220, 53, 69, 255));
    }
    
    if (m_sync_status_label) {
        m_sync_status_label->SetText("Synchronization: Up to date");
        m_sync_status_label->SetColor(Color(46, 160, 67, 255));
    }
}

void MainScreen::RenderBackground(Renderer& renderer)
{
    // Dark background with subtle gradient
    renderer.Clear(Color(15, 15, 20, 255));
    
    // Add some atmospheric grid lines
    int width = renderer.GetWidth();
    int height = renderer.GetHeight();
    
    renderer.SetDrawColor(Color(25, 25, 35, 100));
    
    // Vertical lines
    for (int x = 0; x < width; x += 50) {
        renderer.DrawLine(Point(x, 0), Point(x, height));
    }
    
    // Horizontal lines
    for (int y = 0; y < height; y += 50) {
        renderer.DrawLine(Point(0, y), Point(width, y));
    }
}

// Navigation callbacks
void MainScreen::OnWalletClicked()
{
    std::cout << "Switching to wallet screen" << std::endl;
    m_gui.SwitchScreen(ScreenType::WALLET);
}

void MainScreen::OnSendClicked()
{
    std::cout << "Switching to send screen" << std::endl;
    m_gui.SwitchScreen(ScreenType::SEND);
}

void MainScreen::OnReceiveClicked()
{
    std::cout << "Switching to receive screen" << std::endl;
    m_gui.SwitchScreen(ScreenType::RECEIVE);
}

void MainScreen::OnTransactionsClicked()
{
    std::cout << "Switching to transactions screen" << std::endl;
    m_gui.SwitchScreen(ScreenType::TRANSACTIONS);
}

void MainScreen::OnConsoleClicked()
{
    std::cout << "Switching to console screen" << std::endl;
    m_gui.SwitchScreen(ScreenType::CONSOLE);
}

void MainScreen::OnSettingsClicked()
{
    std::cout << "Switching to settings screen" << std::endl;
    m_gui.SwitchScreen(ScreenType::SETTINGS);
}

void MainScreen::UpdateActiveScreen(const std::string& screen_name)
{
    if (m_active_screen != screen_name) {
        m_active_screen = screen_name;
        RefreshNavigationButtons();
    }
}

void MainScreen::RefreshNavigationButtons()
{
    if (!m_ui_factory) return;
    
    // Update navigation button styles based on active screen
    if (m_wallet_button) {
        if (m_active_screen == "wallet") {
            m_ui_factory->CreateNavigationButton("Wallet", m_wallet_button->GetBounds(), true);
        } else {
            m_ui_factory->CreateNavigationButton("Wallet", m_wallet_button->GetBounds(), false);
        }
    }
    
    if (m_send_button) {
        if (m_active_screen == "send") {
            m_ui_factory->CreateNavigationButton("Send", m_send_button->GetBounds(), true);
        } else {
            m_ui_factory->CreateNavigationButton("Send", m_send_button->GetBounds(), false);
        }
    }
    
    if (m_receive_button) {
        if (m_active_screen == "receive") {
            m_ui_factory->CreateNavigationButton("Receive", m_receive_button->GetBounds(), true);
        } else {
            m_ui_factory->CreateNavigationButton("Receive", m_receive_button->GetBounds(), false);
        }
    }
    
    if (m_transactions_button) {
        if (m_active_screen == "transactions") {
            m_ui_factory->CreateNavigationButton("Transactions", m_transactions_button->GetBounds(), true);
        } else {
            m_ui_factory->CreateNavigationButton("Transactions", m_transactions_button->GetBounds(), false);
        }
    }
    
    if (m_console_button) {
        if (m_active_screen == "console") {
            m_ui_factory->CreateNavigationButton("Console", m_console_button->GetBounds(), true);
        } else {
            m_ui_factory->CreateNavigationButton("Console", m_console_button->GetBounds(), false);
        }
    }
}