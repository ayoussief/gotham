// Copyright (c) 2025 The Gotham Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or https://opensource.org/license/mit/.

#include "settings_screen.h"
#include "../gotham_city_gui.h"
#include "../renderer.h"
#include "../utils/theme.h"
#include "../utils/font_manager.h"
#include <iostream>
#include <algorithm>

SettingsScreen::SettingsScreen(GothamCityGUI& gui) : Screen(gui)
{
}

bool SettingsScreen::Initialize()
{
    if (m_initialized) {
        return true;
    }

    CreateLayout();
    SetupButtonCallbacks();
    m_initialized = true;
    std::cout << "Settings screen initialized" << std::endl;
    return true;
}

void SettingsScreen::HandleEvent(const SDL_Event& event)
{
    if (event.type == SDL_KEYDOWN) {
        switch (event.key.keysym.sym) {
            case SDLK_ESCAPE:
                m_gui.SwitchScreen(ScreenType::MAIN);
                return;
        }
    }
}

void SettingsScreen::Update(float delta_time)
{
    m_elapsed_time += delta_time;
}

void SettingsScreen::Render(Renderer& renderer)
{
    // Clear with dark background
    renderer.Clear(Color(15, 15, 20, 255));
    
    // Draw header panel
    renderer.SetDrawColor(Color(25, 25, 35, 255));
    renderer.FillRect(Rect(0, 0, renderer.GetWidth(), 80));
    renderer.SetDrawColor(Color(255, 215, 0, 255));
    renderer.DrawRect(Rect(0, 0, renderer.GetWidth(), 80));
    
    // Title and back button
    renderer.DrawText("Settings", 50, 30, Color(255, 215, 0, 255), 24);
    renderer.DrawText("Back (ESC)", renderer.GetWidth() - 150, 30, Color(200, 200, 200, 255), 16);
    
    // Network settings panel
    int panel_width = std::min(600, renderer.GetWidth() - 40);
    int panel_x = (renderer.GetWidth() - panel_width) / 2;
    renderer.SetDrawColor(Color(25, 25, 35, 255));
    renderer.FillRect(Rect(panel_x, 100, panel_width, 120));
    renderer.SetDrawColor(Color(255, 215, 0, 255));
    renderer.DrawRect(Rect(panel_x, 100, panel_width, 120));
    
    renderer.DrawText("Network Settings", panel_x + 20, 120, Color(255, 215, 0, 255), 18);
    renderer.DrawText("• Network: Mainnet", panel_x + 40, 150, Color(200, 200, 200, 255), 14);
    renderer.DrawText("• RPC Port: 8332", panel_x + 40, 175, Color(200, 200, 200, 255), 14);
    renderer.DrawText("• Proxy: None", panel_x + 40, 200, Color(200, 200, 200, 255), 14);
    
    // Display settings panel
    renderer.SetDrawColor(Color(25, 25, 35, 255));
    renderer.FillRect(Rect(panel_x, 240, panel_width, 120));
    renderer.SetDrawColor(Color(60, 60, 70, 255));
    renderer.DrawRect(Rect(panel_x, 240, panel_width, 120));
    
    renderer.DrawText("Display Settings", panel_x + 20, 260, Color(255, 215, 0, 255), 18);
    renderer.DrawText("• Theme: Gotham Dark", panel_x + 40, 290, Color(200, 200, 200, 255), 14);
    renderer.DrawText("• Language: English", panel_x + 40, 315, Color(200, 200, 200, 255), 14);
    renderer.DrawText("• Currency: BTC", panel_x + 40, 340, Color(200, 200, 200, 255), 14);
    
    // Security settings panel
    renderer.SetDrawColor(Color(25, 25, 35, 255));
    renderer.FillRect(Rect(panel_x, 380, panel_width, 95));
    renderer.SetDrawColor(Color(60, 60, 70, 255));
    renderer.DrawRect(Rect(panel_x, 380, panel_width, 95));
    
    renderer.DrawText("Security", panel_x + 20, 400, Color(255, 215, 0, 255), 18);
    renderer.DrawText("• Wallet Encryption: Enabled", panel_x + 40, 430, Color(46, 160, 67, 255), 14);
    renderer.DrawText("• Auto-lock: 10 minutes", panel_x + 40, 455, Color(200, 200, 200, 255), 14);
    
    // About panel
    renderer.SetDrawColor(Color(25, 25, 35, 255));
    renderer.FillRect(Rect(panel_x, 495, panel_width, 95));
    renderer.SetDrawColor(Color(60, 60, 70, 255));
    renderer.DrawRect(Rect(panel_x, 495, panel_width, 95));
    
    renderer.DrawText("About", panel_x + 20, 515, Color(255, 215, 0, 255), 18);
    renderer.DrawText("• Gotham City Wallet v1.0.0", panel_x + 40, 545, Color(200, 200, 200, 255), 14);
    renderer.DrawText("• Bitcoin Core v25.0", panel_x + 40, 570, Color(200, 200, 200, 255), 14);
    
    // Instructions
    renderer.DrawText("Press ESC to go back to main screen", 50, renderer.GetHeight() - 30, Color(150, 150, 150, 255), 12);
}

void SettingsScreen::OnActivate()
{
    std::cout << "Settings screen activated" << std::endl;
}

void SettingsScreen::OnResize(int new_width, int new_height)
{
    std::cout << "Settings screen resizing to: " << new_width << "x" << new_height << std::endl;
}

void SettingsScreen::CreateLayout()
{
    CreateHeaderPanel();
    CreateNetworkPanel();
    CreateDisplayPanel();
    CreateSecurityPanel();
    CreateAboutPanel();
}

void SettingsScreen::CreateHeaderPanel()
{
    // Header panel creation logic would go here
    // For now, we'll use the direct rendering approach in Render()
}

void SettingsScreen::CreateNetworkPanel()
{
    // Network panel creation logic would go here
    // For now, we'll use the direct rendering approach in Render()
}

void SettingsScreen::CreateDisplayPanel()
{
    // Display panel creation logic would go here
    // For now, we'll use the direct rendering approach in Render()
}

void SettingsScreen::CreateSecurityPanel()
{
    // Security panel creation logic would go here
    // For now, we'll use the direct rendering approach in Render()
}

void SettingsScreen::CreateAboutPanel()
{
    // About panel creation logic would go here
    // For now, we'll use the direct rendering approach in Render()
}

void SettingsScreen::SetupButtonCallbacks()
{
    // Button callback setup would go here
    // For now, we handle events directly in HandleEvent()
}

void SettingsScreen::OnBackClicked()
{
    m_gui.SwitchScreen(ScreenType::MAIN);
}

Color SettingsScreen::GetGothamGoldColor() const
{
    return Color(255, 215, 0, 255);
}

Color SettingsScreen::GetGothamDarkColor() const
{
    return Color(15, 15, 20, 255);
}

