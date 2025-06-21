// Copyright (c) 2025 The Gotham Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or https://opensource.org/license/mit/.

#include "receive_screen.h"
#include "../gotham_city_gui.h"
#include "../renderer.h"
#include "../utils/theme.h"
#include "../utils/font_manager.h"
#include <iostream>
#include <random>
#include <algorithm>

ReceiveScreen::ReceiveScreen(GothamCityGUI& gui) : Screen(gui)
{
}

bool ReceiveScreen::Initialize()
{
    if (m_initialized) {
        return true;
    }

    GenerateNewAddress();
    CreateLayout();
    SetupButtonCallbacks();
    m_initialized = true;
    std::cout << "Receive screen initialized" << std::endl;
    return true;
}

void ReceiveScreen::HandleEvent(const SDL_Event& event)
{
    if (event.type == SDL_KEYDOWN) {
        switch (event.key.keysym.sym) {
            case SDLK_ESCAPE:
                m_gui.SwitchScreen(ScreenType::WALLET);
                return;
            case SDLK_c:
                if (event.key.keysym.mod & KMOD_CTRL) {
                    std::cout << "Address copied to clipboard: " << m_current_address << std::endl;
                    return;
                }
                break;
            case SDLK_n:
                if (event.key.keysym.mod & KMOD_CTRL) {
                    GenerateNewAddress();
                    std::cout << "New address generated: " << m_current_address << std::endl;
                    return;
                }
                break;
        }
    }
}

void ReceiveScreen::Update(float delta_time)
{
    m_elapsed_time += delta_time;
}

void ReceiveScreen::Render(Renderer& renderer)
{
    // Clear with dark background
    renderer.Clear(Color(15, 15, 20, 255));
    
    // Draw header panel
    renderer.SetDrawColor(Color(25, 25, 35, 255));
    renderer.FillRect(Rect(0, 0, renderer.GetWidth(), 80));
    renderer.SetDrawColor(Color(255, 215, 0, 255));
    renderer.DrawRect(Rect(0, 0, renderer.GetWidth(), 80));
    
    // Title and back button
    renderer.DrawText("Receive Bitcoin", 50, 30, Color(255, 215, 0, 255), 24);
    renderer.DrawText("Back (ESC)", renderer.GetWidth() - 150, 30, Color(200, 200, 200, 255), 16);
    
    // Address panel
    int panel_width = std::min(700, renderer.GetWidth() - 40);
    int panel_x = (renderer.GetWidth() - panel_width) / 2;
    renderer.SetDrawColor(Color(25, 25, 35, 255));
    renderer.FillRect(Rect(panel_x, 100, panel_width, 180));
    renderer.SetDrawColor(Color(255, 215, 0, 255));
    renderer.DrawRect(Rect(panel_x, 100, panel_width, 180));
    
    // Address section
    renderer.DrawText("Your Bitcoin Address", panel_x + 20, 120, Color(255, 215, 0, 255), 18);
    renderer.DrawText(m_current_address.c_str(), panel_x + 20, 150, Color(255, 255, 255, 255), 14);
    
    // Buttons
    renderer.SetDrawColor(Color(46, 160, 67, 255));
    renderer.FillRect(Rect(panel_x + 20, 180, 100, 35));
    renderer.DrawText("Copy (Ctrl+C)", panel_x + 30, 190, Color(255, 255, 255, 255), 14);
    
    renderer.SetDrawColor(Color(0, 123, 255, 255));
    renderer.FillRect(Rect(panel_x + 140, 180, 120, 35));
    renderer.DrawText("New (Ctrl+N)", panel_x + 150, 190, Color(255, 255, 255, 255), 14);
    
    // QR Code section
    renderer.SetDrawColor(Color(20, 20, 30, 255));
    renderer.FillRect(Rect(panel_x, 300, panel_width, 150));
    renderer.SetDrawColor(Color(60, 60, 70, 255));
    renderer.DrawRect(Rect(panel_x, 300, panel_width, 150));
    
    renderer.DrawText("QR Code", panel_x + 20, 320, Color(255, 255, 255, 255), 18);
    renderer.DrawText("QR Code would be displayed here", panel_x + 20, 350, Color(150, 150, 150, 255), 14);
    renderer.DrawText("Share this address to receive Bitcoin", panel_x + 20, 380, Color(150, 150, 150, 255), 14);
    
    // Instructions
    renderer.DrawText("Controls: ESC=Back, Ctrl+C=Copy, Ctrl+N=New Address", 50, renderer.GetHeight() - 40, Color(150, 150, 150, 255), 12);
}

void ReceiveScreen::OnActivate()
{
    std::cout << "Receive screen activated" << std::endl;
}

void ReceiveScreen::OnResize(int new_width, int new_height)
{
    std::cout << "Receive screen resizing to: " << new_width << "x" << new_height << std::endl;
}

void ReceiveScreen::GenerateNewAddress()
{
    // Generate a mock Bitcoin address
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 15);
    
    m_current_address = "bc1q";
    for (int i = 0; i < 39; ++i) {
        char c = dis(gen) < 10 ? '0' + dis(gen) % 10 : 'a' + dis(gen) % 6;
        m_current_address += c;
    }
}

void ReceiveScreen::CreateLayout()
{
    CreateHeaderPanel();
    CreateAddressPanel();
    CreateQRPanel();
}

void ReceiveScreen::CreateHeaderPanel()
{
    // Header panel creation logic would go here
    // For now, we'll use the direct rendering approach in Render()
}

void ReceiveScreen::CreateAddressPanel()
{
    // Address panel creation logic would go here
    // For now, we'll use the direct rendering approach in Render()
}

void ReceiveScreen::CreateQRPanel()
{
    // QR panel creation logic would go here
    // For now, we'll use the direct rendering approach in Render()
}

void ReceiveScreen::SetupButtonCallbacks()
{
    // Button callback setup would go here
    // For now, we handle events directly in HandleEvent()
}

void ReceiveScreen::OnBackClicked()
{
    m_gui.SwitchScreen(ScreenType::WALLET);
}

void ReceiveScreen::OnCopyClicked()
{
    std::cout << "Address copied to clipboard: " << m_current_address << std::endl;
}

void ReceiveScreen::OnNewAddressClicked()
{
    GenerateNewAddress();
    std::cout << "New address generated: " << m_current_address << std::endl;
}

Color ReceiveScreen::GetGothamGoldColor() const
{
    return Color(255, 215, 0, 255);
}

Color ReceiveScreen::GetGothamDarkColor() const
{
    return Color(15, 15, 20, 255);
}

