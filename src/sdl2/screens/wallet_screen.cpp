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

    // Only create content area elements
    CreateContentPanel();
    SetupButtonCallbacks();

    m_initialized = true;
    std::cout << "Wallet screen initialized (content area only)" << std::endl;
    return true;
}

void WalletScreen::CreateContentPanel()
{
    if (!m_ui_factory) return;
    
    // Create main content panel that fills the content area exactly
    // Use relative coordinates since viewport handles translation
    Rect relative_content_bounds(0, 0, m_content_area_bounds.w, m_content_area_bounds.h);
    m_content_panel = m_ui_factory->CreatePanel(
        relative_content_bounds,
        PanelStyle::CONTENT
    );
    
    CreateBalancePanel();
    CreateActionsPanel();
    CreateTransactionsPanel();
    CreateWalletInfoPanel();
}

void WalletScreen::CreateBalancePanel()
{
    // Create balance display panel - positions will be set by layout
    m_balance_panel = m_ui_factory->CreatePanel(
        Rect(0, 0, 100, 100), // Placeholder bounds
        PanelStyle::CARD
    );
    
    m_balance_title_label = m_ui_factory->CreateLabel(
        "Wallet Balance", 
        Point(0, 0), // Placeholder position
        LabelStyle::HEADING
    );
    
    m_confirmed_balance_label = m_ui_factory->CreateLabel(
        "Confirmed: 0.00000000 BTC", 
        Point(0, 0), // Placeholder position
        LabelStyle::BODY
    );
    
    m_unconfirmed_balance_label = m_ui_factory->CreateLabel(
        "Unconfirmed: 0.00000000 BTC", 
        Point(0, 0), // Placeholder position
        LabelStyle::CAPTION
    );
    
    m_total_balance_label = m_ui_factory->CreateLabel(
        "Total: 0.00000000 BTC", 
        Point(0, 0), // Placeholder position
        LabelStyle::SUBHEADING
    );
}

void WalletScreen::CreateActionsPanel()
{
    // Create actions panel exactly like main screen quick actions panel
    m_actions_panel = m_ui_factory->CreatePanel(
        Rect(0, 0, 100, 84), // Minimal placeholder size (like main screen)
        PanelStyle::CARD
    );
    
    // Create action buttons exactly like main screen - positions will be set by layout
    m_send_button = m_ui_factory->CreateButton(
        "📤 Send Bitcoin", 
        Rect(0, 0, 140, 44), // Same size as main screen buttons
        ButtonStyle::PRIMARY
    );
    
    m_receive_button = m_ui_factory->CreateButton(
        "📥 Receive Bitcoin", 
        Rect(0, 0, 140, 44), 
        ButtonStyle::SECONDARY
    );
    
    m_backup_button = m_ui_factory->CreateButton(
        "💾 Backup Wallet", 
        Rect(0, 0, 140, 44), 
        ButtonStyle::GHOST
    );
    
    m_restore_button = m_ui_factory->CreateButton(
        "📁 Restore Wallet", 
        Rect(0, 0, 140, 44), 
        ButtonStyle::GHOST
    );
}

void WalletScreen::CreateTransactionsPanel()
{
    // Create recent transactions panel - positions will be set by layout
    m_transactions_panel = m_ui_factory->CreatePanel(
        Rect(0, 0, 100, 100), // Placeholder bounds
        PanelStyle::CARD
    );
    
    m_transactions_title_label = m_ui_factory->CreateLabel(
        "Recent Transactions", 
        Point(0, 0), // Placeholder position
        LabelStyle::SUBHEADING
    );
    
    // Create transaction labels - positions will be set by layout
    for (int i = 0; i < 6; ++i) {
        auto tx_label = m_ui_factory->CreateLabel(
            "No transactions yet", 
            Point(0, 0), // Placeholder position
            LabelStyle::CAPTION
        );
        m_transaction_labels.push_back(std::move(tx_label));
    }
    
    m_view_all_transactions_button = m_ui_factory->CreateButton(
        "View All Transactions", 
        Rect(0, 0, 150, 25), // Placeholder bounds
        ButtonStyle::GHOST
    );
}

void WalletScreen::CreateWalletInfoPanel()
{
    // Create wallet info panel - positions will be set by layout
    m_wallet_info_panel = m_ui_factory->CreatePanel(
        Rect(0, 0, 100, 100), // Placeholder bounds
        PanelStyle::INFO
    );
    
    m_wallet_status_label = m_ui_factory->CreateLabel(
        "Status: Loading...", 
        Point(0, 0), // Placeholder position
        LabelStyle::CAPTION
    );
    
    m_encryption_status_label = m_ui_factory->CreateLabel(
        "Encryption: Unknown", 
        Point(0, 0), // Placeholder position
        LabelStyle::CAPTION
    );
    
    m_backup_status_label = m_ui_factory->CreateLabel(
        "Last Backup: Never", 
        Point(0, 0), // Placeholder position
        LabelStyle::CAPTION
    );
}

void WalletScreen::SetupButtonCallbacks()
{
    if (m_send_button) {
        m_send_button->SetOnClick([this]() {
            OnSendClicked();
        });
    }
    
    if (m_receive_button) {
        m_receive_button->SetOnClick([this]() {
            OnReceiveClicked();
        });
    }
    
    if (m_backup_button) {
        m_backup_button->SetOnClick([this]() {
            OnBackupClicked();
        });
    }
    
    if (m_restore_button) {
        m_restore_button->SetOnClick([this]() {
            OnRestoreClicked();
        });
    }
    
    if (m_view_all_transactions_button) {
        m_view_all_transactions_button->SetOnClick([this]() {
            OnViewAllTransactionsClicked();
        });
    }
}

void WalletScreen::HandleEvent(const SDL_Event& event)
{
    // Handle keyboard shortcuts
    if (event.type == SDL_KEYDOWN) {
        switch (event.key.keysym.sym) {
            case SDLK_s:
                if (event.key.keysym.mod & KMOD_CTRL) {
                    OnSendClicked();
                    return;
                }
                break;
            case SDLK_r:
                if (event.key.keysym.mod & KMOD_CTRL) {
                    OnReceiveClicked();
                    return;
                }
                break;
        }
    }

    // Handle content area events only
    if (m_content_panel) {
        m_content_panel->HandleEvent(event);
    }
    
    // Handle action buttons
    if (m_send_button) m_send_button->HandleEvent(event);
    if (m_receive_button) m_receive_button->HandleEvent(event);
    if (m_backup_button) m_backup_button->HandleEvent(event);
    if (m_restore_button) m_restore_button->HandleEvent(event);
    if (m_view_all_transactions_button) m_view_all_transactions_button->HandleEvent(event);
}

void WalletScreen::Update(float delta_time)
{
    m_elapsed_time += delta_time;
    
    // Update action buttons
    if (m_send_button) m_send_button->Update(delta_time);
    if (m_receive_button) m_receive_button->Update(delta_time);
    if (m_backup_button) m_backup_button->Update(delta_time);
    if (m_restore_button) m_restore_button->Update(delta_time);
    if (m_view_all_transactions_button) m_view_all_transactions_button->Update(delta_time);
    
    // Update wallet data periodically
    static float last_update = 0.0f;
    if (m_elapsed_time - last_update > 3.0f) {
        UpdateWalletData();
        last_update = m_elapsed_time;
    }
}

void WalletScreen::Render(Renderer& renderer)
{
    // Only render content area!
    if (m_content_panel) {
        m_content_panel->Render(renderer);
    }
    
    // Render panels
    if (m_balance_panel) {
        m_balance_panel->Render(renderer);
    }
    
    if (m_actions_panel) {
        m_actions_panel->Render(renderer);
    }
    
    if (m_transactions_panel) {
        m_transactions_panel->Render(renderer);
    }
    
    if (m_wallet_info_panel) {
        m_wallet_info_panel->Render(renderer);
    }
    
    // Get fonts
    TTF_Font* heading_font = m_gui.GetFontManager()->GetDefaultFont(UIStyleGuide::FontSize::HEADING);
    TTF_Font* subheading_font = m_gui.GetFontManager()->GetDefaultFont(UIStyleGuide::FontSize::SUBHEADING);
    TTF_Font* body_font = m_gui.GetFontManager()->GetDefaultFont(UIStyleGuide::FontSize::BODY);
    TTF_Font* caption_font = m_gui.GetFontManager()->GetDefaultFont(UIStyleGuide::FontSize::CAPTION);
    
    // Render balance section
    if (m_balance_title_label && heading_font) {
        m_balance_title_label->Render(renderer, heading_font);
    }
    
    if (m_confirmed_balance_label && body_font) {
        m_confirmed_balance_label->Render(renderer, body_font);
    }
    
    if (m_unconfirmed_balance_label && caption_font) {
        m_unconfirmed_balance_label->Render(renderer, caption_font);
    }
    
    if (m_total_balance_label && subheading_font) {
        m_total_balance_label->Render(renderer, subheading_font);
    }
    
    // Render action buttons
    if (m_send_button && body_font) {
        m_send_button->Render(renderer, body_font);
    }
    
    if (m_receive_button && body_font) {
        m_receive_button->Render(renderer, body_font);
    }
    
    if (m_backup_button && caption_font) {
        m_backup_button->Render(renderer, caption_font);
    }
    
    if (m_restore_button && caption_font) {
        m_restore_button->Render(renderer, caption_font);
    }
    
    // Render transactions section
    if (m_transactions_title_label && subheading_font) {
        m_transactions_title_label->Render(renderer, subheading_font);
    }
    
    for (auto& tx_label : m_transaction_labels) {
        if (tx_label && caption_font) {
            tx_label->Render(renderer, caption_font);
        }
    }
    
    if (m_view_all_transactions_button && caption_font) {
        m_view_all_transactions_button->Render(renderer, caption_font);
    }
    
    // Render wallet info
    if (m_wallet_status_label && caption_font) {
        m_wallet_status_label->Render(renderer, caption_font);
    }
    
    if (m_encryption_status_label && caption_font) {
        m_encryption_status_label->Render(renderer, caption_font);
    }
    
    if (m_backup_status_label && caption_font) {
        m_backup_status_label->Render(renderer, caption_font);
    }
}

void WalletScreen::OnActivate()
{
    std::cout << "Wallet screen activated" << std::endl;
    RefreshWalletData();
}

void WalletScreen::SetContentAreaBounds(const Rect& bounds)
{
    m_content_area_bounds = bounds;
    
    // Update content panel bounds to match the content area exactly
    if (m_content_panel) {
        // Use relative coordinates since the content panel is positioned within the content area
        Rect relative_bounds(0, 0, m_content_area_bounds.w, m_content_area_bounds.h);
        m_content_panel->SetBounds(relative_bounds);
    }
    
    // Reposition all elements based on new content area size
    RepositionElements(m_content_area_bounds.w, m_content_area_bounds.h);
}

void WalletScreen::OnResize(int new_width, int new_height)
{
    // Content area bounds are updated automatically by persistent layout via SetContentAreaBounds
    // This method is kept for compatibility but main positioning happens in SetContentAreaBounds
}

void WalletScreen::OnNavigatedTo(const NavigationContext& context)
{
    std::cout << "Navigated to Wallet screen" << std::endl;
    
    // Refresh wallet data when screen becomes active
    RefreshWalletData();
    RefreshTransactionHistory();
    
    // Handle navigation context
    if (context.HasParameter("action")) {
        std::string action = context.GetParameter("action");
        if (action == "send") {
            // Highlight send button
            if (m_send_button) {
                // Could add visual highlight here
            }
        } else if (action == "receive") {
            // Highlight receive button
            if (m_receive_button) {
                // Could add visual highlight here
            }
        }
    }
}

void WalletScreen::OnNavigatedFrom()
{
    std::cout << "Navigated away from Wallet screen" << std::endl;
    // Could save any temporary state here
}

void WalletScreen::UpdateWalletData()
{
    if (auto* wallet = m_gui.GetWalletManager()) {
        // Update balance displays
        auto balance = wallet->GetBalance();
        
        std::ostringstream confirmed_oss, unconfirmed_oss, total_oss;
        confirmed_oss << "Confirmed: " << std::fixed << std::setprecision(8) << balance.confirmed << " BTC";
        unconfirmed_oss << "Unconfirmed: " << std::fixed << std::setprecision(8) << balance.unconfirmed << " BTC";
        total_oss << "Total: " << std::fixed << std::setprecision(8) << balance.total << " BTC";
        
        if (m_confirmed_balance_label) {
            m_confirmed_balance_label->SetText(confirmed_oss.str());
        }
        
        if (m_unconfirmed_balance_label) {
            m_unconfirmed_balance_label->SetText(unconfirmed_oss.str());
        }
        
        if (m_total_balance_label) {
            m_total_balance_label->SetText(total_oss.str());
        }
        
        // Update wallet status
        if (m_wallet_status_label) {
            if (wallet->IsConnected()) {
                m_wallet_status_label->SetText("Status: Connected and Ready");
            } else {
                m_wallet_status_label->SetText("Status: Disconnected");
            }
        }
        
        if (m_encryption_status_label) {
            // Encryption status would be checked here in real implementation
            m_encryption_status_label->SetText("Encryption: Unknown");
        }
        
        if (m_backup_status_label) {
            // This would get actual backup date in real implementation
            m_backup_status_label->SetText("Last Backup: 2 days ago");
        }
    }
}

void WalletScreen::RefreshWalletData()
{
    UpdateWalletData();
}

void WalletScreen::RefreshTransactionHistory()
{
    // Update recent transactions (mock data for now)
    if (m_transaction_labels.size() >= 6) {
        m_transaction_labels[0]->SetText("📥 Received 0.001 BTC - 2 hours ago");
        m_transaction_labels[1]->SetText("📤 Sent 0.005 BTC - 1 day ago");
        m_transaction_labels[2]->SetText("📥 Received 0.002 BTC - 3 days ago");
        m_transaction_labels[3]->SetText("📤 Sent 0.001 BTC - 1 week ago");
        m_transaction_labels[4]->SetText("📥 Received 0.01 BTC - 2 weeks ago");
        m_transaction_labels[5]->SetText("📤 Sent 0.003 BTC - 1 month ago");
    }
}

void WalletScreen::RepositionElements(int content_width, int content_height)
{
    // Use dynamic positioning like main screen and send screen
    int padding = UIStyleGuide::Spacing::LG;
    int element_width = content_width - 2 * padding;
    int element_x = padding;
    int current_y = padding;
    
    // Position balance panel and its labels
    if (m_balance_panel) {
        int balance_panel_height = 120;
        m_balance_panel->SetBounds(Rect(element_x, current_y, element_width, balance_panel_height));
        
        // Position labels within the balance panel
        int label_x = element_x + UIStyleGuide::Spacing::MD;
        int label_y = current_y + UIStyleGuide::Spacing::MD;
        
        if (m_balance_title_label) {
            m_balance_title_label->SetPosition(Point(label_x, label_y));
            label_y += UIStyleGuide::FontSize::HEADING + UIStyleGuide::Spacing::SM;
        }
        
        if (m_confirmed_balance_label) {
            m_confirmed_balance_label->SetPosition(Point(label_x, label_y));
            label_y += UIStyleGuide::FontSize::BODY + UIStyleGuide::Spacing::XS;
        }
        
        if (m_unconfirmed_balance_label) {
            m_unconfirmed_balance_label->SetPosition(Point(label_x, label_y));
            label_y += UIStyleGuide::FontSize::CAPTION + UIStyleGuide::Spacing::XS;
        }
        
        if (m_total_balance_label) {
            m_total_balance_label->SetPosition(Point(label_x, label_y));
        }
        
        current_y += balance_panel_height + UIStyleGuide::Spacing::LG;
    }
    
    // Position actions panel exactly like main screen quick actions panel
    if (m_actions_panel) {
        int panel_height = UIStyleGuide::Accessibility::MIN_TOUCH_TARGET + (2 * UIStyleGuide::Spacing::MD);
        m_actions_panel->SetBounds(Rect(element_x, current_y, element_width, panel_height));
        
      // Position buttons within the panel - use flexible layout (exactly like main screen)
        int button_width = 140;
        int button_spacing = UIStyleGuide::Spacing::MD;
        int buttons_total_width = 4 * button_width + 3 * button_spacing; // 4 buttons now
        
        // For 4 buttons, use 2 rows if not enough space, otherwise 1 row
        if (element_width >= buttons_total_width) {
            // Single row (like main screen with 3 buttons)
            int button_start_x = element_x + ((element_width > buttons_total_width) ? 
                (element_width - buttons_total_width) / 2 : UIStyleGuide::Spacing::MD);
            int button_y = current_y + UIStyleGuide::Spacing::MD;
            
            if (m_send_button) {
                m_send_button->SetBounds(Rect(button_start_x, button_y, button_width, UIStyleGuide::Accessibility::MIN_TOUCH_TARGET));
              }
            if (m_receive_button) {
                m_receive_button->SetBounds(Rect(button_start_x + button_width + button_spacing, button_y, button_width, UIStyleGuide::Accessibility::MIN_TOUCH_TARGET));
            }
            if (m_backup_button) {
                m_backup_button->SetBounds(Rect(button_start_x + 2 * (button_width + button_spacing), button_y, button_width, UIStyleGuide::Accessibility::MIN_TOUCH_TARGET));
            }
            if (m_restore_button) {
                m_restore_button->SetBounds(Rect(button_start_x + 3 * (button_width + button_spacing), button_y, button_width, UIStyleGuide::Accessibility::MIN_TOUCH_TARGET));
            }
        } else {
            // Two rows (2 buttons per row)
            int buttons_per_row = 2;
            int row_width = buttons_per_row * button_width + (buttons_per_row - 1) * button_spacing;
            int button_start_x = element_x + ((element_width > row_width) ? 
                (element_width - row_width) / 2 : UIStyleGuide::Spacing::MD);
            int button_y = current_y + UIStyleGuide::Spacing::MD;
            
            // First row: Send, Receive
            if (m_send_button) {
                m_send_button->SetBounds(Rect(button_start_x, button_y, button_width, UIStyleGuide::Accessibility::MIN_TOUCH_TARGET));
            }
            if (m_receive_button) {
                m_receive_button->SetBounds(Rect(button_start_x + button_width + button_spacing, button_y, button_width, UIStyleGuide::Accessibility::MIN_TOUCH_TARGET));
            }
            
            // Second row: Backup, Restore
            button_y += UIStyleGuide::Accessibility::MIN_TOUCH_TARGET + UIStyleGuide::Spacing::SM;
            if (m_backup_button) {
                m_backup_button->SetBounds(Rect(button_start_x, button_y, button_width, UIStyleGuide::Accessibility::MIN_TOUCH_TARGET));
            }
            if (m_restore_button) {
                m_restore_button->SetBounds(Rect(button_start_x + button_width + button_spacing, button_y, button_width, UIStyleGuide::Accessibility::MIN_TOUCH_TARGET));
            }
        }
        
        current_y += panel_height + UIStyleGuide::Spacing::LG;
    }
    
    // Position transactions panel and its labels
    if (m_transactions_panel) {
        int transactions_panel_height = 200;
        m_transactions_panel->SetBounds(Rect(element_x, current_y, element_width, transactions_panel_height));
        
        // Position labels within the transactions panel
        int label_x = element_x + UIStyleGuide::Spacing::MD;
        int label_y = current_y + UIStyleGuide::Spacing::MD;
        
        if (m_transactions_title_label) {
            m_transactions_title_label->SetPosition(Point(label_x, label_y));
            label_y += UIStyleGuide::FontSize::SUBHEADING + UIStyleGuide::Spacing::SM;
        }
        
        // Position transaction labels
        for (auto& tx_label : m_transaction_labels) {
            if (tx_label) {
                tx_label->SetPosition(Point(label_x, label_y));
                label_y += UIStyleGuide::FontSize::CAPTION + UIStyleGuide::Spacing::XS;
            }
        }
        
        if (m_view_all_transactions_button) {
            m_view_all_transactions_button->SetBounds(Rect(label_x, label_y + UIStyleGuide::Spacing::SM, 150, UIStyleGuide::Accessibility::MIN_TOUCH_TARGET));
        }
        
        current_y += transactions_panel_height + UIStyleGuide::Spacing::LG;
    }
    
    // Position wallet info panel and its labels
    if (m_wallet_info_panel) {
        int wallet_info_panel_height = 80;
        m_wallet_info_panel->SetBounds(Rect(element_x, current_y, element_width, wallet_info_panel_height));
        
        // Position labels within the wallet info panel
        int label_x = element_x + UIStyleGuide::Spacing::MD;
        int label_y = current_y + UIStyleGuide::Spacing::MD;
        
        if (m_wallet_status_label) {
            m_wallet_status_label->SetPosition(Point(label_x, label_y));
            label_y += UIStyleGuide::FontSize::CAPTION + UIStyleGuide::Spacing::XS;
        }
        
        if (m_encryption_status_label) {
            m_encryption_status_label->SetPosition(Point(label_x, label_y));
            label_y += UIStyleGuide::FontSize::CAPTION + UIStyleGuide::Spacing::XS;
        }
        
        if (m_backup_status_label) {
            m_backup_status_label->SetPosition(Point(label_x, label_y));
        }
    }
}

void WalletScreen::OnSendClicked()
{
    std::cout << "Send button clicked - navigating to Send screen" << std::endl;
    m_gui.SwitchScreen(ScreenType::SEND);
}

void WalletScreen::OnReceiveClicked()
{
    std::cout << "Receive button clicked - navigating to Receive screen" << std::endl;
    m_gui.SwitchScreen(ScreenType::RECEIVE);
}

void WalletScreen::OnBackupClicked()
{
    std::cout << "Backup wallet clicked" << std::endl;
    // Implement wallet backup functionality
    if (auto* wallet = m_gui.GetWalletManager()) {
        // wallet->BackupWallet();
        (void)wallet; // Suppress unused variable warning
        std::cout << "Wallet backup initiated" << std::endl;
    }
}

void WalletScreen::OnRestoreClicked()
{
    std::cout << "Restore wallet clicked" << std::endl;
    // Implement wallet restore functionality
    if (auto* wallet = m_gui.GetWalletManager()) {
        // wallet->RestoreWallet();
        (void)wallet; // Suppress unused variable warning
        std::cout << "Wallet restore initiated" << std::endl;
    }
}

void WalletScreen::OnViewAllTransactionsClicked()
{
    std::cout << "View all transactions clicked - navigating to Transactions screen" << std::endl;
    m_gui.SwitchScreen(ScreenType::TRANSACTIONS);
}