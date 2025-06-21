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
    // Initialize UI systems
    if (auto* theme_manager = m_gui.GetThemeManager()) {
        m_ui_factory = std::make_unique<UIFactory>(*theme_manager);
    }
    m_layout_manager = std::make_unique<LayoutManager>();
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
    
    // Pass events to UI components
    if (m_back_button) m_back_button->HandleEvent(event);
}

void SettingsScreen::Update(float delta_time)
{
    m_elapsed_time += delta_time;
}

void SettingsScreen::Render(Renderer& renderer)
{
    // Render background with consistent theme
    renderer.Clear(UIStyleGuide::Colors::BACKGROUND_DARK);
    
    // Render UI panels
    if (m_header_panel) m_header_panel->Render(renderer);
    if (m_network_panel) m_network_panel->Render(renderer);
    if (m_display_panel) m_display_panel->Render(renderer);
    if (m_security_panel) m_security_panel->Render(renderer);
    if (m_about_panel) m_about_panel->Render(renderer);
    
    // Get fonts from font manager
    FontManager* font_manager = m_gui.GetFontManager();
    if (font_manager) {
        TTF_Font* title_font = font_manager->GetDefaultFont(24);
        TTF_Font* heading_font = font_manager->GetDefaultFont(18);
        TTF_Font* body_font = font_manager->GetDefaultFont(14);
        
        // Render header components
        if (m_title_label) m_title_label->Render(renderer, title_font);
        if (m_back_button) m_back_button->Render(renderer, body_font);
        
        // Render network settings
        if (m_network_title_label) m_network_title_label->Render(renderer, heading_font);
        if (m_network_status_label) m_network_status_label->Render(renderer, body_font);
        if (m_rpc_port_label) m_rpc_port_label->Render(renderer, body_font);
        if (m_proxy_label) m_proxy_label->Render(renderer, body_font);
        
        // Render display settings
        if (m_display_title_label) m_display_title_label->Render(renderer, heading_font);
        if (m_theme_label) m_theme_label->Render(renderer, body_font);
        if (m_language_label) m_language_label->Render(renderer, body_font);
        if (m_currency_label) m_currency_label->Render(renderer, body_font);
        
        // Render security settings
        if (m_security_title_label) m_security_title_label->Render(renderer, heading_font);
        if (m_encryption_label) m_encryption_label->Render(renderer, body_font);
        if (m_autolock_label) m_autolock_label->Render(renderer, body_font);
        
        // Render about section
        if (m_about_title_label) m_about_title_label->Render(renderer, heading_font);
        if (m_version_label) m_version_label->Render(renderer, body_font);
        if (m_core_version_label) m_core_version_label->Render(renderer, body_font);
    }
}

void SettingsScreen::OnActivate()
{
    std::cout << "Settings screen activated" << std::endl;
}

void SettingsScreen::OnResize(int new_width, int new_height)
{
    std::cout << "Settings screen resizing to: " << new_width << "x" << new_height << std::endl;
    CreateLayout();
}

void SettingsScreen::CreateLayout()
{
    if (!m_ui_factory || !m_layout_manager) return;
    
    int screen_width = m_gui.GetRenderer()->GetWidth();
    int screen_height = m_gui.GetRenderer()->GetHeight();
    
    // Calculate responsive panel dimensions
    int panel_width = std::min(600, screen_width - UIStyleGuide::Spacing::XL * 2);
    int panel_x = (screen_width - panel_width) / 2;
    
    // Calculate vertical positions with proper spacing
    int header_height = UIStyleGuide::Dimensions::HEADER_HEIGHT;
    int current_y = header_height + UIStyleGuide::Spacing::LG;
    int panel_spacing = UIStyleGuide::Spacing::MD;
    
    CreateHeaderPanel();
    CreateNetworkPanel(panel_x, current_y, panel_width);
    current_y += 120 + panel_spacing;
    
    CreateDisplayPanel(panel_x, current_y, panel_width);
    current_y += 120 + panel_spacing;
    
    CreateSecurityPanel(panel_x, current_y, panel_width);
    current_y += 95 + panel_spacing;
    
    CreateAboutPanel(panel_x, current_y, panel_width);
}

void SettingsScreen::CreateHeaderPanel()
{
    if (!m_ui_factory || !m_layout_manager) return;
    
    int width = m_gui.GetRenderer()->GetWidth();
    
    // Create header panel with consistent styling
    Rect header_bounds = Rect(0, 0, width, UIStyleGuide::Dimensions::HEADER_HEIGHT);
    m_header_panel = m_ui_factory->CreatePanel(header_bounds, PanelStyle::HEADER);
    
    // Create header components
    m_title_label = m_ui_factory->CreateLabel("⚙️ Settings", Point(0, 0), LabelStyle::TITLE);
    m_back_button = m_ui_factory->CreateButton("← Back", Rect(0, 0, 100, 40), ButtonStyle::SECONDARY);
    
    // Setup layout for header components
    std::vector<LayoutItem> header_items(2);
    
    // Title item
    header_items[0].constraints.preferred_width = 300;
    header_items[0].margin = Margin(0, UIStyleGuide::Spacing::LG);
    header_items[0].on_bounds_changed = [this](const Rect& bounds) {
        if (m_title_label) {
            m_title_label->SetPosition(Point(bounds.x, bounds.y + bounds.h / 2 - 12));
        }
    };
    
    // Back button
    header_items[1].constraints.preferred_width = 100;
    header_items[1].margin = Margin(UIStyleGuide::Spacing::SM);
    header_items[1].on_bounds_changed = [this](const Rect& bounds) {
        if (m_back_button) {
            m_back_button->SetBounds(bounds);
        }
    };
    
    // Apply horizontal layout for header
    m_layout_manager->CreateHorizontalLayout(header_bounds, header_items, 
                                           UIStyleGuide::Spacing::MD, Alignment::CENTER);
}

void SettingsScreen::CreateNetworkPanel(int x, int y, int width)
{
    if (!m_ui_factory || !m_layout_manager) return;
    
    // Create network panel
    Rect network_bounds = Rect(x, y, width, 120);
    m_network_panel = m_ui_factory->CreatePanel(network_bounds, PanelStyle::CARD);
    
    // Create network components with proper spacing
    m_network_title_label = m_ui_factory->CreateLabel("Network Settings", Point(x + UIStyleGuide::Spacing::LG, y + UIStyleGuide::Spacing::LG), LabelStyle::HEADING);
    m_network_title_label->SetColor(UIStyleGuide::Colors::GOTHAM_GOLD);
    
    int label_y = y + UIStyleGuide::Spacing::LG + 30;
    m_network_status_label = m_ui_factory->CreateLabel("• Network: Mainnet", Point(x + UIStyleGuide::Spacing::XL, label_y), LabelStyle::BODY);
    m_rpc_port_label = m_ui_factory->CreateLabel("• RPC Port: 8332", Point(x + UIStyleGuide::Spacing::XL, label_y + 25), LabelStyle::BODY);
    m_proxy_label = m_ui_factory->CreateLabel("• Proxy: None", Point(x + UIStyleGuide::Spacing::XL, label_y + 50), LabelStyle::BODY);
}

void SettingsScreen::CreateDisplayPanel(int x, int y, int width)
{
    if (!m_ui_factory || !m_layout_manager) return;
    
    // Create display panel
    Rect display_bounds = Rect(x, y, width, 120);
    m_display_panel = m_ui_factory->CreatePanel(display_bounds, PanelStyle::CARD);
    
    // Create display components with proper spacing
    m_display_title_label = m_ui_factory->CreateLabel("Display Settings", Point(x + UIStyleGuide::Spacing::LG, y + UIStyleGuide::Spacing::LG), LabelStyle::HEADING);
    m_display_title_label->SetColor(UIStyleGuide::Colors::GOTHAM_GOLD);
    
    int label_y = y + UIStyleGuide::Spacing::LG + 30;
    m_theme_label = m_ui_factory->CreateLabel("• Theme: Gotham Dark", Point(x + UIStyleGuide::Spacing::XL, label_y), LabelStyle::BODY);
    m_language_label = m_ui_factory->CreateLabel("• Language: English", Point(x + UIStyleGuide::Spacing::XL, label_y + 25), LabelStyle::BODY);
    m_currency_label = m_ui_factory->CreateLabel("• Currency: BTC", Point(x + UIStyleGuide::Spacing::XL, label_y + 50), LabelStyle::BODY);
}

void SettingsScreen::CreateSecurityPanel(int x, int y, int width)
{
    if (!m_ui_factory || !m_layout_manager) return;
    
    // Create security panel
    Rect security_bounds = Rect(x, y, width, 95);
    m_security_panel = m_ui_factory->CreatePanel(security_bounds, PanelStyle::CARD);
    
    // Create security components with proper spacing
    m_security_title_label = m_ui_factory->CreateLabel("Security", Point(x + UIStyleGuide::Spacing::LG, y + UIStyleGuide::Spacing::LG), LabelStyle::HEADING);
    m_security_title_label->SetColor(UIStyleGuide::Colors::GOTHAM_GOLD);
    
    int label_y = y + UIStyleGuide::Spacing::LG + 30;
    m_encryption_label = m_ui_factory->CreateLabel("• Wallet Encryption: Enabled", Point(x + UIStyleGuide::Spacing::XL, label_y), LabelStyle::BODY);
    m_encryption_label->SetColor(UIStyleGuide::Colors::SUCCESS);
    
    m_autolock_label = m_ui_factory->CreateLabel("• Auto-lock: 10 minutes", Point(x + UIStyleGuide::Spacing::XL, label_y + 25), LabelStyle::BODY);
}

void SettingsScreen::CreateAboutPanel(int x, int y, int width)
{
    if (!m_ui_factory || !m_layout_manager) return;
    
    // Create about panel
    Rect about_bounds = Rect(x, y, width, 95);
    m_about_panel = m_ui_factory->CreatePanel(about_bounds, PanelStyle::CARD);
    
    // Create about components with proper spacing
    m_about_title_label = m_ui_factory->CreateLabel("About", Point(x + UIStyleGuide::Spacing::LG, y + UIStyleGuide::Spacing::LG), LabelStyle::HEADING);
    m_about_title_label->SetColor(UIStyleGuide::Colors::GOTHAM_GOLD);
    
    int label_y = y + UIStyleGuide::Spacing::LG + 30;
    m_version_label = m_ui_factory->CreateLabel("• Gotham City Wallet v1.0.0", Point(x + UIStyleGuide::Spacing::XL, label_y), LabelStyle::BODY);
    m_core_version_label = m_ui_factory->CreateLabel("• Bitcoin Core v25.0", Point(x + UIStyleGuide::Spacing::XL, label_y + 25), LabelStyle::BODY);
}

void SettingsScreen::SetupButtonCallbacks()
{
    if (m_back_button) {
        m_back_button->SetOnClick([this]() { 
            OnBackClicked(); 
        });
    }
}

void SettingsScreen::OnBackClicked()
{
    m_gui.SwitchScreen(ScreenType::MAIN);
}



