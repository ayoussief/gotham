// Copyright (c) 2025 The Gotham Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or https://opensource.org/license/mit/.

#ifndef GOTHAM_SDL2_SCREENS_MAIN_SCREEN_H
#define GOTHAM_SDL2_SCREENS_MAIN_SCREEN_H

#include "../ui/screen.h"
#include "../ui/button.h"
#include "../ui/label.h"
#include "../ui/panel.h"
#include "../ui/ui_factory.h"
#include "../ui/layout_manager.h"
#include "../ui/navigation_manager.h"
#include <memory>
#include <vector>

/**
 * Main dashboard screen for Gotham City - Content Area Only
 * Header, sidebar, and status bar handled by persistent layout
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
    void SetContentAreaBounds(const Rect& bounds) override;
    
    // Navigation lifecycle methods
    void OnNavigatedTo(const NavigationContext& context) override;
    void OnNavigatedFrom() override;

private:
    // UI Systems
    std::unique_ptr<UIFactory> m_ui_factory;
    std::unique_ptr<LayoutManager> m_layout_manager;
    
    // Content area components only
    std::unique_ptr<Panel> m_content_panel;
    
    // Main content elements
    std::unique_ptr<Label> m_welcome_label;
    std::unique_ptr<Label> m_balance_display_label;
    std::unique_ptr<Panel> m_quick_actions_panel;
    std::unique_ptr<Button> m_send_quick_button;
    std::unique_ptr<Button> m_receive_quick_button;
    std::unique_ptr<Button> m_transactions_quick_button;
    
    // Status display panel (content-specific, not global status bar)
    std::unique_ptr<Panel> m_wallet_status_panel;
    std::unique_ptr<Label> m_network_status_label;
    std::unique_ptr<Label> m_sync_status_label;
    std::unique_ptr<Label> m_wallet_status_label;
    
    // Recent activity panel
    std::unique_ptr<Panel> m_recent_activity_panel;
    std::unique_ptr<Label> m_recent_activity_title;
    std::vector<std::unique_ptr<Label>> m_recent_transactions;
    
    // Animation and state
    float m_elapsed_time{0.0f};
    bool m_wallet_connected{false};
    int m_current_block_height{0};
    int m_connection_count{0};
    
    // Content area bounds (set by persistent layout)
    Rect m_content_area_bounds{0, 0, 800, 600};
    
    // Content area methods
    void CreateContentElements();
    void CreateContentPanel();
    void CreateQuickActionsPanel();
    void CreateWalletStatusPanel();
    void CreateRecentActivityPanel();
    void UpdateWalletInfo();
    void UpdateNetworkInfo();
    void SetupButtonCallbacks();
    void RefreshWalletData();
    void RefreshRecentTransactions();
    void RepositionElements(int content_width, int content_height);
    void PositionElementsDirectly();
    void ApplyContentLayout();
    void ApplyQuickActionsLayout(const Rect& panel_bounds);
    void ApplyStatusPanelLayout(const Rect& panel_bounds);
    void ApplyRecentActivityLayout(const Rect& panel_bounds);
    void ApplyResponsiveLayout();
    void ValidateUIElements();
    
    // Quick action callbacks
    void OnSendQuickClicked();
    void OnReceiveQuickClicked();
    void OnTransactionsQuickClicked();
};

#endif // GOTHAM_SDL2_SCREENS_MAIN_SCREEN_H