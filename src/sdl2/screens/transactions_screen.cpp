// Copyright (c) 2025 The Gotham Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or https://opensource.org/license/mit/.

#include "transactions_screen.h"
#include "../gotham_city_gui.h"
#include "../renderer.h"
#include "../utils/theme.h"
#include "../utils/font_manager.h"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <cstdio>

TransactionsScreen::TransactionsScreen(GothamCityGUI& gui) : Screen(gui)
{
    // Initialize UI systems
    if (auto* theme_manager = m_gui.GetThemeManager()) {
        m_ui_factory = std::make_unique<UIFactory>(*theme_manager);
    }
    m_layout_manager = std::make_unique<LayoutManager>();
}

bool TransactionsScreen::Initialize()
{
    if (m_initialized) {
        return true;
    }

    std::cout << "TransactionsScreen::Initialize() - Starting initialization" << std::endl;
    
    try {
        LoadTransactions();
        std::cout << "TransactionsScreen::Initialize() - LoadTransactions completed" << std::endl;
        
        CreateLayout();
        std::cout << "TransactionsScreen::Initialize() - CreateLayout completed" << std::endl;
        
        SetupButtonCallbacks();
        std::cout << "TransactionsScreen::Initialize() - SetupButtonCallbacks completed" << std::endl;
        
        m_initialized = true;
        std::cout << "Transactions screen initialized successfully" << std::endl;
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Exception in TransactionsScreen::Initialize(): " << e.what() << std::endl;
        return false;
    } catch (...) {
        std::cerr << "Unknown exception in TransactionsScreen::Initialize()" << std::endl;
        return false;
    }
}

void TransactionsScreen::CreateLayout()
{
    CreateHeaderPanel();
    CreateFilterPanel();
    CreateTransactionsPanel();
    CreatePaginationPanel();
}

void TransactionsScreen::CreateHeaderPanel()
{
    if (!m_ui_factory || !m_layout_manager) return;
    
    int width = m_gui.GetRenderer()->GetWidth();
    
    // Create header panel with consistent styling
    Rect header_bounds = Rect(0, 0, width, UIStyleGuide::Dimensions::HEADER_HEIGHT);
    m_header_panel = m_ui_factory->CreatePanel(header_bounds, PanelStyle::HEADER);
    
    // Create header components with factory
    m_title_label = m_ui_factory->CreateLabel("📊 Transaction History", Point(0, 0), LabelStyle::TITLE);
    m_back_button = m_ui_factory->CreateButton("← Back", Rect(0, 0, 100, 40), ButtonStyle::SECONDARY);
    
    // Setup layout for header components
    std::vector<LayoutItem> header_items(2);
    
    // Title item
    header_items[0].constraints.preferred_width = 300;
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

void TransactionsScreen::CreateFilterPanel()
{
    if (!m_ui_factory || !m_layout_manager) return;
    
    int width = m_gui.GetRenderer()->GetWidth();
    int panel_width = std::min(600, width - 2 * UIStyleGuide::Spacing::LG);
    int panel_x = (width - panel_width) / 2;
    
    // Create filter panel with consistent styling
    Rect filter_bounds = Rect(panel_x, 
                             UIStyleGuide::Dimensions::HEADER_HEIGHT + UIStyleGuide::Spacing::MD,
                             panel_width, 
                             60);
    m_filter_panel = m_ui_factory->CreatePanel(filter_bounds, PanelStyle::CARD);
    
    // Create filter buttons with factory
    m_all_filter_button = m_ui_factory->CreateButton("📋 All", Rect(0, 0, 120, UIStyleGuide::Dimensions::BUTTON_HEIGHT), 
                                                     m_current_filter == "all" ? ButtonStyle::PRIMARY : ButtonStyle::SECONDARY);
    m_sent_filter_button = m_ui_factory->CreateButton("📤 Sent", Rect(0, 0, 120, UIStyleGuide::Dimensions::BUTTON_HEIGHT), 
                                                      m_current_filter == "sent" ? ButtonStyle::PRIMARY : ButtonStyle::SECONDARY);
    m_received_filter_button = m_ui_factory->CreateButton("📥 Received", Rect(0, 0, 120, UIStyleGuide::Dimensions::BUTTON_HEIGHT), 
                                                          m_current_filter == "received" ? ButtonStyle::PRIMARY : ButtonStyle::SECONDARY);
    
    m_filter_status_label = m_ui_factory->CreateLabel("Showing all transactions", Point(0, 0), LabelStyle::CAPTION);
    m_filter_status_label->SetColor(UIStyleGuide::Colors::TEXT_SECONDARY);
    
    // Setup layout for filter panel
    std::vector<LayoutItem> filter_items(2);
    
    // Filter buttons (horizontal layout)
    filter_items[0].constraints.preferred_height = UIStyleGuide::Dimensions::BUTTON_HEIGHT;
    filter_items[0].margin = Margin(UIStyleGuide::Spacing::MD);
    filter_items[0].on_bounds_changed = [this](const Rect& bounds) {
        if (m_all_filter_button && m_sent_filter_button && m_received_filter_button) {
            int button_width = (bounds.w - 2 * UIStyleGuide::Spacing::MD) / 3;
            
            Rect all_bounds = bounds;
            all_bounds.w = button_width;
            m_all_filter_button->SetBounds(all_bounds);
            
            Rect sent_bounds = bounds;
            sent_bounds.x += button_width + UIStyleGuide::Spacing::MD;
            sent_bounds.w = button_width;
            m_sent_filter_button->SetBounds(sent_bounds);
            
            Rect received_bounds = bounds;
            received_bounds.x += 2 * (button_width + UIStyleGuide::Spacing::MD);
            received_bounds.w = button_width;
            m_received_filter_button->SetBounds(received_bounds);
        }
    };
    
    // Status label
    filter_items[1].constraints.preferred_height = UIStyleGuide::FontSize::CAPTION + UIStyleGuide::Spacing::SM;
    filter_items[1].margin = Margin(UIStyleGuide::Spacing::SM, UIStyleGuide::Spacing::MD);
    filter_items[1].on_bounds_changed = [this](const Rect& bounds) {
        if (m_filter_status_label) {
            m_filter_status_label->SetPosition(Point(bounds.x, bounds.y));
        }
    };
    
    // Apply vertical layout for filter panel
    m_layout_manager->CreateVerticalLayout(filter_bounds, filter_items, 
                                         UIStyleGuide::Spacing::SM, Alignment::START);
}

void TransactionsScreen::CreateTransactionsPanel()
{
    if (!m_ui_factory || !m_layout_manager) return;
    
    int width = m_gui.GetRenderer()->GetWidth();
    int height = m_gui.GetRenderer()->GetHeight();
    int panel_width = std::min(800, width - 2 * UIStyleGuide::Spacing::LG);
    int panel_x = (width - panel_width) / 2;
    
    // Create transactions panel with consistent styling
    Rect transactions_bounds = Rect(panel_x, 
                                   UIStyleGuide::Dimensions::HEADER_HEIGHT + 60 + 2 * UIStyleGuide::Spacing::MD,
                                   panel_width, 
                                   height - (UIStyleGuide::Dimensions::HEADER_HEIGHT + 60 + 80 + 4 * UIStyleGuide::Spacing::MD));
    m_transactions_panel = m_ui_factory->CreatePanel(transactions_bounds, PanelStyle::MAIN);
}

void TransactionsScreen::CreatePaginationPanel()
{
    if (!m_ui_factory || !m_layout_manager) return;
    
    int width = m_gui.GetRenderer()->GetWidth();
    int height = m_gui.GetRenderer()->GetHeight();
    int panel_width = std::min(600, width - 2 * UIStyleGuide::Spacing::LG);
    int panel_x = (width - panel_width) / 2;
    
    // Create pagination panel with consistent styling
    Rect pagination_bounds = Rect(panel_x, 
                                 height - 80 - UIStyleGuide::Spacing::MD,
                                 panel_width, 
                                 60);
    m_pagination_panel = m_ui_factory->CreatePanel(pagination_bounds, PanelStyle::STATUS);
}

void TransactionsScreen::UpdateTransactionLabels()
{
    // Clear existing transaction labels
    m_transaction_labels.clear();

    // Get filtered transactions for current page
    auto filtered_transactions = GetFilteredTransactions();
    int start_index = m_current_page * m_transactions_per_page;
    int end_index = std::min(start_index + m_transactions_per_page, (int)filtered_transactions.size());

    int y_pos = 200;
    for (int i = start_index; i < end_index; ++i) {
        const auto& tx = filtered_transactions[i];
        
        // Format transaction text
        std::string addr_short = tx.address.length() > 12 ? tx.address.substr(0, 12) + "..." : tx.address;
        char amount_str[32];
        snprintf(amount_str, sizeof(amount_str), "%.8f", tx.amount);
        
        std::string tx_text = (tx.is_incoming ? "+" : "-") + std::string(amount_str) + " BTC " +
                             (tx.is_incoming ? "from " : "to ") + addr_short + 
                             " (" + std::to_string(tx.confirmations) + " conf) - " + tx.date;

        // Create label for this transaction
        auto tx_label = std::make_unique<Label>(tx_text, Point(50, y_pos));
        Color tx_color = tx.is_incoming ? Color(46, 160, 67, 255) : Color(220, 53, 69, 255);
        tx_label->SetColor(tx_color);
        
        m_transaction_labels.push_back(std::move(tx_label));
        y_pos += 30;
    }

    // Update page label
    auto filtered_txs = GetFilteredTransactions();
    int max_pages = std::max(1, (int)((filtered_txs.size() + m_transactions_per_page - 1) / m_transactions_per_page));
    std::string page_text = std::to_string(m_current_page + 1) + " / " + std::to_string(max_pages);
    if (m_page_label) {
        m_page_label->SetText(page_text);
    }
}

void TransactionsScreen::HandleEvent(const SDL_Event& event)
{
    if (event.type == SDL_KEYDOWN) {
        switch (event.key.keysym.sym) {
            case SDLK_ESCAPE:
                m_gui.SwitchScreen(ScreenType::WALLET);
                break;
            case SDLK_1:
                m_current_filter = "all";
                m_current_page = 0;
                UpdateTransactionLabels();
                std::cout << "Filter: All transactions" << std::endl;
                break;
            case SDLK_2:
                m_current_filter = "sent";
                m_current_page = 0;
                UpdateTransactionLabels();
                std::cout << "Filter: Sent transactions" << std::endl;
                break;
            case SDLK_3:
                m_current_filter = "received";
                m_current_page = 0;
                UpdateTransactionLabels();
                std::cout << "Filter: Received transactions" << std::endl;
                break;
            case SDLK_LEFT:
                if (m_current_page > 0) {
                    m_current_page--;
                    UpdateTransactionLabels();
                    std::cout << "Previous page: " << (m_current_page + 1) << std::endl;
                }
                break;
            case SDLK_RIGHT:
                {
                    auto filtered_transactions = GetFilteredTransactions();
                    if (m_transactions_per_page > 0 && !filtered_transactions.empty()) {
                        int max_pages = (filtered_transactions.size() + m_transactions_per_page - 1) / m_transactions_per_page;
                        
                        if (m_current_page < max_pages - 1) {
                            m_current_page++;
                            UpdateTransactionLabels();
                            std::cout << "Next page: " << (m_current_page + 1) << std::endl;
                        }
                    }
                }
                break;
        }
    }
    
    // Pass events to UI components
    if (m_back_button) m_back_button->HandleEvent(event);
    if (m_all_filter_button) m_all_filter_button->HandleEvent(event);
    if (m_sent_filter_button) m_sent_filter_button->HandleEvent(event);
    if (m_received_filter_button) m_received_filter_button->HandleEvent(event);
}

void TransactionsScreen::Update(float delta_time)
{
    m_elapsed_time += delta_time;
}

void TransactionsScreen::Render(Renderer& renderer)
{
    // Render background with consistent theme
    renderer.Clear(UIStyleGuide::Colors::BACKGROUND_DARK);
    
    // Render UI panels
    if (m_header_panel) m_header_panel->Render(renderer);
    if (m_filter_panel) m_filter_panel->Render(renderer);
    if (m_transactions_panel) m_transactions_panel->Render(renderer);
    if (m_pagination_panel) m_pagination_panel->Render(renderer);
    
    // Get fonts from font manager
    FontManager* font_manager = m_gui.GetFontManager();
    if (font_manager) {
        TTF_Font* title_font = font_manager->GetDefaultFont(24);
        TTF_Font* body_font = font_manager->GetDefaultFont(16);
        TTF_Font* small_font = font_manager->GetDefaultFont(14);
        
        // Render header components
        if (m_title_label) m_title_label->Render(renderer, title_font);
        if (m_back_button) m_back_button->Render(renderer, body_font);
        
        // Render filter components
        if (m_all_filter_button) m_all_filter_button->Render(renderer, body_font);
        if (m_sent_filter_button) m_sent_filter_button->Render(renderer, body_font);
        if (m_received_filter_button) m_received_filter_button->Render(renderer, body_font);
        if (m_filter_status_label) m_filter_status_label->Render(renderer, small_font);
        
        // Render transaction list
        for (const auto& tx_label : m_transaction_labels) {
            if (tx_label) tx_label->Render(renderer, body_font);
        }
        
        // Render pagination components
        if (m_prev_button) m_prev_button->Render(renderer, body_font);
        if (m_next_button) m_next_button->Render(renderer, body_font);
        if (m_page_label) m_page_label->Render(renderer, body_font);
        if (m_instructions_label) m_instructions_label->Render(renderer, small_font);
    }
}

void TransactionsScreen::OnActivate()
{
    std::cout << "Transactions screen activated" << std::endl;
    UpdateTransactionLabels();
}

void TransactionsScreen::OnResize(int new_width, int new_height)
{
    CreateLayout();
}

void TransactionsScreen::LoadTransactions()
{
    std::cout << "TransactionsScreen::LoadTransactions() - Starting" << std::endl;
    
    m_transactions.clear();
    
    // Add some test transactions
    TransactionDisplay tx1;
    tx1.amount = 0.001;
    tx1.address = "1A1zP1eP5QGefi2DMPTfTL5SLmv7DivfNa";
    tx1.confirmations = 6;
    tx1.date = "2025-01-01 12:00";
    tx1.is_incoming = true;
    tx1.label = "Received Bitcoin";
    m_transactions.push_back(tx1);
    
    TransactionDisplay tx2;
    tx2.amount = 0.0005;
    tx2.address = "3J98t1WpEZ73CNmQviecrnyiWrnqRhWNLy";
    tx2.confirmations = 3;
    tx2.date = "2025-01-01 10:30";
    tx2.is_incoming = false;
    tx2.label = "Sent Bitcoin";
    m_transactions.push_back(tx2);
    
    std::cout << "TransactionsScreen::LoadTransactions() - Added " << m_transactions.size() << " transactions" << std::endl;
}

void TransactionsScreen::SetupButtonCallbacks()
{
    if (m_back_button) {
        m_back_button->SetOnClick([this]() { 
            OnBackClicked();
        });
    }
    
    if (m_all_filter_button) {
        m_all_filter_button->SetOnClick([this]() {
            OnAllFilterClicked();
        });
    }
    
    if (m_sent_filter_button) {
        m_sent_filter_button->SetOnClick([this]() {
            OnSentFilterClicked();
        });
    }
    
    if (m_received_filter_button) {
        m_received_filter_button->SetOnClick([this]() {
            OnReceivedFilterClicked();
        });
    }
}

std::vector<TransactionDisplay> TransactionsScreen::GetFilteredTransactions() const
{
    std::vector<TransactionDisplay> filtered;
    
    for (const auto& tx : m_transactions) {
        if (m_current_filter == "all" ||
            (m_current_filter == "sent" && !tx.is_incoming) ||
            (m_current_filter == "received" && tx.is_incoming)) {
            filtered.push_back(tx);
        }
    }
    
    return filtered;
}

void TransactionsScreen::OnBackClicked()
{
    // Context-aware navigation: go back to where we came from
    ScreenType previous = m_gui.GetPreviousScreen();
    if (previous == ScreenType::MAIN) {
        m_gui.SwitchScreen(ScreenType::MAIN);
    } else {
        m_gui.SwitchScreen(ScreenType::WALLET);
    }
}

void TransactionsScreen::OnAllFilterClicked()
{
    m_current_filter = "all";
    m_current_page = 0;
    FilterTransactions();
    UpdateTransactionDisplay();
}

void TransactionsScreen::OnSentFilterClicked()
{
    m_current_filter = "sent";
    m_current_page = 0;
    FilterTransactions();
    UpdateTransactionDisplay();
}

void TransactionsScreen::OnReceivedFilterClicked()
{
    m_current_filter = "received";
    m_current_page = 0;
    FilterTransactions();
    UpdateTransactionDisplay();
}

void TransactionsScreen::OnPrevPageClicked()
{
    if (m_current_page > 0) {
        m_current_page--;
        UpdateTransactionDisplay();
    }
}

void TransactionsScreen::OnNextPageClicked()
{
    auto filtered = GetFilteredTransactions();
    int total_pages = (filtered.size() + m_transactions_per_page - 1) / m_transactions_per_page;
    if (m_current_page < total_pages - 1) {
        m_current_page++;
        UpdateTransactionDisplay();
    }
}

void TransactionsScreen::FilterTransactions()
{
    // This method filters the transactions based on current filter
    // The actual filtering is done in GetFilteredTransactions()
    // This method can be used for additional filtering logic if needed
    std::cout << "Filtering transactions with filter: " << m_current_filter << std::endl;
}

void TransactionsScreen::UpdateTransactionDisplay()
{
    // Update the transaction labels based on current page and filter
    auto filtered = GetFilteredTransactions();
    
    // Clear existing transaction labels
    m_transaction_labels.clear();
    
    // Calculate which transactions to show on current page
    int start_index = m_current_page * m_transactions_per_page;
    int end_index = std::min(start_index + m_transactions_per_page, static_cast<int>(filtered.size()));
    
    // Update page label
    int total_pages = (filtered.size() + m_transactions_per_page - 1) / m_transactions_per_page;
    if (m_page_label) {
        std::string page_text = "Page " + std::to_string(m_current_page + 1) + " of " + std::to_string(total_pages);
        m_page_label->SetText(page_text);
    }
    
    // Create labels for visible transactions
    if (m_ui_factory) {
        int y_offset = 150; // Starting Y position for transaction list
        for (int i = start_index; i < end_index; i++) {
            const auto& tx = filtered[i];
            
            // Create transaction display string
            std::string tx_text = (tx.is_incoming ? "↓ +" : "↑ -") + 
                                 std::to_string(tx.amount) + " BTC - " + 
                                 tx.date + " (" + std::to_string(tx.confirmations) + " conf)";
            
            auto tx_label = m_ui_factory->CreateLabel(tx_text, Point(50, y_offset), LabelStyle::BODY);
            tx_label->SetColor(tx.is_incoming ? UIStyleGuide::Colors::SUCCESS : UIStyleGuide::Colors::WARNING);
            
            m_transaction_labels.push_back(std::move(tx_label));
            y_offset += 30;
        }
    }
    
    std::cout << "Updated transaction display: showing " << (end_index - start_index) 
              << " transactions on page " << (m_current_page + 1) << std::endl;
}

