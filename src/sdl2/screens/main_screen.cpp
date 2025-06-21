// Copyright (c) 2025 The Gotham Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or https://opensource.org/license/mit/.

#include "main_screen.h"
#include "../gotham_city_gui.h"
#include "../utils/theme.h"
#include "../utils/font_manager.h"
#include "../ui/ui_style_guide.h"
#include "../ui/ui_utils.h"
#include "../wallet/wallet_manager.h"
#include <iostream>
#include <iomanip>
#include <sstream>

MainScreen::MainScreen(GothamCityGUI& gui)
    : Screen(gui)
{
    // Initialize UI systems
    if (auto* theme_manager = m_gui.GetThemeManager()) {
        m_ui_factory = std::make_unique<UIFactory>(*theme_manager);
    }
    m_layout_manager = std::make_unique<LayoutManager>();
}

bool MainScreen::Initialize()
{
    if (m_initialized) {
        return true;
    }

    // Only create content area elements (layout will be applied when bounds are set)
    CreateContentElements();
    SetupButtonCallbacks();
    
    // Note: UI validation will happen after content area bounds are set

    m_initialized = true;
    std::cout << "Main screen initialized (content area only)" << std::endl;
    return true;
}

void MainScreen::CreateContentElements()
{
    if (!m_ui_factory) return;
    
    // Create welcome message
    m_welcome_label = m_ui_factory->CreateLabel(
        "Welcome to Gotham City", 
        Point(0, 0), // Position will be set by layout
        LabelStyle::HEADING
    );
    
    // Create balance display
    m_balance_display_label = m_ui_factory->CreateLabel(
        "Wallet Balance: Loading...", 
        Point(0, 0), // Position will be set by layout
        LabelStyle::BODY
    );
    
    CreateQuickActionsPanel();
    CreateWalletStatusPanel();
    CreateRecentActivityPanel();
}

void MainScreen::CreateContentPanel()
{
    if (!m_ui_factory) return;
    
    // Create main content panel that fills the content area
    // Use relative coordinates since viewport handles translation
    Rect relative_content_bounds(0, 0, m_content_area_bounds.w, m_content_area_bounds.h);
    m_content_panel = m_ui_factory->CreatePanel(
        relative_content_bounds,
        PanelStyle::CONTENT
    );
    
    // Add child elements to the content panel (convert unique_ptr to shared_ptr)
    if (m_content_panel) {
        // Add main content labels
        if (m_welcome_label) {
            m_content_panel->AddLabel(std::shared_ptr<Label>(m_welcome_label.get(), [](Label*){}));
        }
        if (m_balance_display_label) {
            m_content_panel->AddLabel(std::shared_ptr<Label>(m_balance_display_label.get(), [](Label*){}));
        }
        
        // Add child panels
        if (m_quick_actions_panel) {
            m_content_panel->AddPanel(std::shared_ptr<Panel>(m_quick_actions_panel.get(), [](Panel*){}));
        }
        if (m_wallet_status_panel) {
            m_content_panel->AddPanel(std::shared_ptr<Panel>(m_wallet_status_panel.get(), [](Panel*){}));
        }
        if (m_recent_activity_panel) {
            m_content_panel->AddPanel(std::shared_ptr<Panel>(m_recent_activity_panel.get(), [](Panel*){}));
        }
    }
    
    std::cout << "Created content panel with relative bounds: " << 0 << "," 
              << 0 << " " << m_content_area_bounds.w << "x" 
              << m_content_area_bounds.h << std::endl;
}

void MainScreen::CreateQuickActionsPanel()
{
    // Create quick actions panel - size will be set by layout when content area bounds are known
    // Initial size is placeholder, will be resized properly later
    m_quick_actions_panel = m_ui_factory->CreatePanel(
        Rect(0, 0, 100, 84), // Minimal placeholder size
        PanelStyle::CARD
    );
    
    // Create quick action buttons - positions will be set by layout
    // Using minimum touch target size (44x44) for accessibility
    m_send_quick_button = m_ui_factory->CreateButton(
        "📤 Send", 
        Rect(0, 0, 140, 44), 
        ButtonStyle::PRIMARY
    );
    
    m_receive_quick_button = m_ui_factory->CreateButton(
        "📥 Receive", 
        Rect(0, 0, 140, 44), 
        ButtonStyle::SECONDARY
    );
    
    m_transactions_quick_button = m_ui_factory->CreateButton(
        "📋 History", 
        Rect(0, 0, 140, 44), 
        ButtonStyle::SECONDARY
    );
    
    // Add buttons to the quick actions panel
    if (m_quick_actions_panel) {
        if (m_send_quick_button) {
            m_quick_actions_panel->AddButton(std::shared_ptr<Button>(m_send_quick_button.get(), [](Button*){}));
        }
        if (m_receive_quick_button) {
            m_quick_actions_panel->AddButton(std::shared_ptr<Button>(m_receive_quick_button.get(), [](Button*){}));
        }
        if (m_transactions_quick_button) {
            m_quick_actions_panel->AddButton(std::shared_ptr<Button>(m_transactions_quick_button.get(), [](Button*){}));
        }
    }
}

void MainScreen::CreateWalletStatusPanel()
{
    // Create wallet status panel - size will be set by layout when content area bounds are known
    m_wallet_status_panel = m_ui_factory->CreatePanel(
        Rect(0, 0, 100, 120), // Minimal placeholder size
        PanelStyle::INFO
    );
    
    // Status labels - positions will be set by layout
    m_network_status_label = m_ui_factory->CreateLabel(
        "Network: Connecting...", 
        Point(0, 0), 
        LabelStyle::CAPTION
    );
    
    m_sync_status_label = m_ui_factory->CreateLabel(
        "Sync: 0%", 
        Point(0, 0), 
        LabelStyle::CAPTION
    );
    
    m_wallet_status_label = m_ui_factory->CreateLabel(
        "Wallet: Loading...", 
        Point(0, 0), 
        LabelStyle::CAPTION
    );
    
    // Add labels to the wallet status panel
    if (m_wallet_status_panel) {
        if (m_network_status_label) {
            m_wallet_status_panel->AddLabel(std::shared_ptr<Label>(m_network_status_label.get(), [](Label*){}));
        }
        if (m_sync_status_label) {
            m_wallet_status_panel->AddLabel(std::shared_ptr<Label>(m_sync_status_label.get(), [](Label*){}));
        }
        if (m_wallet_status_label) {
            m_wallet_status_panel->AddLabel(std::shared_ptr<Label>(m_wallet_status_label.get(), [](Label*){}));
        }
    }
}

void MainScreen::CreateRecentActivityPanel()
{
    // Create recent activity panel - size will be set by layout when content area bounds are known
    m_recent_activity_panel = m_ui_factory->CreatePanel(
        Rect(0, 0, 100, 200), // Minimal placeholder size
        PanelStyle::CARD
    );
    
    m_recent_activity_title = m_ui_factory->CreateLabel(
        "Recent Activity", 
        Point(0, 0), 
        LabelStyle::SUBHEADING
    );
    
    // Create placeholder transaction labels - positions will be set by layout
    for (int i = 0; i < 5; ++i) {
        auto tx_label = m_ui_factory->CreateLabel(
            "No recent transactions", 
            Point(0, 0), 
            LabelStyle::CAPTION
        );
        m_recent_transactions.push_back(std::move(tx_label));
    }
    
    // Add labels to the recent activity panel
    if (m_recent_activity_panel) {
        if (m_recent_activity_title) {
            m_recent_activity_panel->AddLabel(std::shared_ptr<Label>(m_recent_activity_title.get(), [](Label*){}));
        }
        for (auto& tx_label : m_recent_transactions) {
            if (tx_label) {
                m_recent_activity_panel->AddLabel(std::shared_ptr<Label>(tx_label.get(), [](Label*){}));
            }
        }
    }
}

void MainScreen::SetupButtonCallbacks()
{
    if (m_send_quick_button) {
        m_send_quick_button->SetOnClick([this]() {
            OnSendQuickClicked();
        });
    }
    
    if (m_receive_quick_button) {
        m_receive_quick_button->SetOnClick([this]() {
            OnReceiveQuickClicked();
        });
    }
    
    if (m_transactions_quick_button) {
        m_transactions_quick_button->SetOnClick([this]() {
            OnTransactionsQuickClicked();
        });
    }
}

void MainScreen::HandleEvent(const SDL_Event& event)
{
    // Handle events through the content panel hierarchy
    // The content panel will automatically handle events for all its nested child panels
    if (m_content_panel) {
        m_content_panel->HandleEvent(event);
        
        // Still need to handle button events directly since buttons need direct handling
        if (m_send_quick_button) m_send_quick_button->HandleEvent(event);
        if (m_receive_quick_button) m_receive_quick_button->HandleEvent(event);
        if (m_transactions_quick_button) m_transactions_quick_button->HandleEvent(event);
    }
}

void MainScreen::Update(float delta_time)
{
    m_elapsed_time += delta_time;
    
    // Update content panel and all its children
    if (m_content_panel) {
        m_content_panel->Update(delta_time);
    }
    
    // Still need to update buttons directly since they need explicit updates
    if (m_send_quick_button) m_send_quick_button->Update(delta_time);
    if (m_receive_quick_button) m_receive_quick_button->Update(delta_time);
    if (m_transactions_quick_button) m_transactions_quick_button->Update(delta_time);
    
    // Update wallet and network info periodically
    static float last_update = 0.0f;
    if (m_elapsed_time - last_update > 2.0f) {
        UpdateWalletInfo();
        UpdateNetworkInfo();
        last_update = m_elapsed_time;
    }
}

void MainScreen::Render(Renderer& renderer)
{
    // Render the main content panel (which fills the entire content area)
    // This will render the panel background and all nested child panels
    if (m_content_panel) {
        m_content_panel->Render(renderer);
        
        // Get fonts for rendering child elements
        TTF_Font* heading_font = m_gui.GetFontManager()->GetDefaultFont(UIStyleGuide::FontSize::HEADING);
        TTF_Font* body_font = m_gui.GetFontManager()->GetDefaultFont(UIStyleGuide::FontSize::BODY);
        TTF_Font* caption_font = m_gui.GetFontManager()->GetDefaultFont(UIStyleGuide::FontSize::CAPTION);
        
        // Render main content labels
        if (m_welcome_label && heading_font) {
            m_welcome_label->Render(renderer, heading_font);
        }
        
        if (m_balance_display_label && body_font) {
            m_balance_display_label->Render(renderer, body_font);
        }
        
        // Render quick action buttons
        if (m_send_quick_button && body_font) {
            m_send_quick_button->Render(renderer, body_font);
        }
        
        if (m_receive_quick_button && body_font) {
            m_receive_quick_button->Render(renderer, body_font);
        }
        
        if (m_transactions_quick_button && body_font) {
            m_transactions_quick_button->Render(renderer, body_font);
        }
        
        // Render status labels
        if (m_network_status_label && caption_font) {
            m_network_status_label->Render(renderer, caption_font);
        }
        
        if (m_sync_status_label && caption_font) {
            m_sync_status_label->Render(renderer, caption_font);
        }
        
        if (m_wallet_status_label && caption_font) {
            m_wallet_status_label->Render(renderer, caption_font);
        }
        
        // Render recent activity
        if (m_recent_activity_title && body_font) {
            m_recent_activity_title->Render(renderer, body_font);
        }
        
        for (auto& tx_label : m_recent_transactions) {
            if (tx_label && caption_font) {
                tx_label->Render(renderer, caption_font);
            }
        }
    }
}

void MainScreen::OnActivate()
{
    std::cout << "Main screen activated" << std::endl;
    RefreshWalletData();
}

void MainScreen::OnResize(int new_width, int new_height)
{
    // Content area bounds will be updated by SetContentAreaBounds() call from PersistentLayout
    // No need to reposition here as SetContentAreaBounds() will handle positioning
    
    // Apply responsive layout based on new dimensions
    ApplyResponsiveLayout();
    
    // Validate UI after resize
    ValidateUIElements();
}

void MainScreen::SetContentAreaBounds(const Rect& bounds)
{
    m_content_area_bounds = bounds;
    std::cout << "Setting content area bounds: " << bounds.x << "," << bounds.y 
              << " " << bounds.w << "x" << bounds.h << std::endl;
    
    // Update content panel bounds if it exists, or create it if it doesn't
    if (m_content_panel) {
        // Use relative coordinates since viewport handles translation
        Rect relative_bounds(0, 0, m_content_area_bounds.w, m_content_area_bounds.h);
        m_content_panel->SetBounds(relative_bounds);
        std::cout << "Updated existing content panel to relative bounds: 0,0 " 
                  << m_content_area_bounds.w << "x" << m_content_area_bounds.h << std::endl;
    } else {
        CreateContentPanel();
        std::cout << "Created new content panel" << std::endl;
    }
    
    // Position all elements directly within the content area
    PositionElementsDirectly();
    
    // Apply responsive layout for the new bounds
    ApplyResponsiveLayout();
}

void MainScreen::OnNavigatedTo(const NavigationContext& context)
{
    std::cout << "Navigated to Main screen" << std::endl;
    
    // Refresh data when screen becomes active
    RefreshWalletData();
    RefreshRecentTransactions();
    
    // Handle navigation context
    if (context.HasParameter("highlight")) {
        std::string highlight = context.GetParameter("highlight");
        if (highlight == "balance") {
            // Highlight balance area
            if (m_balance_display_label) {
                m_balance_display_label->SetColor(UIStyleGuide::Colors::PRIMARY);
            }
        }
    }
}

void MainScreen::OnNavigatedFrom()
{
    std::cout << "Navigated away from Main screen" << std::endl;
    
    // Reset any highlights
    if (m_balance_display_label) {
        m_balance_display_label->SetColor(UIStyleGuide::Colors::TEXT_PRIMARY);
    }
}

void MainScreen::UpdateWalletInfo()
{
    if (auto* wallet = m_gui.GetWalletManager()) {
        auto balance = wallet->GetBalance();
        std::ostringstream oss;
        oss << "Wallet Balance: " << std::fixed << std::setprecision(8) << balance.total << " BTC";
        
        if (m_balance_display_label) {
            m_balance_display_label->SetText(oss.str());
        }
        
        // Update wallet status
        if (m_wallet_status_label) {
            if (wallet->IsConnected()) {
                m_wallet_status_label->SetText("Wallet: Connected");
                m_wallet_connected = true;
            } else {
                m_wallet_status_label->SetText("Wallet: Disconnected");
                m_wallet_connected = false;
            }
        }
    }
}

void MainScreen::UpdateNetworkInfo()
{
    // Update network status (mock data for now)
    if (m_network_status_label) {
        m_network_status_label->SetText("Network: 8 connections");
    }
    
    if (m_sync_status_label) {
        m_sync_status_label->SetText("Sync: 100% (Block 750,123)");
    }
}

void MainScreen::RefreshWalletData()
{
    UpdateWalletInfo();
    UpdateNetworkInfo();
}

void MainScreen::RefreshRecentTransactions()
{
    // Update recent transactions display
    // This would fetch from wallet manager in real implementation
    if (!m_recent_transactions.empty()) {
        m_recent_transactions[0]->SetText("Received 0.001 BTC - 2 hours ago");
        m_recent_transactions[1]->SetText("Sent 0.005 BTC - 1 day ago");
        m_recent_transactions[2]->SetText("Received 0.002 BTC - 3 days ago");
        m_recent_transactions[3]->SetText("Sent 0.001 BTC - 1 week ago");
        m_recent_transactions[4]->SetText("Click 'History' to view all transactions");
    }
}

void MainScreen::ApplyContentLayout()
{
    if (!m_layout_manager) return;
    
    // Create layout items for main content
    std::vector<LayoutItem> content_items;
    
    // Welcome label
    if (m_welcome_label) {
        LayoutItem welcome_item;
        welcome_item.constraints.preferred_height = UIStyleGuide::FontSize::HEADING;
        welcome_item.margin = Margin(UIStyleGuide::Spacing::MD, UIStyleGuide::Spacing::LG, UIStyleGuide::Spacing::SM, UIStyleGuide::Spacing::LG);
        welcome_item.on_bounds_changed = [this](const Rect& bounds) {
            m_welcome_label->SetPosition(Point(bounds.x, bounds.y));
        };
        content_items.push_back(welcome_item);
    }
    
    // Balance label
    if (m_balance_display_label) {
        LayoutItem balance_item;
        balance_item.constraints.preferred_height = UIStyleGuide::FontSize::BODY;
        balance_item.margin = Margin(0, UIStyleGuide::Spacing::LG, UIStyleGuide::Spacing::MD, UIStyleGuide::Spacing::LG);
        balance_item.on_bounds_changed = [this](const Rect& bounds) {
            m_balance_display_label->SetPosition(Point(bounds.x, bounds.y));
        };
        content_items.push_back(balance_item);
    }
    
    // Quick actions panel
    if (m_quick_actions_panel) {
        LayoutItem actions_item;
        actions_item.constraints.preferred_height = UIStyleGuide::Accessibility::MIN_TOUCH_TARGET + (2 * UIStyleGuide::Spacing::MD);
        actions_item.margin = Margin(UIStyleGuide::Spacing::MD, UIStyleGuide::Spacing::LG, UIStyleGuide::Spacing::MD, UIStyleGuide::Spacing::LG);
        actions_item.on_bounds_changed = [this](const Rect& bounds) {
            m_quick_actions_panel->SetBounds(bounds);
            ApplyQuickActionsLayout(bounds);
        };
        content_items.push_back(actions_item);
    }
    
    // Wallet status panel
    if (m_wallet_status_panel) {
        LayoutItem status_item;
        status_item.constraints.preferred_height = (3 * UIStyleGuide::FontSize::CAPTION) + (4 * UIStyleGuide::Spacing::MD);
        status_item.margin = Margin(UIStyleGuide::Spacing::MD, UIStyleGuide::Spacing::LG, UIStyleGuide::Spacing::MD, UIStyleGuide::Spacing::LG);
        status_item.on_bounds_changed = [this](const Rect& bounds) {
            m_wallet_status_panel->SetBounds(bounds);
            ApplyStatusPanelLayout(bounds);
        };
        content_items.push_back(status_item);
    }
    
    // Recent activity panel
    if (m_recent_activity_panel) {
        LayoutItem activity_item;
        activity_item.constraints.preferred_height = UIStyleGuide::FontSize::SUBHEADING + (5 * UIStyleGuide::FontSize::CAPTION) + (7 * UIStyleGuide::Spacing::SM);
        activity_item.constraints.weight = 1.0f; // Take remaining space
        activity_item.margin = Margin(UIStyleGuide::Spacing::MD, UIStyleGuide::Spacing::LG, UIStyleGuide::Spacing::LG, UIStyleGuide::Spacing::LG);
        activity_item.on_bounds_changed = [this](const Rect& bounds) {
            m_recent_activity_panel->SetBounds(bounds);
            ApplyRecentActivityLayout(bounds);
        };
        content_items.push_back(activity_item);
    }
    
    // Create content area relative to (0,0) since viewport handles translation
    Rect relative_content_area = Rect(0, 0, m_content_area_bounds.w, m_content_area_bounds.h);
    
    // Apply content layout - use full content area width
    int full_width = m_content_area_bounds.w - (2 * UIStyleGuide::Spacing::LG); // Full width minus padding
    m_layout_manager->CreateContentLayout(relative_content_area, content_items, full_width, Alignment::START);
}

void MainScreen::ApplyQuickActionsLayout(const Rect& panel_bounds)
{
    if (!m_layout_manager) return;
    
    std::vector<LayoutItem> button_items;
    
    // Create layout items for buttons
    if (m_send_quick_button) {
        LayoutItem send_item;
        send_item.constraints.preferred_width = 140;
        send_item.constraints.preferred_height = UIStyleGuide::Accessibility::MIN_TOUCH_TARGET;
        send_item.margin = Margin(UIStyleGuide::Spacing::MD, UIStyleGuide::Spacing::MD, UIStyleGuide::Spacing::MD, UIStyleGuide::Spacing::MD);
        send_item.on_bounds_changed = [this](const Rect& bounds) {
            m_send_quick_button->SetBounds(bounds);
        };
        button_items.push_back(send_item);
    }
    
    if (m_receive_quick_button) {
        LayoutItem receive_item;
        receive_item.constraints.preferred_width = 140;
        receive_item.constraints.preferred_height = UIStyleGuide::Accessibility::MIN_TOUCH_TARGET;
        receive_item.margin = Margin(UIStyleGuide::Spacing::MD, UIStyleGuide::Spacing::SM, UIStyleGuide::Spacing::MD, UIStyleGuide::Spacing::SM);
        receive_item.on_bounds_changed = [this](const Rect& bounds) {
            m_receive_quick_button->SetBounds(bounds);
        };
        button_items.push_back(receive_item);
    }
    
    if (m_transactions_quick_button) {
        LayoutItem history_item;
        history_item.constraints.preferred_width = 140;
        history_item.constraints.preferred_height = UIStyleGuide::Accessibility::MIN_TOUCH_TARGET;
        history_item.margin = Margin(UIStyleGuide::Spacing::MD, UIStyleGuide::Spacing::MD, UIStyleGuide::Spacing::MD, UIStyleGuide::Spacing::SM);
        history_item.on_bounds_changed = [this](const Rect& bounds) {
            m_transactions_quick_button->SetBounds(bounds);
        };
        button_items.push_back(history_item);
    }
    
    // Apply responsive button layout using UI utilities
    UIUtils::ResponsiveButtonLayout::Apply(*m_layout_manager, panel_bounds, button_items, m_content_area_bounds.w);
}

void MainScreen::ApplyStatusPanelLayout(const Rect& panel_bounds)
{
    if (!m_layout_manager) return;
    
    std::vector<LayoutItem> status_items;
    
    // Create layout items for status labels
    if (m_network_status_label) {
        LayoutItem network_item;
        network_item.constraints.preferred_height = UIStyleGuide::FontSize::CAPTION;
        network_item.margin = Margin(UIStyleGuide::Spacing::MD, UIStyleGuide::Spacing::MD, UIStyleGuide::Spacing::XS, UIStyleGuide::Spacing::MD);
        network_item.on_bounds_changed = [this](const Rect& bounds) {
            m_network_status_label->SetPosition(Point(bounds.x, bounds.y));
        };
        status_items.push_back(network_item);
    }
    
    if (m_sync_status_label) {
        LayoutItem sync_item;
        sync_item.constraints.preferred_height = UIStyleGuide::FontSize::CAPTION;
        sync_item.margin = Margin(UIStyleGuide::Spacing::XS, UIStyleGuide::Spacing::MD, UIStyleGuide::Spacing::XS, UIStyleGuide::Spacing::MD);
        sync_item.on_bounds_changed = [this](const Rect& bounds) {
            m_sync_status_label->SetPosition(Point(bounds.x, bounds.y));
        };
        status_items.push_back(sync_item);
    }
    
    if (m_wallet_status_label) {
        LayoutItem wallet_item;
        wallet_item.constraints.preferred_height = UIStyleGuide::FontSize::CAPTION;
        wallet_item.margin = Margin(UIStyleGuide::Spacing::XS, UIStyleGuide::Spacing::MD, UIStyleGuide::Spacing::MD, UIStyleGuide::Spacing::MD);
        wallet_item.on_bounds_changed = [this](const Rect& bounds) {
            m_wallet_status_label->SetPosition(Point(bounds.x, bounds.y));
        };
        status_items.push_back(wallet_item);
    }
    
    // Apply vertical layout for status labels with consistent spacing
    m_layout_manager->CreateVerticalLayout(panel_bounds, status_items, UIStyleGuide::Spacing::XS, Alignment::STRETCH);
}

void MainScreen::ApplyRecentActivityLayout(const Rect& panel_bounds)
{
    if (!m_layout_manager) return;
    
    std::vector<LayoutItem> activity_items;
    
    // Title
    if (m_recent_activity_title) {
        LayoutItem title_item;
        title_item.constraints.preferred_height = UIStyleGuide::FontSize::SUBHEADING;
        title_item.margin = Margin(UIStyleGuide::Spacing::MD, UIStyleGuide::Spacing::MD, UIStyleGuide::Spacing::SM, UIStyleGuide::Spacing::MD);
        title_item.on_bounds_changed = [this](const Rect& bounds) {
            m_recent_activity_title->SetPosition(Point(bounds.x, bounds.y));
        };
        activity_items.push_back(title_item);
    }
    
    // Transaction labels
    for (auto& tx_label : m_recent_transactions) {
        if (tx_label) {
            LayoutItem tx_item;
            tx_item.constraints.preferred_height = UIStyleGuide::FontSize::CAPTION;
            tx_item.margin = Margin(UIStyleGuide::Spacing::XS, UIStyleGuide::Spacing::MD, UIStyleGuide::Spacing::XS, UIStyleGuide::Spacing::MD);
            tx_item.on_bounds_changed = [&tx_label](const Rect& bounds) {
                tx_label->SetPosition(Point(bounds.x, bounds.y));
            };
            activity_items.push_back(tx_item);
        }
    }
    
    // Apply vertical layout for activity items with consistent spacing
    m_layout_manager->CreateVerticalLayout(panel_bounds, activity_items, UIStyleGuide::Spacing::XS, Alignment::STRETCH);
}

void MainScreen::PositionElementsDirectly()
{
    // Content positioning should be RELATIVE within the content area bounds
    // Viewport translation is handled by PersistentLayout, so use (0,0) as starting point
    int content_w = m_content_area_bounds.w;
    int content_h = m_content_area_bounds.h;
    
    // Use style guide spacing for consistent layout
    int padding = UIStyleGuide::Spacing::LG;
    int current_y = padding; // Start at relative position (0,0) within content area
    int element_width = content_w - 2 * padding; // Use available width minus padding
    int element_x = padding; // Start at relative position within content area
    
    std::cout << "Positioning elements within content area (relative): " << 0 << "," << 0 
              << " " << content_w << "x" << content_h << std::endl;
    
    
    // Position welcome label
    if (m_welcome_label) {
        m_welcome_label->SetPosition(Point(element_x, current_y));
        current_y += UIStyleGuide::FontSize::HEADING + UIStyleGuide::Spacing::LG; // Height + spacing
    }
    
    // Position balance label
    if (m_balance_display_label) {
        m_balance_display_label->SetPosition(Point(element_x, current_y));
        current_y += UIStyleGuide::FontSize::BODY + UIStyleGuide::Spacing::LG; // Height + spacing
    }
    
    // Position quick actions panel
    if (m_quick_actions_panel) {
        int panel_height = UIStyleGuide::Accessibility::MIN_TOUCH_TARGET + (2 * UIStyleGuide::Spacing::MD);
        m_quick_actions_panel->SetBounds(Rect(element_x, current_y, element_width, panel_height));
        
        // Position buttons within the panel - use flexible layout
        int button_width = 140;
        int button_spacing = UIStyleGuide::Spacing::MD;
        int buttons_total_width = 3 * button_width + 2 * button_spacing;
        
        // Center buttons if there's enough space, otherwise use left alignment
        int button_start_x = element_x + ((element_width > buttons_total_width) ? 
            (element_width - buttons_total_width) / 2 : UIStyleGuide::Spacing::MD);
        int button_y = current_y + UIStyleGuide::Spacing::MD; // Relative position within content area
        
        if (m_send_quick_button) {
            m_send_quick_button->SetBounds(Rect(button_start_x, button_y, button_width, UIStyleGuide::Accessibility::MIN_TOUCH_TARGET));
        }
        if (m_receive_quick_button) {
            m_receive_quick_button->SetBounds(Rect(button_start_x + button_width + button_spacing, button_y, button_width, UIStyleGuide::Accessibility::MIN_TOUCH_TARGET));
        }
        if (m_transactions_quick_button) {
            m_transactions_quick_button->SetBounds(Rect(button_start_x + 2 * (button_width + button_spacing), button_y, button_width, UIStyleGuide::Accessibility::MIN_TOUCH_TARGET));
        }
        
        current_y += panel_height + UIStyleGuide::Spacing::LG; // Panel height + spacing
    }
    
    // Position wallet status panel
    if (m_wallet_status_panel) {
        int panel_height = (3 * UIStyleGuide::FontSize::CAPTION) + (4 * UIStyleGuide::Spacing::MD);
        m_wallet_status_panel->SetBounds(Rect(element_x, current_y, element_width, panel_height));
        
        // Position status labels within the panel - use relative positioning within content area
        int label_y = current_y + UIStyleGuide::Spacing::MD; // Relative position within content area
        int label_x = element_x + UIStyleGuide::Spacing::MD; // Relative position within content area
        
        if (m_network_status_label) {
            m_network_status_label->SetPosition(Point(label_x, label_y));
            label_y += UIStyleGuide::FontSize::CAPTION + UIStyleGuide::Spacing::SM;
        }
        if (m_sync_status_label) {
            m_sync_status_label->SetPosition(Point(label_x, label_y));
            label_y += UIStyleGuide::FontSize::CAPTION + UIStyleGuide::Spacing::SM;
        }
        if (m_wallet_status_label) {
            m_wallet_status_label->SetPosition(Point(label_x, label_y));
        }
        
        current_y += panel_height + UIStyleGuide::Spacing::LG; // Panel height + spacing
    }
    
    // Position recent activity panel - use remaining space
    if (m_recent_activity_panel) {
        int remaining_height = content_h - current_y - padding;
        int min_panel_height = UIStyleGuide::FontSize::SUBHEADING + (5 * UIStyleGuide::FontSize::CAPTION) + (7 * UIStyleGuide::Spacing::SM);
        int panel_height = std::max(min_panel_height, remaining_height);
        
        m_recent_activity_panel->SetBounds(Rect(element_x, current_y, element_width, panel_height));
        
        // Position activity elements within the panel - use relative coordinates within content area
        int activity_y = current_y + UIStyleGuide::Spacing::MD; // Relative position within content area
        int activity_x = element_x + UIStyleGuide::Spacing::MD; // Relative position within content area
        
        if (m_recent_activity_title) {
            m_recent_activity_title->SetPosition(Point(activity_x, activity_y));
            activity_y += UIStyleGuide::FontSize::SUBHEADING + UIStyleGuide::Spacing::MD;
        }
        
        // Position transaction labels with consistent spacing
        for (auto& tx_label : m_recent_transactions) {
            if (tx_label) {
                tx_label->SetPosition(Point(activity_x, activity_y));
                activity_y += UIStyleGuide::FontSize::CAPTION + UIStyleGuide::Spacing::SM;
            }
        }
    }
}

void MainScreen::RepositionElements(int content_width, int content_height)
{
    // Update content area bounds and reposition directly
    m_content_area_bounds.w = content_width;
    m_content_area_bounds.h = content_height;
    
    PositionElementsDirectly();
}

void MainScreen::OnSendQuickClicked()
{
    std::cout << "Quick Send clicked - navigating to Send screen" << std::endl;
    m_gui.SwitchScreen(ScreenType::SEND);
}

void MainScreen::OnReceiveQuickClicked()
{
    std::cout << "Quick Receive clicked - navigating to Receive screen" << std::endl;
    m_gui.SwitchScreen(ScreenType::RECEIVE);
}

void MainScreen::OnTransactionsQuickClicked()
{
    std::cout << "Quick Transactions clicked - navigating to Transactions screen" << std::endl;
    m_gui.SwitchScreen(ScreenType::TRANSACTIONS);
}

void MainScreen::ApplyResponsiveLayout()
{
    // Get screen size and determine responsive behavior
    int screen_width = m_content_area_bounds.w;
    UIStyleGuide::Responsive::ScreenSize screen_size = UIStyleGuide::Responsive::GetScreenSize(screen_width);
    
    // Adjust button layout based on screen size
    if (screen_size == UIStyleGuide::Responsive::ScreenSize::MOBILE) {
        // Stack buttons vertically on mobile
        if (m_quick_actions_panel) {
            std::vector<LayoutItem> button_items;
            
            if (m_send_quick_button) {
                LayoutItem send_item;
                send_item.constraints.preferred_width = screen_width - (2 * UIStyleGuide::Spacing::LG);
                send_item.constraints.preferred_height = UIStyleGuide::Accessibility::MIN_TOUCH_TARGET;
                send_item.margin = Margin(UIStyleGuide::Spacing::SM, UIStyleGuide::Spacing::MD, UIStyleGuide::Spacing::SM, UIStyleGuide::Spacing::MD);
                send_item.on_bounds_changed = [this](const Rect& bounds) {
                    m_send_quick_button->SetBounds(bounds);
                };
                button_items.push_back(send_item);
            }
            
            if (m_receive_quick_button) {
                LayoutItem receive_item;
                receive_item.constraints.preferred_width = screen_width - (2 * UIStyleGuide::Spacing::LG);
                receive_item.constraints.preferred_height = UIStyleGuide::Accessibility::MIN_TOUCH_TARGET;
                receive_item.margin = Margin(UIStyleGuide::Spacing::SM, UIStyleGuide::Spacing::MD, UIStyleGuide::Spacing::SM, UIStyleGuide::Spacing::MD);
                receive_item.on_bounds_changed = [this](const Rect& bounds) {
                    m_receive_quick_button->SetBounds(bounds);
                };
                button_items.push_back(receive_item);
            }
            
            if (m_transactions_quick_button) {
                LayoutItem history_item;
                history_item.constraints.preferred_width = screen_width - (2 * UIStyleGuide::Spacing::LG);
                history_item.constraints.preferred_height = UIStyleGuide::Accessibility::MIN_TOUCH_TARGET;
                history_item.margin = Margin(UIStyleGuide::Spacing::SM, UIStyleGuide::Spacing::MD, UIStyleGuide::Spacing::SM, UIStyleGuide::Spacing::MD);
                history_item.on_bounds_changed = [this](const Rect& bounds) {
                    m_transactions_quick_button->SetBounds(bounds);
                };
                button_items.push_back(history_item);
            }
            
            // Apply vertical layout for mobile
            Rect panel_bounds = m_quick_actions_panel->GetBounds();
            panel_bounds.h = 3 * UIStyleGuide::Accessibility::MIN_TOUCH_TARGET + (4 * UIStyleGuide::Spacing::SM);
            m_quick_actions_panel->SetBounds(panel_bounds);
            m_layout_manager->CreateVerticalLayout(panel_bounds, button_items, UIStyleGuide::Spacing::SM, Alignment::STRETCH);
        }
    }
    
    // Adjust spacing based on screen size
    int responsive_spacing = UIStyleGuide::Responsive::GetSpacing(screen_size);
    
    // Applied responsive layout for current screen size
}

void MainScreen::ValidateUIElements()
{
    // Validate that all UI elements are properly initialized and positioned
    bool validation_passed = true;
    
    if (!m_ui_factory) {
        UIUtils::ErrorHandler::LogUIError("UI Factory not initialized", "MainScreen");
        validation_passed = false;
    }
    
    if (!m_layout_manager) {
        UIUtils::ErrorHandler::LogUIError("Layout Manager not initialized", "MainScreen");
        validation_passed = false;
    }
    
    // Check if critical UI elements exist
    if (!m_welcome_label) {
        std::cerr << "WARNING: Welcome label not created" << std::endl;
    }
    
    if (!m_balance_display_label) {
        std::cerr << "WARNING: Balance display label not created" << std::endl;
    }
    
    // Validate button accessibility using UI utilities
    std::vector<Rect> button_bounds;
    if (m_send_quick_button) {
        Rect bounds = m_send_quick_button->GetBounds();
        button_bounds.push_back(bounds);
        UIUtils::AccessibilityValidator::LogAccessibilityWarnings("Send Button", bounds);
    }
    
    if (m_receive_quick_button) {
        Rect bounds = m_receive_quick_button->GetBounds();
        button_bounds.push_back(bounds);
        UIUtils::AccessibilityValidator::LogAccessibilityWarnings("Receive Button", bounds);
    }
    
    if (m_transactions_quick_button) {
        Rect bounds = m_transactions_quick_button->GetBounds();
        button_bounds.push_back(bounds);
        UIUtils::AccessibilityValidator::LogAccessibilityWarnings("Transactions Button", bounds);
    }
    
    // Validate all button touch targets at once
    if (!button_bounds.empty()) {
        UIUtils::AccessibilityValidator::ValidateMinimumTouchTargets(button_bounds);
    }
    
    // Validate content area bounds with error recovery
    Rect fallback_bounds(0, 0, 1000, 690); // Use realistic content area size
    if (UIUtils::ErrorHandler::RecoverFromInvalidBounds(m_content_area_bounds, fallback_bounds)) {
        validation_passed = false;
    }
    
    // Log layout information for debugging
    UIUtils::LayoutDebugger::LogLayoutInfo("Content Area", m_content_area_bounds);
    if (m_quick_actions_panel) {
        UIUtils::LayoutDebugger::LogLayoutInfo("Quick Actions Panel", m_quick_actions_panel->GetBounds());
    }
    
    if (validation_passed) {
        std::cout << "UI validation passed successfully" << std::endl;
    } else {
        std::cerr << "UI validation failed - check errors above" << std::endl;
    }
}