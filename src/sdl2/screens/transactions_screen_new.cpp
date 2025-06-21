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
    // Get current screen dimensions
    int screen_width = m_gui.GetRenderer()->GetWidth();
    int screen_height = m_gui.GetRenderer()->GetHeight();
    int center_x = screen_width / 2;

    // Create header components
    if (!m_title_label) {
        m_title_label = std::make_unique<Label>("Transaction History", Point(50, 30));
        m_title_label->SetColor(Color(255, 215, 0, 255)); // Gotham gold
    }

    if (!m_back_button) {
        m_back_button = std::make_unique<Button>("← Back (ESC)", Point(screen_width - 150, 25), Size(120, 30));
        m_back_button->SetBackgroundColor(Color(60, 60, 70, 255));
        m_back_button->SetTextColor(Color(180, 180, 180, 255));
    }

    // Create filter buttons
    int filter_y = 100;
    if (!m_all_filter_button) {
        m_all_filter_button = std::make_unique<Button>("1 - All", Point(center_x - 150, filter_y), Size(90, 35));
        m_all_filter_button->SetBackgroundColor(m_current_filter == "all" ? Color(0, 123, 255, 255) : Color(50, 50, 60, 255));
        m_all_filter_button->SetTextColor(Color(255, 255, 255, 255));
    }

    if (!m_sent_filter_button) {
        m_sent_filter_button = std::make_unique<Button>("2 - Sent", Point(center_x - 45, filter_y), Size(90, 35));
        m_sent_filter_button->SetBackgroundColor(m_current_filter == "sent" ? Color(220, 53, 69, 255) : Color(50, 50, 60, 255));
        m_sent_filter_button->SetTextColor(Color(255, 255, 255, 255));
    }

    if (!m_received_filter_button) {
        m_received_filter_button = std::make_unique<Button>("3 - Received", Point(center_x + 60, filter_y), Size(90, 35));
        m_received_filter_button->SetBackgroundColor(m_current_filter == "received" ? Color(46, 160, 67, 255) : Color(50, 50, 60, 255));
        m_received_filter_button->SetTextColor(Color(255, 255, 255, 255));
    }

    // Create transactions header
    if (!m_transactions_header_label) {
        m_transactions_header_label = std::make_unique<Label>("Recent Transactions", Point(50, 170));
        m_transactions_header_label->SetColor(Color(255, 215, 0, 255));
    }

    // Create pagination controls
    int pagination_y = screen_height - 60;
    if (!m_prev_button) {
        m_prev_button = std::make_unique<Button>("← Prev", Point(center_x - 100, pagination_y), Size(70, 30));
        m_prev_button->SetBackgroundColor(Color(0, 123, 255, 255));
        m_prev_button->SetTextColor(Color(255, 255, 255, 255));
    }

    if (!m_next_button) {
        m_next_button = std::make_unique<Button>("Next →", Point(center_x + 30, pagination_y), Size(70, 30));
        m_next_button->SetBackgroundColor(Color(0, 123, 255, 255));
        m_next_button->SetTextColor(Color(255, 255, 255, 255));
    }

    if (!m_page_label) {
        m_page_label = std::make_unique<Label>("1 / 1", Point(center_x - 15, pagination_y + 8));
        m_page_label->SetColor(Color(255, 215, 0, 255));
        m_page_label->SetAlignment(TextAlignment::CENTER);
    }

    // Create instructions
    if (!m_instructions_label) {
        m_instructions_label = std::make_unique<Label>("Controls: ESC=Back, 1/2/3=Filter, ←/→=Pages", Point(30, screen_height - 15));
        m_instructions_label->SetColor(Color(120, 120, 120, 255));
    }

    // Update transaction labels
    UpdateTransactionLabels();
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
}

void TransactionsScreen::Update(float delta_time)
{
    m_elapsed_time += delta_time;
}

void TransactionsScreen::Render(Renderer& renderer)
{
    // Clear with dark background
    renderer.Clear(Color(15, 15, 20, 255));
    
    // Draw header background
    renderer.SetDrawColor(Color(30, 30, 40, 255));
    renderer.FillRect(Rect(0, 0, renderer.GetWidth(), 70));
    renderer.SetDrawColor(Color(255, 215, 0, 255));
    renderer.FillRect(Rect(0, 67, renderer.GetWidth(), 3));
    
    // Draw filter panel background
    int center_x = renderer.GetWidth() / 2;
    renderer.SetDrawColor(Color(35, 35, 45, 255));
    renderer.FillRect(Rect(center_x - 170, 85, 340, 55));
    renderer.SetDrawColor(Color(70, 70, 80, 255));
    renderer.DrawRect(Rect(center_x - 170, 85, 340, 55));
    
    // Draw transactions panel background
    renderer.SetDrawColor(Color(25, 25, 35, 255));
    renderer.FillRect(Rect(30, 155, renderer.GetWidth() - 60, renderer.GetHeight() - 230));
    renderer.SetDrawColor(Color(70, 70, 80, 255));
    renderer.DrawRect(Rect(30, 155, renderer.GetWidth() - 60, renderer.GetHeight() - 230));
    
    // Render UI components with proper fonts
    FontManager* font_manager = m_gui.GetFontManager();
    if (font_manager) {
        TTF_Font* title_font = font_manager->GetDefaultFont(24);
        TTF_Font* button_font = font_manager->GetDefaultFont(16);
        TTF_Font* body_font = font_manager->GetDefaultFont(14);
        TTF_Font* small_font = font_manager->GetDefaultFont(12);
        
        // Header components
        if (m_title_label && title_font) {
            m_title_label->Render(renderer, title_font);
        }
        if (m_back_button && button_font) {
            m_back_button->Render(renderer, button_font);
        }
        
        // Filter components
        if (m_all_filter_button && button_font) {
            m_all_filter_button->Render(renderer, button_font);
        }
        if (m_sent_filter_button && button_font) {
            m_sent_filter_button->Render(renderer, button_font);
        }
        if (m_received_filter_button && button_font) {
            m_received_filter_button->Render(renderer, button_font);
        }
        
        // Transaction list components
        if (m_transactions_header_label && body_font) {
            m_transactions_header_label->Render(renderer, body_font);
        }
        
        // Render transaction labels
        for (auto& label : m_transaction_labels) {
            if (label && body_font) {
                label->Render(renderer, body_font);
            }
        }
        
        // Pagination components
        if (m_prev_button && button_font) {
            m_prev_button->Render(renderer, button_font);
        }
        if (m_next_button && button_font) {
            m_next_button->Render(renderer, button_font);
        }
        if (m_page_label && body_font) {
            m_page_label->Render(renderer, body_font);
        }
        if (m_instructions_label && small_font) {
            m_instructions_label->Render(renderer, small_font);
        }
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
    // Button callbacks would be set up here if needed
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

