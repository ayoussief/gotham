// Copyright (c) 2025 The Gotham Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or https://opensource.org/license/mit/.

#include "transactions_screen.h"
#include "../gotham_city_gui.h"
#include "../utils/theme.h"
#include "../utils/font_manager.h"
#include "../wallet/wallet_manager.h"
#include <iostream>

TransactionsScreen::TransactionsScreen(GothamCityGUI& gui) : Screen(gui)
{
    if (auto* theme_manager = m_gui.GetThemeManager()) {
        m_ui_factory = std::make_unique<UIFactory>(*theme_manager);
    }
    m_layout_manager = std::make_unique<LayoutManager>();
}

bool TransactionsScreen::Initialize()
{
    if (m_initialized) return true;
    
    CreateContentPanel();
    SetupButtonCallbacks();
    
    m_initialized = true;
    std::cout << "Transactions screen initialized (content area only)" << std::endl;
    return true;
}

void TransactionsScreen::CreateContentPanel()
{
    if (!m_ui_factory) return;
    
    m_content_panel = m_ui_factory->CreatePanel(Rect(0, 0, 800, 600), PanelStyle::CONTENT);
    CreateFilterPanel();
    CreateListPanel();
    CreateSummaryPanel();
    CreatePaginationPanel();
}

void TransactionsScreen::CreateFilterPanel()
{
    m_filter_panel = m_ui_factory->CreatePanel(Rect(20, 20, 500, 60), PanelStyle::CARD);
    m_filter_title_label = m_ui_factory->CreateLabel("Filter Transactions", Point(30, 30), LabelStyle::SUBHEADING);
    m_all_button = m_ui_factory->CreateButton("📋 All", Rect(30, 50, 80, 30), ButtonStyle::PRIMARY);
    m_sent_button = m_ui_factory->CreateButton("📤 Sent", Rect(120, 50, 80, 30), ButtonStyle::SECONDARY);
    m_received_button = m_ui_factory->CreateButton("📥 Received", Rect(210, 50, 80, 30), ButtonStyle::SECONDARY);
    m_export_button = m_ui_factory->CreateButton("💾 Export", Rect(400, 50, 80, 30), ButtonStyle::GHOST);
}

void TransactionsScreen::CreateListPanel()
{
    m_list_panel = m_ui_factory->CreatePanel(Rect(20, 100, 500, 300), PanelStyle::CARD);
    m_list_title_label = m_ui_factory->CreateLabel("Transaction History", Point(30, 110), LabelStyle::SUBHEADING);
    
    // Create transaction display labels
    for (int i = 0; i < 10; ++i) {
        auto tx_label = m_ui_factory->CreateLabel("No transactions", Point(30, 140 + i * 25), LabelStyle::CAPTION);
        m_transaction_labels.push_back(std::move(tx_label));
    }
}

void TransactionsScreen::CreateSummaryPanel()
{
    m_summary_panel = m_ui_factory->CreatePanel(Rect(20, 420, 250, 100), PanelStyle::INFO);
    m_summary_title_label = m_ui_factory->CreateLabel("Summary", Point(30, 430), LabelStyle::SUBHEADING);
    m_total_sent_label = m_ui_factory->CreateLabel("Total Sent: 0.00000000 BTC", Point(30, 450), LabelStyle::CAPTION);
    m_total_received_label = m_ui_factory->CreateLabel("Total Received: 0.00000000 BTC", Point(30, 470), LabelStyle::CAPTION);
    m_transaction_count_label = m_ui_factory->CreateLabel("Transactions: 0", Point(30, 490), LabelStyle::CAPTION);
}

void TransactionsScreen::CreatePaginationPanel()
{
    m_pagination_panel = m_ui_factory->CreatePanel(Rect(290, 420, 230, 100), PanelStyle::CARD);
    m_prev_page_button = m_ui_factory->CreateButton("◀ Previous", Rect(300, 430, 80, 30), ButtonStyle::SECONDARY);
    m_next_page_button = m_ui_factory->CreateButton("Next ▶", Rect(390, 430, 80, 30), ButtonStyle::SECONDARY);
    m_page_info_label = m_ui_factory->CreateLabel("Page 1 of 1", Point(320, 470), LabelStyle::CAPTION);
}

void TransactionsScreen::SetupButtonCallbacks()
{
    if (m_all_button) {
        m_all_button->SetOnClick([this]() { OnAllFilterClicked(); });
    }
    if (m_sent_button) {
        m_sent_button->SetOnClick([this]() { OnSentFilterClicked(); });
    }
    if (m_received_button) {
        m_received_button->SetOnClick([this]() { OnReceivedFilterClicked(); });
    }
    if (m_export_button) {
        m_export_button->SetOnClick([this]() { OnExportClicked(); });
    }
    if (m_prev_page_button) {
        m_prev_page_button->SetOnClick([this]() { OnPrevPageClicked(); });
    }
    if (m_next_page_button) {
        m_next_page_button->SetOnClick([this]() { OnNextPageClicked(); });
    }
}

void TransactionsScreen::HandleEvent(const SDL_Event& event)
{
    if (m_content_panel) m_content_panel->HandleEvent(event);
    if (m_all_button) m_all_button->HandleEvent(event);
    if (m_sent_button) m_sent_button->HandleEvent(event);
    if (m_received_button) m_received_button->HandleEvent(event);
    if (m_export_button) m_export_button->HandleEvent(event);
    if (m_prev_page_button) m_prev_page_button->HandleEvent(event);
    if (m_next_page_button) m_next_page_button->HandleEvent(event);
}

void TransactionsScreen::Update(float delta_time)
{
    m_elapsed_time += delta_time;
    if (m_all_button) m_all_button->Update(delta_time);
    if (m_sent_button) m_sent_button->Update(delta_time);
    if (m_received_button) m_received_button->Update(delta_time);
    if (m_export_button) m_export_button->Update(delta_time);
    if (m_prev_page_button) m_prev_page_button->Update(delta_time);
    if (m_next_page_button) m_next_page_button->Update(delta_time);
}

void TransactionsScreen::Render(Renderer& renderer)
{
    if (m_content_panel) m_content_panel->Render(renderer);
    if (m_filter_panel) m_filter_panel->Render(renderer);
    if (m_list_panel) m_list_panel->Render(renderer);
    if (m_summary_panel) m_summary_panel->Render(renderer);
    if (m_pagination_panel) m_pagination_panel->Render(renderer);
    
    TTF_Font* subheading_font = m_gui.GetFontManager()->GetDefaultFont(UIStyleGuide::FontSize::SUBHEADING);
    TTF_Font* body_font = m_gui.GetFontManager()->GetDefaultFont(UIStyleGuide::FontSize::BODY);
    TTF_Font* caption_font = m_gui.GetFontManager()->GetDefaultFont(UIStyleGuide::FontSize::CAPTION);
    
    if (m_filter_title_label && subheading_font) m_filter_title_label->Render(renderer, subheading_font);
    if (m_all_button && body_font) m_all_button->Render(renderer, body_font);
    if (m_sent_button && body_font) m_sent_button->Render(renderer, body_font);
    if (m_received_button && body_font) m_received_button->Render(renderer, body_font);
    if (m_export_button && caption_font) m_export_button->Render(renderer, caption_font);
    
    if (m_list_title_label && subheading_font) m_list_title_label->Render(renderer, subheading_font);
    for (auto& tx_label : m_transaction_labels) {
        if (tx_label && caption_font) tx_label->Render(renderer, caption_font);
    }
    
    if (m_summary_title_label && subheading_font) m_summary_title_label->Render(renderer, subheading_font);
    if (m_total_sent_label && caption_font) m_total_sent_label->Render(renderer, caption_font);
    if (m_total_received_label && caption_font) m_total_received_label->Render(renderer, caption_font);
    if (m_transaction_count_label && caption_font) m_transaction_count_label->Render(renderer, caption_font);
    
    if (m_prev_page_button && body_font) m_prev_page_button->Render(renderer, body_font);
    if (m_next_page_button && body_font) m_next_page_button->Render(renderer, body_font);
    if (m_page_info_label && caption_font) m_page_info_label->Render(renderer, caption_font);
}

void TransactionsScreen::OnActivate()
{
    std::cout << "Transactions screen activated" << std::endl;
    RefreshTransactionList();
}

void TransactionsScreen::OnResize(int new_width, int new_height)
{
    RepositionElements(new_width, new_height);
}

void TransactionsScreen::OnNavigatedTo(const NavigationContext& context)
{
    std::cout << "Navigated to Transactions screen" << std::endl;
    RefreshTransactionList();
    UpdateSummary();
}

void TransactionsScreen::OnNavigatedFrom()
{
    std::cout << "Navigated away from Transactions screen" << std::endl;
}

void TransactionsScreen::RefreshTransactionList()
{
    // Mock transaction data
    if (m_transaction_labels.size() >= 5) {
        m_transaction_labels[0]->SetText("📥 Received 0.001 BTC - 2 hours ago");
        m_transaction_labels[1]->SetText("📤 Sent 0.005 BTC - 1 day ago");
        m_transaction_labels[2]->SetText("📥 Received 0.002 BTC - 3 days ago");
        m_transaction_labels[3]->SetText("📤 Sent 0.001 BTC - 1 week ago");
        m_transaction_labels[4]->SetText("📥 Received 0.01 BTC - 2 weeks ago");
    }
}

void TransactionsScreen::ApplyFilter()
{
    // Filter logic would be implemented here
    RefreshTransactionList();
}

void TransactionsScreen::UpdateSummary()
{
    // Summary calculation would be implemented here
}

void TransactionsScreen::UpdatePagination()
{
    // Pagination update would be implemented here
}

void TransactionsScreen::RepositionElements(int content_width, int content_height)
{
    // Responsive repositioning would be implemented here
}

void TransactionsScreen::OnAllFilterClicked()
{
    std::cout << "All filter clicked" << std::endl;
    m_current_filter = FilterType::ALL;
    ApplyFilter();
}

void TransactionsScreen::OnSentFilterClicked()
{
    std::cout << "Sent filter clicked" << std::endl;
    m_current_filter = FilterType::SENT;
    ApplyFilter();
}

void TransactionsScreen::OnReceivedFilterClicked()
{
    std::cout << "Received filter clicked" << std::endl;
    m_current_filter = FilterType::RECEIVED;
    ApplyFilter();
}

void TransactionsScreen::OnExportClicked()
{
    std::cout << "Export transactions clicked" << std::endl;
}

void TransactionsScreen::OnPrevPageClicked()
{
    std::cout << "Previous page clicked" << std::endl;
    if (m_current_page > 0) {
        m_current_page--;
        RefreshTransactionList();
    }
}

void TransactionsScreen::OnNextPageClicked()
{
    std::cout << "Next page clicked" << std::endl;
    m_current_page++;
    RefreshTransactionList();
}

void TransactionsScreen::OnSearchChanged()
{
    std::cout << "Search changed" << std::endl;
    ApplyFilter();
}