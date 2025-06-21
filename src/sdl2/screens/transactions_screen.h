// Copyright (c) 2025 The Gotham Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or https://opensource.org/license/mit/.

#ifndef GOTHAM_SDL2_SCREENS_TRANSACTIONS_SCREEN_H
#define GOTHAM_SDL2_SCREENS_TRANSACTIONS_SCREEN_H

#include "../ui/screen.h"
#include "../ui/button.h"
#include "../ui/label.h"
#include "../ui/panel.h"
#include <memory>
#include <vector>

struct TransactionDisplay {
    std::string txid;
    double amount;
    int confirmations;
    std::string address;
    std::string label;
    std::string date;
    bool is_incoming;
};

class TransactionsScreen : public Screen
{
public:
    explicit TransactionsScreen(GothamCityGUI& gui);
    bool Initialize() override;
    void HandleEvent(const SDL_Event& event) override;
    void Update(float delta_time) override;
    void Render(Renderer& renderer) override;
    void OnActivate() override;
    void OnResize(int new_width, int new_height) override;

private:
    // UI Components
    std::unique_ptr<Panel> m_header_panel;
    std::unique_ptr<Panel> m_filter_panel;
    std::unique_ptr<Panel> m_transactions_panel;
    std::unique_ptr<Panel> m_pagination_panel;
    
    std::unique_ptr<Label> m_title_label;
    std::unique_ptr<Button> m_back_button;
    
    std::unique_ptr<Button> m_all_filter_button;
    std::unique_ptr<Button> m_sent_filter_button;
    std::unique_ptr<Button> m_received_filter_button;
    std::unique_ptr<Label> m_filter_status_label;
    
    std::unique_ptr<Label> m_transactions_header_label;
    std::vector<std::unique_ptr<Label>> m_transaction_labels;
    
    std::unique_ptr<Button> m_prev_button;
    std::unique_ptr<Button> m_next_button;
    std::unique_ptr<Label> m_page_label;
    std::unique_ptr<Label> m_instructions_label;
    
    // Transaction data
    std::vector<TransactionDisplay> m_transactions;
    
    int m_current_page{0};
    int m_transactions_per_page{8};
    std::string m_current_filter{"all"};
    float m_elapsed_time{0.0f};
    
    void CreateLayout();
    void CreateHeaderPanel();
    void CreateFilterPanel();
    void CreateTransactionsPanel();
    void LoadTransactions();
    void FilterTransactions();
    void UpdateTransactionDisplay();
    void UpdateTransactionLabels();
    void SetupButtonCallbacks();
    
    // Button callbacks
    void OnBackClicked();
    void OnAllFilterClicked();
    void OnSentFilterClicked();
    void OnReceivedFilterClicked();
    void OnPrevPageClicked();
    void OnNextPageClicked();
    
    // Helper methods
    Color GetGothamGoldColor() const;
    Color GetGothamDarkColor() const;
    std::vector<TransactionDisplay> GetFilteredTransactions() const;
};

#endif // GOTHAM_SDL2_SCREENS_TRANSACTIONS_SCREEN_H