// Copyright (c) 2025 The Gotham Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or https://opensource.org/license/mit/.

#ifndef GOTHAM_SDL2_SCREENS_WALLET_SCREEN_H
#define GOTHAM_SDL2_SCREENS_WALLET_SCREEN_H

#include "../ui/screen.h"
#include "../ui/button.h"
#include "../ui/label.h"
#include "../ui/panel.h"
#include "../ui/ui_factory.h"
#include "../ui/layout_manager.h"
#include "../ui/ui_style_guide.h"
#include "../ui/navigation_manager.h"
#include <memory>

/**
 * Wallet management screen - Content Area Only
 * Shows wallet balance, actions, and recent transactions
 */
class WalletScreen : public Screen
{
public:
    explicit WalletScreen(GothamCityGUI& gui);
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
    
    // Balance section
    std::unique_ptr<Panel> m_balance_panel;
    std::unique_ptr<Label> m_balance_title_label;
    std::unique_ptr<Label> m_confirmed_balance_label;
    std::unique_ptr<Label> m_unconfirmed_balance_label;
    std::unique_ptr<Label> m_total_balance_label;
    
    // Action buttons panel
    std::unique_ptr<Panel> m_actions_panel;
    std::unique_ptr<Button> m_send_button;
    std::unique_ptr<Button> m_receive_button;
    std::unique_ptr<Button> m_backup_button;
    std::unique_ptr<Button> m_restore_button;
    
    // Recent transactions panel
    std::unique_ptr<Panel> m_transactions_panel;
    std::unique_ptr<Label> m_transactions_title_label;
    std::vector<std::unique_ptr<Label>> m_transaction_labels;
    std::unique_ptr<Button> m_view_all_transactions_button;
    
    // Wallet info panel
    std::unique_ptr<Panel> m_wallet_info_panel;
    std::unique_ptr<Label> m_wallet_status_label;
    std::unique_ptr<Label> m_encryption_status_label;
    std::unique_ptr<Label> m_backup_status_label;
    
    float m_elapsed_time{0.0f};
    
    // Content area bounds (set by persistent layout)
    Rect m_content_area_bounds{0, 0, 800, 600};
    
    // Content area methods
    void CreateContentPanel();
    void CreateBalancePanel();
    void CreateActionsPanel();
    void CreateTransactionsPanel();
    void CreateWalletInfoPanel();
    void UpdateWalletData();
    void SetupButtonCallbacks();
    void RefreshWalletData();
    void RefreshTransactionHistory();
    void RepositionElements(int content_width, int content_height);
    
    // Action callbacks
    void OnSendClicked();
    void OnReceiveClicked();
    void OnBackupClicked();
    void OnRestoreClicked();
    void OnViewAllTransactionsClicked();
};

#endif // GOTHAM_SDL2_SCREENS_WALLET_SCREEN_H