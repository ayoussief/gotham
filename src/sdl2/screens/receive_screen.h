// Copyright (c) 2025 The Gotham Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or https://opensource.org/license/mit/.

#ifndef GOTHAM_SDL2_SCREENS_RECEIVE_SCREEN_H
#define GOTHAM_SDL2_SCREENS_RECEIVE_SCREEN_H

#include "../ui/screen.h"
#include "../ui/button.h"
#include "../ui/label.h"
#include "../ui/panel.h"
#include "../ui/text_input.h"
#include <memory>

class ReceiveScreen : public Screen
{
public:
    explicit ReceiveScreen(GothamCityGUI& gui);
    bool Initialize() override;
    void HandleEvent(const SDL_Event& event) override;
    void Update(float delta_time) override;
    void Render(Renderer& renderer) override;
    void OnActivate() override;
    void OnResize(int new_width, int new_height) override;

private:
    // UI Components
    std::unique_ptr<Panel> m_header_panel;
    std::unique_ptr<Panel> m_address_panel;
    std::unique_ptr<Panel> m_qr_panel;
    
    // Header
    std::unique_ptr<Label> m_title_label;
    std::unique_ptr<Button> m_back_button;
    
    // Address section
    std::unique_ptr<Label> m_address_title_label;
    std::unique_ptr<Label> m_address_label;
    std::unique_ptr<Button> m_copy_button;
    std::unique_ptr<Button> m_new_address_button;
    
    // Amount request (optional)
    std::unique_ptr<Label> m_amount_title_label;
    std::unique_ptr<TextInput> m_amount_input;
    std::unique_ptr<Label> m_amount_label_input_label;
    std::unique_ptr<TextInput> m_amount_label_input;
    
    // QR Code placeholder
    std::unique_ptr<Label> m_qr_title_label;
    std::unique_ptr<Label> m_qr_placeholder_label;
    
    // Status
    std::unique_ptr<Label> m_status_label;
    
    std::string m_current_address;
    float m_elapsed_time{0.0f};
    
    void CreateLayout();
    void CreateHeaderPanel();
    void CreateAddressPanel();
    void CreateQRPanel();
    void GenerateNewAddress();
    void SetupButtonCallbacks();
    
    // Button callbacks
    void OnBackClicked();
    void OnCopyClicked();
    void OnNewAddressClicked();
    
    // Helper methods
    Color GetGothamGoldColor() const;
    Color GetGothamDarkColor() const;
};

#endif // GOTHAM_SDL2_SCREENS_RECEIVE_SCREEN_H