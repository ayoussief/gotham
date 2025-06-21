// Copyright (c) 2025 The Gotham Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or https://opensource.org/license/mit/.

#ifndef GOTHAM_SDL2_SCREENS_SETTINGS_SCREEN_H
#define GOTHAM_SDL2_SCREENS_SETTINGS_SCREEN_H

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

/**
 * Application settings screen - Content Area Only
 * Configuration options for the application
 */
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
    
    // Navigation lifecycle methods
    void OnNavigatedTo(const NavigationContext& context) override;
    void OnNavigatedFrom() override;

private:
    // UI Systems
    std::unique_ptr<UIFactory> m_ui_factory;
    std::unique_ptr<LayoutManager> m_layout_manager;
    
    // Content area components only
    std::unique_ptr<Panel> m_content_panel;
    
    // Network settings panel
    std::unique_ptr<Panel> m_network_panel;
    std::unique_ptr<Label> m_network_title_label;
    std::unique_ptr<Button> m_mainnet_button;
    std::unique_ptr<Button> m_testnet_button;
    std::unique_ptr<Button> m_regtest_button;
    std::unique_ptr<Label> m_proxy_label;
    std::unique_ptr<TextInput> m_proxy_input;
    
    // Security settings panel
    std::unique_ptr<Panel> m_security_panel;
    std::unique_ptr<Label> m_security_title_label;
    std::unique_ptr<Button> m_encrypt_wallet_button;
    std::unique_ptr<Button> m_change_passphrase_button;
    std::unique_ptr<Button> m_backup_wallet_button;
    std::unique_ptr<Button> m_verify_backup_button;
    
    // Display settings panel
    std::unique_ptr<Panel> m_display_panel;
    std::unique_ptr<Label> m_display_title_label;
    std::unique_ptr<Button> m_dark_theme_button;
    std::unique_ptr<Button> m_light_theme_button;
    std::unique_ptr<Label> m_language_label;
    std::unique_ptr<Button> m_language_button;
    std::unique_ptr<Button> m_fullscreen_button;
    
    // Advanced settings panel
    std::unique_ptr<Panel> m_advanced_panel;
    std::unique_ptr<Label> m_advanced_title_label;
    std::unique_ptr<Label> m_datadir_label;
    std::unique_ptr<TextInput> m_datadir_input;
    std::unique_ptr<Button> m_browse_datadir_button;
    std::unique_ptr<Button> m_reset_settings_button;
    
    // Action buttons
    std::unique_ptr<Button> m_save_button;
    std::unique_ptr<Button> m_cancel_button;
    std::unique_ptr<Button> m_apply_button;
    
    // Settings state
    struct SettingsState {
        std::string network{"mainnet"};
        std::string proxy;
        std::string theme{"dark"};
        std::string language{"en"};
        std::string datadir;
        bool fullscreen{false};
        bool wallet_encrypted{false};
    } m_current_settings, m_saved_settings;
    
    float m_elapsed_time{0.0f};
    bool m_settings_changed{false};
    
    // Content area methods
    void CreateContentPanel();
    void CreateNetworkPanel();
    void CreateSecurityPanel();
    void CreateDisplayPanel();
    void CreateAdvancedPanel();
    void SetupButtonCallbacks();
    void LoadSettings();
    void SaveSettings();
    void ApplySettings();
    void ResetSettings();
    void CheckForChanges();
    void RepositionElements(int content_width, int content_height);
    
    // Action callbacks
    void OnMainnetClicked();
    void OnTestnetClicked();
    void OnRegtestClicked();
    void OnEncryptWalletClicked();
    void OnChangePassphraseClicked();
    void OnBackupWalletClicked();
    void OnVerifyBackupClicked();
    void OnDarkThemeClicked();
    void OnLightThemeClicked();
    void OnLanguageClicked();
    void OnFullscreenClicked();
    void OnBrowseDatadirClicked();
    void OnResetSettingsClicked();
    void OnSaveClicked();
    void OnCancelClicked();
    void OnApplyClicked();
};

#endif // GOTHAM_SDL2_SCREENS_SETTINGS_SCREEN_H