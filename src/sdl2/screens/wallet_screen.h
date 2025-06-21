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
#include <memory>

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

private:
    // UI Systems
    std::unique_ptr<UIFactory> m_ui_factory;
    std::unique_ptr<LayoutManager> m_layout_manager;
    
    // UI Components
    std::unique_ptr<Panel> m_header_panel;
    std::unique_ptr<Panel> m_balance_panel;
    std::unique_ptr<Panel> m_actions_panel;
    std::unique_ptr<Panel> m_transactions_panel;
    
    // Header
    std::unique_ptr<Label> m_title_label;
    std::unique_ptr<Button> m_back_button;
    
    // Balance section
    std::unique_ptr<Label> m_balance_title_label;
    std::unique_ptr<Label> m_confirmed_balance_label;
    std::unique_ptr<Label> m_unconfirmed_balance_label;
    std::unique_ptr<Label> m_total_balance_label;
    
    // Action buttons
    std::unique_ptr<Button> m_send_button;
    std::unique_ptr<Button> m_receive_button;
    std::unique_ptr<Button> m_transactions_button;
    
    // Transaction list
    std::unique_ptr<Label> m_transactions_title_label;
    std::vector<std::unique_ptr<Label>> m_transaction_labels;
    
    // Layout items for responsive design
    std::vector<LayoutItem> m_balance_items;
    std::vector<LayoutItem> m_action_items;
    
    float m_elapsed_time{0.0f};
    
    void CreateLayout();
    void CreateHeaderPanel();
    void CreateBalancePanel();
    void CreateActionsPanel();
    void CreateTransactionsPanel();
    void UpdateWalletData();
    void SetupButtonCallbacks();
    
    // Button callbacks
    void OnBackClicked();
    void OnSendClicked();
    void OnReceiveClicked();
    void OnTransactionsClicked();
};

#endif // GOTHAM_SDL2_SCREENS_WALLET_SCREEN_H