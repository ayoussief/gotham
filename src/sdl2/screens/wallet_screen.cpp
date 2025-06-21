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
    // Initialize UI systems
    if (auto* theme_manager = m_gui.GetThemeManager()) {
        m_ui_factory = std::make_unique<UIFactory>(*theme_manager);
    }
    m_layout_manager = std::make_unique<LayoutManager>();
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
    // Render background with consistent theme
    renderer.Clear(UIStyleGuide::Colors::BACKGROUND_DARK);
    
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
    if (!m_ui_factory || !m_layout_manager) return;
    
    int width = m_gui.GetRenderer()->GetWidth();
    int height = m_gui.GetRenderer()->GetHeight();
    
    // Create header panel with consistent styling
    Rect header_bounds = Rect(0, 0, width, UIStyleGuide::Dimensions::HEADER_HEIGHT);
    m_header_panel = m_ui_factory->CreatePanel(header_bounds, PanelStyle::HEADER);
    
    // Create header components with factory
    m_title_label = m_ui_factory->CreateLabel("💰 Wallet", Point(0, 0), LabelStyle::TITLE);
    m_back_button = m_ui_factory->CreateButton("← Back", Rect(0, 0, 100, 40), ButtonStyle::SECONDARY);
    
    // Setup layout for header components
    std::vector<LayoutItem> header_items(2);
    
    // Title item
    header_items[0].constraints.preferred_width = 200;
    header_items[0].margin = Margin(0, UIStyleGuide::Spacing::LG);
    header_items[0].on_bounds_changed = [this](const Rect& bounds) {
        if (m_title_label) {
            m_title_label->SetPosition(Point(bounds.x, bounds.y + bounds.h / 2 - 12));
        }
    };
    
    // Back button item
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

void WalletScreen::CreateBalancePanel()
{
    if (!m_ui_factory || !m_layout_manager) return;
    
    int width = m_gui.GetRenderer()->GetWidth();
    
    // Create balance panel with consistent styling
    Rect balance_bounds = Rect(UIStyleGuide::Spacing::LG, 
                              UIStyleGuide::Dimensions::HEADER_HEIGHT + UIStyleGuide::Spacing::LG,
                              width - 2 * UIStyleGuide::Spacing::LG, 
                              150);
    m_balance_panel = m_ui_factory->CreatePanel(balance_bounds, PanelStyle::CARD);
    
    // Create balance labels with factory
    m_balance_title_label = m_ui_factory->CreateLabel("Balance", Point(0, 0), LabelStyle::HEADING);
    m_confirmed_balance_label = m_ui_factory->CreateLabel("Confirmed: 0.00000000 GTC", Point(0, 0), LabelStyle::BODY);
    m_unconfirmed_balance_label = m_ui_factory->CreateLabel("Unconfirmed: 0.00000000 GTC", Point(0, 0), LabelStyle::BODY);
    m_total_balance_label = m_ui_factory->CreateLabel("Total: 0.00000000 GTC", Point(0, 0), LabelStyle::TITLE);
    
    // Apply theme colors
    if (m_confirmed_balance_label) m_confirmed_balance_label->SetColor(UIStyleGuide::Colors::SUCCESS);
    if (m_unconfirmed_balance_label) m_unconfirmed_balance_label->SetColor(UIStyleGuide::Colors::WARNING);
    if (m_total_balance_label) m_total_balance_label->SetColor(UIStyleGuide::Colors::GOTHAM_GOLD);
    
    // Setup layout for balance items
    m_balance_items.clear();
    m_balance_items.resize(4);
    
    // Balance title
    m_balance_items[0].constraints.preferred_height = 30;
    m_balance_items[0].margin = Margin(UIStyleGuide::Spacing::MD);
    m_balance_items[0].on_bounds_changed = [this](const Rect& bounds) {
        if (m_balance_title_label) {
            m_balance_title_label->SetPosition(Point(bounds.x, bounds.y));
        }
    };
    
    // Confirmed balance
    m_balance_items[1].constraints.preferred_height = 25;
    m_balance_items[1].margin = Margin(UIStyleGuide::Spacing::SM, UIStyleGuide::Spacing::LG);
    m_balance_items[1].on_bounds_changed = [this](const Rect& bounds) {
        if (m_confirmed_balance_label) {
            m_confirmed_balance_label->SetPosition(Point(bounds.x, bounds.y));
        }
    };
    
    // Unconfirmed balance
    m_balance_items[2].constraints.preferred_height = 25;
    m_balance_items[2].margin = Margin(UIStyleGuide::Spacing::SM, UIStyleGuide::Spacing::LG);
    m_balance_items[2].on_bounds_changed = [this](const Rect& bounds) {
        if (m_unconfirmed_balance_label) {
            m_unconfirmed_balance_label->SetPosition(Point(bounds.x, bounds.y));
        }
    };
    
    // Total balance
    m_balance_items[3].constraints.preferred_height = 30;
    m_balance_items[3].margin = Margin(UIStyleGuide::Spacing::MD, UIStyleGuide::Spacing::LG);
    m_balance_items[3].on_bounds_changed = [this](const Rect& bounds) {
        if (m_total_balance_label) {
            m_total_balance_label->SetPosition(Point(bounds.x, bounds.y));
        }
    };
    
    // Apply vertical layout for balance panel
    m_layout_manager->CreateVerticalLayout(balance_bounds, m_balance_items, 
                                         UIStyleGuide::Spacing::SM, Alignment::START);
}

void WalletScreen::CreateActionsPanel()
{
    if (!m_ui_factory || !m_layout_manager) return;
    
    int width = m_gui.GetRenderer()->GetWidth();
    
    // Create actions panel with consistent styling
    Rect actions_bounds = Rect(UIStyleGuide::Spacing::LG, 
                              UIStyleGuide::Dimensions::HEADER_HEIGHT + 150 + 2 * UIStyleGuide::Spacing::LG,
                              width - 2 * UIStyleGuide::Spacing::LG, 
                              80);
    m_actions_panel = m_ui_factory->CreatePanel(actions_bounds, PanelStyle::MAIN);
    
    // Create action buttons with factory and consistent styling
    m_send_button = m_ui_factory->CreateButton("💸 Send", Rect(0, 0, 150, UIStyleGuide::Dimensions::BUTTON_HEIGHT), ButtonStyle::WARNING);
    m_receive_button = m_ui_factory->CreateButton("📥 Receive", Rect(0, 0, 150, UIStyleGuide::Dimensions::BUTTON_HEIGHT), ButtonStyle::SUCCESS);
    m_transactions_button = m_ui_factory->CreateButton("📋 History", Rect(0, 0, 150, UIStyleGuide::Dimensions::BUTTON_HEIGHT), ButtonStyle::SECONDARY);
    
    // Setup layout for action buttons
    m_action_items.clear();
    m_action_items.resize(3);
    
    // Send button
    m_action_items[0].constraints.preferred_width = 150;
    m_action_items[0].constraints.weight = 1.0f;
    m_action_items[0].margin = Margin(UIStyleGuide::Spacing::SM);
    m_action_items[0].on_bounds_changed = [this](const Rect& bounds) {
        if (m_send_button) {
            m_send_button->SetBounds(bounds);
        }
    };
    
    // Receive button
    m_action_items[1].constraints.preferred_width = 150;
    m_action_items[1].constraints.weight = 1.0f;
    m_action_items[1].margin = Margin(UIStyleGuide::Spacing::SM);
    m_action_items[1].on_bounds_changed = [this](const Rect& bounds) {
        if (m_receive_button) {
            m_receive_button->SetBounds(bounds);
        }
    };
    
    // Transactions button
    m_action_items[2].constraints.preferred_width = 150;
    m_action_items[2].constraints.weight = 1.0f;
    m_action_items[2].margin = Margin(UIStyleGuide::Spacing::SM);
    m_action_items[2].on_bounds_changed = [this](const Rect& bounds) {
        if (m_transactions_button) {
            m_transactions_button->SetBounds(bounds);
        }
    };
    
    // Apply horizontal layout for action buttons
    m_layout_manager->CreateHorizontalLayout(actions_bounds, m_action_items, 
                                           UIStyleGuide::Spacing::MD, Alignment::CENTER);
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

