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
#include <memory>

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

private:
    // UI Components
    std::unique_ptr<Panel> m_header_panel;
    std::unique_ptr<Panel> m_form_panel;
    std::unique_ptr<Panel> m_preview_panel;
    
    // Header
    std::unique_ptr<Label> m_title_label;
    std::unique_ptr<Button> m_back_button;
    
    // Form fields
    std::unique_ptr<Label> m_address_label;
    std::unique_ptr<TextInput> m_address_input;
    std::unique_ptr<Label> m_amount_label;
    std::unique_ptr<TextInput> m_amount_input;
    std::unique_ptr<Label> m_fee_label;
    std::unique_ptr<TextInput> m_fee_input;
    std::unique_ptr<Label> m_label_label;
    std::unique_ptr<TextInput> m_label_input;
    
    // Preview section
    std::unique_ptr<Label> m_preview_title_label;
    std::unique_ptr<Label> m_preview_address_label;
    std::unique_ptr<Label> m_preview_amount_label;
    std::unique_ptr<Label> m_preview_fee_label;
    std::unique_ptr<Label> m_preview_total_label;
    
    // Action buttons
    std::unique_ptr<Button> m_send_button;
    std::unique_ptr<Button> m_clear_button;
    
    // Status
    std::unique_ptr<Label> m_status_label;
    
    float m_elapsed_time{0.0f};
    bool m_transaction_pending{false};
    
    void CreateLayout();
    void CreateHeaderPanel();
    void CreateFormPanel();
    void CreatePreviewPanel();
    void UpdatePreview();
    void SetupButtonCallbacks();
    void ValidateForm();
    
    // Button callbacks
    void OnBackClicked();
    void OnSendClicked();
    void OnClearClicked();
    
    // Helper methods
    Color GetGothamGoldColor() const { return Color(255, 215, 0, 255); }
    Color GetGothamDarkColor() const { return Color(15, 15, 20, 255); }
    bool IsValidAddress(const std::string& address) const;
    bool IsValidAmount(const std::string& amount) const;
};

#endif // GOTHAM_SDL2_SCREENS_SEND_SCREEN_H