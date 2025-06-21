// Copyright (c) 2025 The Gotham Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or https://opensource.org/license/mit/.

#include "settings_screen.h"
#include "../gotham_city_gui.h"
#include "../utils/theme.h"
#include "../utils/font_manager.h"
#include <iostream>

SettingsScreen::SettingsScreen(GothamCityGUI& gui) : Screen(gui)
{
    if (auto* theme_manager = m_gui.GetThemeManager()) {
        m_ui_factory = std::make_unique<UIFactory>(*theme_manager);
    }
    m_layout_manager = std::make_unique<LayoutManager>();
}

bool SettingsScreen::Initialize()
{
    if (m_initialized) return true;
    
    CreateContentPanel();
    SetupButtonCallbacks();
    LoadSettings();
    
    m_initialized = true;
    std::cout << "Settings screen initialized (content area only)" << std::endl;
    return true;
}

void SettingsScreen::CreateContentPanel()
{
    if (!m_ui_factory) return;
    
    m_content_panel = m_ui_factory->CreatePanel(Rect(0, 0, 800, 600), PanelStyle::CONTENT);
    CreateNetworkPanel();
    CreateSecurityPanel();
    CreateDisplayPanel();
    CreateAdvancedPanel();
    
    // Action buttons
    m_save_button = m_ui_factory->CreateButton("💾 Save", Rect(20, 550, 80, 35), ButtonStyle::PRIMARY);
    m_cancel_button = m_ui_factory->CreateButton("❌ Cancel", Rect(110, 550, 80, 35), ButtonStyle::SECONDARY);
    m_apply_button = m_ui_factory->CreateButton("✅ Apply", Rect(200, 550, 80, 35), ButtonStyle::GHOST);
}

void SettingsScreen::CreateNetworkPanel()
{
    m_network_panel = m_ui_factory->CreatePanel(Rect(20, 20, 300, 120), PanelStyle::CARD);
    m_network_title_label = m_ui_factory->CreateLabel("Network Settings", Point(30, 30), LabelStyle::SUBHEADING);
    m_mainnet_button = m_ui_factory->CreateButton("Mainnet", Rect(30, 60, 80, 30), ButtonStyle::PRIMARY);
    m_testnet_button = m_ui_factory->CreateButton("Testnet", Rect(120, 60, 80, 30), ButtonStyle::SECONDARY);
    m_regtest_button = m_ui_factory->CreateButton("Regtest", Rect(210, 60, 80, 30), ButtonStyle::SECONDARY);
    m_proxy_label = m_ui_factory->CreateLabel("Proxy:", Point(30, 100), LabelStyle::BODY);
    m_proxy_input = m_ui_factory->CreateTextInput(Rect(80, 100, 200, 25), "127.0.0.1:9050");
}

void SettingsScreen::CreateSecurityPanel()
{
    m_security_panel = m_ui_factory->CreatePanel(Rect(340, 20, 300, 120), PanelStyle::CARD);
    m_security_title_label = m_ui_factory->CreateLabel("Security", Point(350, 30), LabelStyle::SUBHEADING);
    m_encrypt_wallet_button = m_ui_factory->CreateButton("🔒 Encrypt Wallet", Rect(350, 60, 120, 25), ButtonStyle::SECONDARY);
    m_change_passphrase_button = m_ui_factory->CreateButton("🔑 Change Pass", Rect(480, 60, 120, 25), ButtonStyle::GHOST);
    m_backup_wallet_button = m_ui_factory->CreateButton("💾 Backup", Rect(350, 90, 120, 25), ButtonStyle::GHOST);
    m_verify_backup_button = m_ui_factory->CreateButton("✅ Verify", Rect(480, 90, 120, 25), ButtonStyle::GHOST);
}

void SettingsScreen::CreateDisplayPanel()
{
    m_display_panel = m_ui_factory->CreatePanel(Rect(20, 160, 300, 120), PanelStyle::CARD);
    m_display_title_label = m_ui_factory->CreateLabel("Display", Point(30, 170), LabelStyle::SUBHEADING);
    m_dark_theme_button = m_ui_factory->CreateButton("🌙 Dark", Rect(30, 200, 80, 30), ButtonStyle::PRIMARY);
    m_light_theme_button = m_ui_factory->CreateButton("☀️ Light", Rect(120, 200, 80, 30), ButtonStyle::SECONDARY);
    m_language_label = m_ui_factory->CreateLabel("Language:", Point(30, 240), LabelStyle::BODY);
    m_language_button = m_ui_factory->CreateButton("English", Rect(100, 240, 80, 25), ButtonStyle::GHOST);
    m_fullscreen_button = m_ui_factory->CreateButton("🖥️ Fullscreen", Rect(190, 240, 100, 25), ButtonStyle::GHOST);
}

void SettingsScreen::CreateAdvancedPanel()
{
    m_advanced_panel = m_ui_factory->CreatePanel(Rect(340, 160, 300, 120), PanelStyle::CARD);
    m_advanced_title_label = m_ui_factory->CreateLabel("Advanced", Point(350, 170), LabelStyle::SUBHEADING);
    m_datadir_label = m_ui_factory->CreateLabel("Data Dir:", Point(350, 200), LabelStyle::BODY);
    m_datadir_input = m_ui_factory->CreateTextInput(Rect(410, 200, 150, 25), "~/.gotham");
    m_browse_datadir_button = m_ui_factory->CreateButton("📁", Rect(570, 200, 30, 25), ButtonStyle::GHOST);
    m_reset_settings_button = m_ui_factory->CreateButton("🔄 Reset", Rect(350, 240, 80, 25), ButtonStyle::ERROR);
}

void SettingsScreen::SetupButtonCallbacks()
{
    if (m_mainnet_button) m_mainnet_button->SetOnClick([this]() { OnMainnetClicked(); });
    if (m_testnet_button) m_testnet_button->SetOnClick([this]() { OnTestnetClicked(); });
    if (m_regtest_button) m_regtest_button->SetOnClick([this]() { OnRegtestClicked(); });
    if (m_dark_theme_button) m_dark_theme_button->SetOnClick([this]() { OnDarkThemeClicked(); });
    if (m_light_theme_button) m_light_theme_button->SetOnClick([this]() { OnLightThemeClicked(); });
    if (m_save_button) m_save_button->SetOnClick([this]() { OnSaveClicked(); });
    if (m_cancel_button) m_cancel_button->SetOnClick([this]() { OnCancelClicked(); });
    if (m_apply_button) m_apply_button->SetOnClick([this]() { OnApplyClicked(); });
}

void SettingsScreen::HandleEvent(const SDL_Event& event)
{
    if (m_content_panel) m_content_panel->HandleEvent(event);
    if (m_mainnet_button) m_mainnet_button->HandleEvent(event);
    if (m_testnet_button) m_testnet_button->HandleEvent(event);
    if (m_regtest_button) m_regtest_button->HandleEvent(event);
    if (m_dark_theme_button) m_dark_theme_button->HandleEvent(event);
    if (m_light_theme_button) m_light_theme_button->HandleEvent(event);
    if (m_save_button) m_save_button->HandleEvent(event);
    if (m_cancel_button) m_cancel_button->HandleEvent(event);
    if (m_apply_button) m_apply_button->HandleEvent(event);
}

void SettingsScreen::Update(float delta_time)
{
    m_elapsed_time += delta_time;
    CheckForChanges();
}

void SettingsScreen::Render(Renderer& renderer)
{
    if (m_content_panel) m_content_panel->Render(renderer);
    if (m_network_panel) m_network_panel->Render(renderer);
    if (m_security_panel) m_security_panel->Render(renderer);
    if (m_display_panel) m_display_panel->Render(renderer);
    if (m_advanced_panel) m_advanced_panel->Render(renderer);
    
    TTF_Font* subheading_font = m_gui.GetFontManager()->GetDefaultFont(UIStyleGuide::FontSize::SUBHEADING);
    TTF_Font* body_font = m_gui.GetFontManager()->GetDefaultFont(UIStyleGuide::FontSize::BODY);
    
    if (m_network_title_label && subheading_font) m_network_title_label->Render(renderer, subheading_font);
    if (m_mainnet_button && body_font) m_mainnet_button->Render(renderer, body_font);
    if (m_testnet_button && body_font) m_testnet_button->Render(renderer, body_font);
    if (m_regtest_button && body_font) m_regtest_button->Render(renderer, body_font);
    
    if (m_security_title_label && subheading_font) m_security_title_label->Render(renderer, subheading_font);
    if (m_display_title_label && subheading_font) m_display_title_label->Render(renderer, subheading_font);
    if (m_advanced_title_label && subheading_font) m_advanced_title_label->Render(renderer, subheading_font);
    
    if (m_save_button && body_font) m_save_button->Render(renderer, body_font);
    if (m_cancel_button && body_font) m_cancel_button->Render(renderer, body_font);
    if (m_apply_button && body_font) m_apply_button->Render(renderer, body_font);
}

void SettingsScreen::OnActivate()
{
    std::cout << "Settings screen activated" << std::endl;
    LoadSettings();
}

void SettingsScreen::OnResize(int new_width, int new_height)
{
    RepositionElements(new_width, new_height);
}

void SettingsScreen::OnNavigatedTo(const NavigationContext& context)
{
    std::cout << "Navigated to Settings screen" << std::endl;
    LoadSettings();
}

void SettingsScreen::OnNavigatedFrom()
{
    std::cout << "Navigated away from Settings screen" << std::endl;
    if (m_settings_changed) {
        // Could prompt to save changes
    }
}

void SettingsScreen::LoadSettings()
{
    // Load settings from configuration
    m_current_settings = m_saved_settings;
}

void SettingsScreen::SaveSettings()
{
    // Save settings to configuration
    m_saved_settings = m_current_settings;
    m_settings_changed = false;
    std::cout << "Settings saved" << std::endl;
}

void SettingsScreen::ApplySettings()
{
    // Apply settings without saving
    std::cout << "Settings applied" << std::endl;
}

void SettingsScreen::ResetSettings()
{
    // Reset to defaults
    m_current_settings = SettingsState{};
    m_settings_changed = true;
    std::cout << "Settings reset to defaults" << std::endl;
}

void SettingsScreen::CheckForChanges()
{
    // Check if settings have changed
}

void SettingsScreen::RepositionElements(int content_width, int content_height)
{
    // Responsive repositioning
}

void SettingsScreen::OnMainnetClicked() { std::cout << "Mainnet selected" << std::endl; }
void SettingsScreen::OnTestnetClicked() { std::cout << "Testnet selected" << std::endl; }
void SettingsScreen::OnRegtestClicked() { std::cout << "Regtest selected" << std::endl; }
void SettingsScreen::OnEncryptWalletClicked() { std::cout << "Encrypt wallet clicked" << std::endl; }
void SettingsScreen::OnChangePassphraseClicked() { std::cout << "Change passphrase clicked" << std::endl; }
void SettingsScreen::OnBackupWalletClicked() { std::cout << "Backup wallet clicked" << std::endl; }
void SettingsScreen::OnVerifyBackupClicked() { std::cout << "Verify backup clicked" << std::endl; }
void SettingsScreen::OnDarkThemeClicked() { std::cout << "Dark theme selected" << std::endl; }
void SettingsScreen::OnLightThemeClicked() { std::cout << "Light theme selected" << std::endl; }
void SettingsScreen::OnLanguageClicked() { std::cout << "Language clicked" << std::endl; }
void SettingsScreen::OnFullscreenClicked() { std::cout << "Fullscreen toggled" << std::endl; }
void SettingsScreen::OnBrowseDatadirClicked() { std::cout << "Browse datadir clicked" << std::endl; }
void SettingsScreen::OnResetSettingsClicked() { ResetSettings(); }

void SettingsScreen::OnSaveClicked()
{
    std::cout << "Save clicked" << std::endl;
    SaveSettings();
    m_gui.SwitchScreen(ScreenType::MAIN);
}

void SettingsScreen::OnCancelClicked()
{
    std::cout << "Cancel clicked" << std::endl;
    LoadSettings(); // Revert changes
    m_gui.SwitchScreen(ScreenType::MAIN);
}

void SettingsScreen::OnApplyClicked()
{
    std::cout << "Apply clicked" << std::endl;
    ApplySettings();
}