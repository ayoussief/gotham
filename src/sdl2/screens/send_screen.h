// Copyright (c) 2025 The Gotham Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or https://opensource.org/license/mit/.

#ifndef GOTHAM_SDL2_SCREENS_SEND_SCREEN_H
#define GOTHAM_SDL2_SCREENS_SEND_SCREEN_H

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

/**
 * Send Bitcoin screen - Content Area Only
 * Form for sending Bitcoin transactions
 */
class SendScreen : public Screen
{
public:
    explicit SendScreen(GothamCityGUI& gui);
    bool Initialize() override;
    void HandleEvent(const SDL_Event& event) override;
    void Update(float delta_time) override;
    void Render(Renderer& renderer) override;
    void OnActivate() override;
    void OnResize(int new_width, int new_height) override;
    void SetContentAreaBounds(const Rect& bounds) override;
    
    // Navigation lifecycle methods
    void OnNavigatedTo(const NavigationContext& context) override;
    void OnNavigatedFrom() override;

private:
    // UI Systems
    std::unique_ptr<UIFactory> m_ui_factory;
    std::unique_ptr<LayoutManager> m_layout_manager;
    
    // Content area components only
    std::unique_ptr<Panel> m_content_panel;
    
    // Send form panel
    std::unique_ptr<Panel> m_form_panel;
    std::unique_ptr<Label> m_form_title_label;
    std::unique_ptr<Label> m_recipient_label;
    std::unique_ptr<TextInput> m_recipient_input;
    std::unique_ptr<Label> m_amount_label;
    std::unique_ptr<TextInput> m_amount_input;
    std::unique_ptr<Label> m_fee_label;
    std::unique_ptr<TextInput> m_fee_input;
    std::unique_ptr<Label> m_description_label;
    std::unique_ptr<TextInput> m_description_input;
    
    // Transaction preview panel
    std::unique_ptr<Panel> m_preview_panel;
    std::unique_ptr<Label> m_preview_title_label;
    std::unique_ptr<Label> m_preview_recipient_label;
    std::unique_ptr<Label> m_preview_amount_label;
    std::unique_ptr<Label> m_preview_fee_label;
    std::unique_ptr<Label> m_preview_total_label;
    
    // Action buttons panel (like main screen)
    std::unique_ptr<Panel> m_actions_panel;
    std::unique_ptr<Button> m_send_button;
    std::unique_ptr<Button> m_clear_button;
    std::unique_ptr<Button> m_scan_qr_button;
    
    // Balance info panel
    std::unique_ptr<Panel> m_balance_panel;
    std::unique_ptr<Label> m_available_balance_label;
    std::unique_ptr<Label> m_after_send_balance_label;
    
    // Form state
    struct FormState {
        std::string recipient;
        std::string amount;
        std::string fee;
        std::string description;
    } m_saved_form_state;
    
    float m_elapsed_time{0.0f};
    bool m_form_valid{false};
    
    // Content area bounds (set by persistent layout)
    Rect m_content_area_bounds{0, 0, 800, 600};
    
    // Content area methods
    void CreateContentPanel();
    void CreateFormPanel();
    void CreateActionsPanel();
    void CreatePreviewPanel();
    void CreateBalancePanel();
    void SetupButtonCallbacks();
    void ValidateForm();
    void UpdatePreview();
    void UpdateBalanceInfo();
    void ClearForm();
    void SaveFormState();
    void RestoreFormState();
    void RepositionElements(int content_width, int content_height);
    
    // Action callbacks
    void OnSendClicked();
    void OnClearClicked();
    void OnScanQRClicked();
    
    // Form validation
    bool IsValidBitcoinAddress(const std::string& address);
    bool IsValidAmount(const std::string& amount);
    bool IsValidFee(const std::string& fee);
    double GetAmountValue();
    double GetFeeValue();
};

#endif // GOTHAM_SDL2_SCREENS_SEND_SCREEN_H