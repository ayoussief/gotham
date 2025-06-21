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
    int width = m_gui.GetRenderer()->GetWidth();
    m_header_panel = std::make_unique<Panel>(Rect(0, 0, width, 80));
    m_header_panel->SetBackgroundColor(Color(25, 25, 35, 255));
    m_header_panel->SetBorderColor(Color(255, 215, 0, 255));
    m_header_panel->SetBorderWidth(2);
    
    // Title
    m_title_label = std::make_unique<Label>("GOTHAM CITY", Point(20, 25));
    m_title_label->SetColor(Color(255, 215, 0, 255));
    
    // Balance
    m_balance_label = std::make_unique<Label>("Balance: 0.00000000 GTC", Point(width - 300, 25));
    m_balance_label->SetColor(Color(200, 200, 200, 255));
    
    // Settings button
    m_settings_button = std::make_unique<Button>("Settings", Rect(width - 120, 20, 100, 40));
    m_settings_button->SetColors(Color(70, 130, 180, 255), Color(100, 149, 237, 255), Color(25, 25, 112, 255));
}

void MainScreen::CreateSidebarPanel()
{
    int height = m_gui.GetRenderer()->GetHeight();
    m_sidebar_panel = std::make_unique<Panel>(Rect(0, 80, 200, height - 110));
    m_sidebar_panel->SetBackgroundColor(Color(20, 20, 30, 255));
    m_sidebar_panel->SetBorderColor(Color(60, 60, 75, 255));
    
    // Navigation buttons
    int button_y = 100;
    int button_spacing = 60;
    
    m_wallet_button = std::make_unique<Button>("Wallet", Rect(10, button_y, 180, 50));
    m_wallet_button->SetPrimary(true);
    
    m_send_button = std::make_unique<Button>("Send", Rect(10, button_y + button_spacing, 180, 50));
    
    m_receive_button = std::make_unique<Button>("Receive", Rect(10, button_y + button_spacing * 2, 180, 50));
    
    m_transactions_button = std::make_unique<Button>("Transactions", Rect(10, button_y + button_spacing * 3, 180, 50));
    
    m_console_button = std::make_unique<Button>("Console", Rect(10, button_y + button_spacing * 4, 180, 50));
}

void MainScreen::CreateMainPanel()
{
    int width = m_gui.GetRenderer()->GetWidth();
    int height = m_gui.GetRenderer()->GetHeight();
    
    m_main_panel = std::make_unique<Panel>(Rect(200, 80, width - 200, height - 110));
    m_main_panel->SetBackgroundColor(Color(15, 15, 20, 255));
    
    // Welcome message
    m_welcome_label = std::make_unique<Label>("Welcome to Gotham City", Point(250, 120));
    m_welcome_label->SetColor(Color(255, 215, 0, 255));
    
    // Network status
    m_network_status_label = std::make_unique<Label>("Network: Connecting...", Point(250, 180));
    m_network_status_label->SetColor(Color(200, 200, 200, 255));
    
    // Sync status
    m_sync_status_label = std::make_unique<Label>("Synchronization: Starting...", Point(250, 210));
    m_sync_status_label->SetColor(Color(200, 200, 200, 255));
}

void MainScreen::CreateStatusPanel()
{
    int width = m_gui.GetRenderer()->GetWidth();
    int height = m_gui.GetRenderer()->GetHeight();
    
    m_status_panel = std::make_unique<Panel>(Rect(0, height - 30, width, 30));
    m_status_panel->SetBackgroundColor(Color(30, 30, 40, 255));
    m_status_panel->SetBorderColor(Color(60, 60, 75, 255));
    
    // Connection status
    m_connection_label = std::make_unique<Label>("Connections: 0", Point(10, height - 25));
    m_connection_label->SetColor(Color(150, 150, 150, 255));
    
    // Block height
    m_block_height_label = std::make_unique<Label>("Block: 0", Point(150, height - 25));
    m_block_height_label->SetColor(Color(150, 150, 150, 255));
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