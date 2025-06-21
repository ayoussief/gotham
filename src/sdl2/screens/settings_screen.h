// Copyright (c) 2025 The Gotham Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or https://opensource.org/license/mit/.

#ifndef GOTHAM_SDL2_SCREENS_SETTINGS_SCREEN_H
#define GOTHAM_SDL2_SCREENS_SETTINGS_SCREEN_H

#include "../ui/screen.h"
#include "../ui/button.h"
#include "../ui/label.h"
#include "../ui/panel.h"
#include <memory>

class SettingsScreen : public Screen
{
public:
    explicit SettingsScreen(GothamCityGUI& gui);
    bool Initialize() override;
    void HandleEvent(const SDL_Event& event) override;
    void Update(float delta_time) override;
    void Render(Renderer& renderer) override;
    void OnActivate() override;
    void OnResize(int new_width, int new_height) override;

private:
    // UI Components
    std::unique_ptr<Panel> m_header_panel;
    std::unique_ptr<Panel> m_network_panel;
    std::unique_ptr<Panel> m_display_panel;
    std::unique_ptr<Panel> m_security_panel;
    std::unique_ptr<Panel> m_about_panel;
    
    // Header
    std::unique_ptr<Label> m_title_label;
    std::unique_ptr<Button> m_back_button;
    
    // Network settings
    std::unique_ptr<Label> m_network_title_label;
    std::unique_ptr<Label> m_network_status_label;
    std::unique_ptr<Label> m_rpc_port_label;
    std::unique_ptr<Label> m_proxy_label;
    
    // Display settings
    std::unique_ptr<Label> m_display_title_label;
    std::unique_ptr<Label> m_theme_label;
    std::unique_ptr<Label> m_language_label;
    std::unique_ptr<Label> m_currency_label;
    
    // Security settings
    std::unique_ptr<Label> m_security_title_label;
    std::unique_ptr<Label> m_encryption_label;
    std::unique_ptr<Label> m_autolock_label;
    
    // About
    std::unique_ptr<Label> m_about_title_label;
    std::unique_ptr<Label> m_version_label;
    std::unique_ptr<Label> m_core_version_label;
    
    float m_elapsed_time{0.0f};
    
    void CreateLayout();
    void CreateHeaderPanel();
    void CreateNetworkPanel();
    void CreateDisplayPanel();
    void CreateSecurityPanel();
    void CreateAboutPanel();
    void SetupButtonCallbacks();
    
    // Button callbacks
    void OnBackClicked();
    
    // Helper methods
    Color GetGothamGoldColor() const;
    Color GetGothamDarkColor() const;
};

#endif // GOTHAM_SDL2_SCREENS_SETTINGS_SCREEN_H