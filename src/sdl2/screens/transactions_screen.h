// Copyright (c) 2025 The Gotham Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or https://opensource.org/license/mit/.

#ifndef GOTHAM_SDL2_SCREENS_TRANSACTIONS_SCREEN_H
#define GOTHAM_SDL2_SCREENS_TRANSACTIONS_SCREEN_H

#include "../ui/screen.h"
#include "../ui/button.h"
#include "../ui/label.h"
#include "../ui/panel.h"
#include "../ui/text_input.h"
#include "../ui/ui_factory.h"
#include "../ui/layout_manager.h"
#include "../ui/ui_style_guide.h"
#include "../ui/navigation_manager.h"
#include <memory>
#include <vector>

/**
 * Transaction history screen - Content Area Only
 * Shows complete transaction history with filtering
 */
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
    
    // Navigation lifecycle methods
    void OnNavigatedTo(const NavigationContext& context) override;
    void OnNavigatedFrom() override;

private:
    // UI Systems
    std::unique_ptr<UIFactory> m_ui_factory;
    std::unique_ptr<LayoutManager> m_layout_manager;
    
    // Content area components only
    std::unique_ptr<Panel> m_content_panel;
    
    // Filter panel
    std::unique_ptr<Panel> m_filter_panel;
    std::unique_ptr<Label> m_filter_title_label;
    std::unique_ptr<Button> m_all_button;
    std::unique_ptr<Button> m_sent_button;
    std::unique_ptr<Button> m_received_button;
    std::unique_ptr<TextInput> m_search_input;
    std::unique_ptr<Button> m_export_button;
    
    // Transaction list panel
    std::unique_ptr<Panel> m_list_panel;
    std::unique_ptr<Label> m_list_title_label;
    std::vector<std::unique_ptr<Panel>> m_transaction_panels;
    std::vector<std::unique_ptr<Label>> m_transaction_labels;
    
    // Summary panel
    std::unique_ptr<Panel> m_summary_panel;
    std::unique_ptr<Label> m_summary_title_label;
    std::unique_ptr<Label> m_total_sent_label;
    std::unique_ptr<Label> m_total_received_label;
    std::unique_ptr<Label> m_transaction_count_label;
    
    // Pagination
    std::unique_ptr<Panel> m_pagination_panel;
    std::unique_ptr<Button> m_prev_page_button;
    std::unique_ptr<Button> m_next_page_button;
    std::unique_ptr<Label> m_page_info_label;
    
    enum class FilterType {
        ALL,
        SENT,
        RECEIVED
    };
    
    FilterType m_current_filter{FilterType::ALL};
    std::string m_search_term;
    int m_current_page{0};
    int m_transactions_per_page{10};
    float m_elapsed_time{0.0f};
    
    // Content area methods
    void CreateContentPanel();
    void CreateFilterPanel();
    void CreateListPanel();
    void CreateSummaryPanel();
    void CreatePaginationPanel();
    void SetupButtonCallbacks();
    void RefreshTransactionList();
    void ApplyFilter();
    void UpdateSummary();
    void UpdatePagination();
    void RepositionElements(int content_width, int content_height);
    
    // Action callbacks
    void OnAllFilterClicked();
    void OnSentFilterClicked();
    void OnReceivedFilterClicked();
    void OnExportClicked();
    void OnPrevPageClicked();
    void OnNextPageClicked();
    void OnSearchChanged();
};

#endif // GOTHAM_SDL2_SCREENS_TRANSACTIONS_SCREEN_H