// Copyright (c) 2025 The Gotham Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or https://opensource.org/license/mit/.

#ifndef GOTHAM_SDL2_SCREENS_MAIN_SCREEN_H
#define GOTHAM_SDL2_SCREENS_MAIN_SCREEN_H

#include "../ui/screen.h"
#include "../ui/button.h"
#include "../ui/label.h"
#include "../ui/panel.h"
#include <memory>
#include <vector>

/**
 * Main dashboard screen for Gotham City
 */
class MainScreen : public Screen
{
public:
    explicit MainScreen(GothamCityGUI& gui);
    ~MainScreen() override = default;

    bool Initialize() override;
    void HandleEvent(const SDL_Event& event) override;
    void Update(float delta_time) override;
    void Render(Renderer& renderer) override;
    void OnActivate() override;
    void OnResize(int new_width, int new_height) override;

private:
    // UI Components
    std::unique_ptr<Panel> m_header_panel;
    std::unique_ptr<Panel> m_sidebar_panel;
    std::unique_ptr<Panel> m_main_panel;
    std::unique_ptr<Panel> m_status_panel;
    
    // Header components
    std::unique_ptr<Label> m_title_label;
    std::unique_ptr<Label> m_balance_label;
    std::unique_ptr<Button> m_settings_button;
    
    // Sidebar navigation
    std::unique_ptr<Button> m_wallet_button;
    std::unique_ptr<Button> m_send_button;
    std::unique_ptr<Button> m_receive_button;
    std::unique_ptr<Button> m_transactions_button;
    std::unique_ptr<Button> m_console_button;
    
    // Main content
    std::unique_ptr<Label> m_welcome_label;
    std::unique_ptr<Label> m_network_status_label;
    std::unique_ptr<Label> m_sync_status_label;
    
    // Status bar
    std::unique_ptr<Label> m_connection_label;
    std::unique_ptr<Label> m_block_height_label;
    
    // Animation and state
    float m_elapsed_time{0.0f};
    bool m_wallet_connected{false};
    int m_current_block_height{0};
    int m_connection_count{0};
    
    void CreateLayout();
    void CreateHeaderPanel();
    void CreateSidebarPanel();
    void CreateMainPanel();
    void CreateStatusPanel();
    void UpdateWalletInfo();
    void UpdateNetworkInfo();
    void RenderBackground(Renderer& renderer);
    void SetupButtonCallbacks();
    
    // Navigation callbacks
    void OnWalletClicked();
    void OnSendClicked();
    void OnReceiveClicked();
    void OnTransactionsClicked();
    void OnConsoleClicked();
    void OnSettingsClicked();
};

#endif // GOTHAM_SDL2_SCREENS_MAIN_SCREEN_H