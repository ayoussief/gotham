// Copyright (c) 2025 The Gotham Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or https://opensource.org/license/mit/.

#include "wallet_screen.h"
#include "../gotham_city_gui.h"
#include "../utils/theme.h"
#include "../utils/font_manager.h"
#include "../wallet/wallet_manager.h"
#include <iostream>
#include <iomanip>
#include <sstream>

WalletScreen::WalletScreen(GothamCityGUI& gui) : Screen(gui)
{
}

bool WalletScreen::Initialize()
{
    if (m_initialized) {
        return true;
    }

    CreateLayout();
    SetupButtonCallbacks();

    m_initialized = true;
    std::cout << "Wallet screen initialized" << std::endl;
    return true;
}

void WalletScreen::HandleEvent(const SDL_Event& event)
{
    // Handle keyboard shortcuts
    if (event.type == SDL_KEYDOWN) {
        switch (event.key.keysym.sym) {
            case SDLK_ESCAPE:
                OnBackClicked();
                return;
        }
    }

    // Pass events to UI components
    if (m_back_button) m_back_button->HandleEvent(event);
    if (m_send_button) m_send_button->HandleEvent(event);
    if (m_receive_button) m_receive_button->HandleEvent(event);
    if (m_transactions_button) m_transactions_button->HandleEvent(event);
}

void WalletScreen::Update(float delta_time)
{
    m_elapsed_time += delta_time;
    
    // Update UI components
    if (m_back_button) m_back_button->Update(delta_time);
    if (m_send_button) m_send_button->Update(delta_time);
    if (m_receive_button) m_receive_button->Update(delta_time);
    if (m_transactions_button) m_transactions_button->Update(delta_time);
    
    // Update wallet data periodically
    if (static_cast<int>(m_elapsed_time) % 3 == 0) {
        UpdateWalletData();
    }
}

void WalletScreen::Render(Renderer& renderer)
{
    // Render background
    renderer.Clear(GetGothamDarkColor());
    
    // Render UI panels
    if (m_header_panel) m_header_panel->Render(renderer);
    if (m_balance_panel) m_balance_panel->Render(renderer);
    if (m_actions_panel) m_actions_panel->Render(renderer);
    if (m_transactions_panel) m_transactions_panel->Render(renderer);
    
    // Render components with fonts
    FontManager* font_manager = m_gui.GetFontManager();
    if (font_manager) {
        TTF_Font* title_font = font_manager->GetDefaultFont(24);
        TTF_Font* subtitle_font = font_manager->GetDefaultFont(18);
        TTF_Font* button_font = font_manager->GetDefaultFont(16);
        TTF_Font* body_font = font_manager->GetDefaultFont(14);
        
        // Header
        if (m_title_label && title_font) {
            m_title_label->Render(renderer, title_font);
        }
        if (m_back_button && button_font) {
            m_back_button->Render(renderer, button_font);
        }
        
        // Balance section
        if (m_balance_title_label && subtitle_font) {
            m_balance_title_label->Render(renderer, subtitle_font);
        }
        if (m_confirmed_balance_label && body_font) {
            m_confirmed_balance_label->Render(renderer, body_font);
        }
        if (m_unconfirmed_balance_label && body_font) {
            m_unconfirmed_balance_label->Render(renderer, body_font);
        }
        if (m_total_balance_label && button_font) {
            m_total_balance_label->Render(renderer, button_font);
        }
        
        // Action buttons
        if (m_send_button && button_font) {
            m_send_button->Render(renderer, button_font);
        }
        if (m_receive_button && button_font) {
            m_receive_button->Render(renderer, button_font);
        }
        if (m_transactions_button && button_font) {
            m_transactions_button->Render(renderer, button_font);
        }
        
        // Transactions
        if (m_transactions_title_label && subtitle_font) {
            m_transactions_title_label->Render(renderer, subtitle_font);
        }
        for (auto& label : m_transaction_labels) {
            if (label && body_font) {
                label->Render(renderer, body_font);
            }
        }
    }
}

void WalletScreen::OnActivate()
{
    std::cout << "Wallet screen activated" << std::endl;
    UpdateWalletData();
}

void WalletScreen::OnResize(int new_width, int new_height)
{
    std::cout << "Wallet screen resizing to: " << new_width << "x" << new_height << std::endl;
    CreateLayout();
    SetupButtonCallbacks();
}

void WalletScreen::CreateLayout()
{
    CreateHeaderPanel();
    CreateBalancePanel();
    CreateActionsPanel();
    CreateTransactionsPanel();
}

void WalletScreen::CreateHeaderPanel()
{
    int width = m_gui.GetRenderer()->GetWidth();
    
    m_header_panel = std::make_unique<Panel>(Rect(0, 0, width, 80));
    m_header_panel->SetBackgroundColor(GetGothamDarkColor());
    
    m_title_label = std::make_unique<Label>("💰 Wallet", Point(50, 30));
    m_title_label->SetColor(GetGothamGoldColor());
    
    m_back_button = std::make_unique<Button>("← Back", Rect(width - 120, 20, 100, 40));
    m_back_button->SetColors(Color(60, 60, 70, 255), Color(80, 80, 90, 255), Color(40, 40, 50, 255));
    m_back_button->SetTextColor(Color(255, 255, 255, 255));
}

void WalletScreen::CreateBalancePanel()
{
    int width = m_gui.GetRenderer()->GetWidth();
    
    m_balance_panel = std::make_unique<Panel>(Rect(20, 100, width - 40, 150));
    m_balance_panel->SetBackgroundColor(Color(25, 25, 35, 255));
    m_balance_panel->SetBorderColor(GetGothamGoldColor());
    
    m_balance_title_label = std::make_unique<Label>("Balance", Point(40, 120));
    m_balance_title_label->SetColor(Color(255, 255, 255, 255));
    
    m_confirmed_balance_label = std::make_unique<Label>("Confirmed: 0.00000000 BTC", Point(60, 150));
    m_confirmed_balance_label->SetColor(Color(46, 160, 67, 255));
    
    m_unconfirmed_balance_label = std::make_unique<Label>("Unconfirmed: 0.00000000 BTC", Point(60, 175));
    m_unconfirmed_balance_label->SetColor(Color(255, 193, 7, 255));
    
    m_total_balance_label = std::make_unique<Label>("Total: 0.00000000 BTC", Point(60, 210));
    m_total_balance_label->SetColor(GetGothamGoldColor());
}

void WalletScreen::CreateActionsPanel()
{
    int width = m_gui.GetRenderer()->GetWidth();
    int button_width = 150;
    int button_spacing = 20;
    int start_x = (width - (3 * button_width + 2 * button_spacing)) / 2;
    
    m_actions_panel = std::make_unique<Panel>(Rect(20, 270, width - 40, 80));
    m_actions_panel->SetBackgroundColor(Color(20, 20, 30, 255));
    
    m_send_button = std::make_unique<Button>("Send", Rect(start_x, 290, button_width, 40));
    m_send_button->SetColors(Color(220, 53, 69, 255), Color(240, 73, 89, 255), Color(200, 33, 49, 255));
    m_send_button->SetTextColor(Color(255, 255, 255, 255));
    
    m_receive_button = std::make_unique<Button>("Receive", Rect(start_x + button_width + button_spacing, 290, button_width, 40));
    m_receive_button->SetColors(Color(46, 160, 67, 255), Color(66, 180, 87, 255), Color(26, 140, 47, 255));
    m_receive_button->SetTextColor(Color(255, 255, 255, 255));
    
    m_transactions_button = std::make_unique<Button>("Transactions", Rect(start_x + 2 * (button_width + button_spacing), 290, button_width, 40));
    m_transactions_button->SetColors(Color(0, 123, 255, 255), Color(20, 143, 255, 255), Color(0, 103, 235, 255));
    m_transactions_button->SetTextColor(Color(255, 255, 255, 255));
}

void WalletScreen::CreateTransactionsPanel()
{
    int width = m_gui.GetRenderer()->GetWidth();
    int height = m_gui.GetRenderer()->GetHeight();
    
    m_transactions_panel = std::make_unique<Panel>(Rect(20, 370, width - 40, height - 390));
    m_transactions_panel->SetBackgroundColor(Color(20, 20, 30, 255));
    m_transactions_panel->SetBorderColor(Color(60, 60, 70, 255));
    
    m_transactions_title_label = std::make_unique<Label>("Recent Transactions", Point(40, 390));
    m_transactions_title_label->SetColor(Color(255, 255, 255, 255));
}

void WalletScreen::UpdateWalletData()
{
    auto* wallet_manager = m_gui.GetWalletManager();
    if (!wallet_manager || !wallet_manager->IsWalletLoaded()) {
        return;
    }
    
    auto balance = wallet_manager->GetBalance();
    auto transactions = wallet_manager->GetTransactions(5);
    
    // Update balance labels
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(8);
    
    oss.str("");
    oss << "Confirmed: " << balance.confirmed << " BTC";
    if (m_confirmed_balance_label) {
        m_confirmed_balance_label->SetText(oss.str());
    }
    
    oss.str("");
    oss << "Unconfirmed: " << balance.unconfirmed << " BTC";
    if (m_unconfirmed_balance_label) {
        m_unconfirmed_balance_label->SetText(oss.str());
    }
    
    oss.str("");
    oss << "Total: " << balance.total << " BTC";
    if (m_total_balance_label) {
        m_total_balance_label->SetText(oss.str());
    }
    
    // Update transaction list
    m_transaction_labels.clear();
    int y_pos = 420;
    
    if (transactions.empty()) {
        auto no_tx_label = std::make_unique<Label>("No transactions found", Point(60, y_pos));
        no_tx_label->SetColor(Color(150, 150, 150, 255));
        m_transaction_labels.push_back(std::move(no_tx_label));
    } else {
        for (const auto& tx : transactions) {
            Color amount_color = tx.amount > 0 ? Color(46, 160, 67, 255) : Color(220, 53, 69, 255);
            
            oss.str("");
            oss << (tx.amount > 0 ? "+" : "") << std::fixed << std::setprecision(8) << tx.amount << " BTC (" << tx.confirmations << " conf)";
            
            auto tx_label = std::make_unique<Label>(oss.str(), Point(60, y_pos));
            tx_label->SetColor(amount_color);
            m_transaction_labels.push_back(std::move(tx_label));
            
            y_pos += 25;
            
            std::string details = tx.address.substr(0, 20) + "... | " + tx.label;
            auto detail_label = std::make_unique<Label>(details, Point(80, y_pos));
            detail_label->SetColor(Color(150, 150, 150, 255));
            m_transaction_labels.push_back(std::move(detail_label));
            
            y_pos += 30;
        }
    }
}

void WalletScreen::SetupButtonCallbacks()
{
    if (m_back_button) {
        m_back_button->SetOnClick([this]() { OnBackClicked(); });
    }
    if (m_send_button) {
        m_send_button->SetOnClick([this]() { OnSendClicked(); });
    }
    if (m_receive_button) {
        m_receive_button->SetOnClick([this]() { OnReceiveClicked(); });
    }
    if (m_transactions_button) {
        m_transactions_button->SetOnClick([this]() { OnTransactionsClicked(); });
    }
}

void WalletScreen::OnBackClicked()
{
    m_gui.SwitchScreen(ScreenType::MAIN);
}

void WalletScreen::OnSendClicked()
{
    m_gui.SwitchScreen(ScreenType::SEND);
}

void WalletScreen::OnReceiveClicked()
{
    m_gui.SwitchScreen(ScreenType::RECEIVE);
}

void WalletScreen::OnTransactionsClicked()
{
    m_gui.SwitchScreen(ScreenType::TRANSACTIONS);
}

Color WalletScreen::GetGothamGoldColor() const
{
    return Color(255, 215, 0, 255);
}

Color WalletScreen::GetGothamDarkColor() const
{
    return Color(15, 15, 20, 255);
}