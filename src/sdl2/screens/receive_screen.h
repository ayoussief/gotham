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
#include "../ui/ui_factory.h"
#include "../ui/layout_manager.h"
#include "../ui/ui_style_guide.h"
#include "../ui/navigation_manager.h"
#include <memory>

/**
 * Receive Bitcoin screen - Content Area Only
 * Shows QR code and address for receiving Bitcoin
 */
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
    
    // QR code panel
    std::unique_ptr<Panel> m_qr_panel;
    std::unique_ptr<Label> m_qr_title_label;
    // QR code would be rendered as texture/image
    
    // Address panel
    std::unique_ptr<Panel> m_address_panel;
    std::unique_ptr<Label> m_address_title_label;
    std::unique_ptr<Label> m_address_label;
    std::unique_ptr<Button> m_copy_address_button;
    std::unique_ptr<Button> m_new_address_button;
    
    // Request amount panel
    std::unique_ptr<Panel> m_request_panel;
    std::unique_ptr<Label> m_request_title_label;
    std::unique_ptr<Label> m_amount_label;
    std::unique_ptr<TextInput> m_amount_input;
    std::unique_ptr<Label> m_description_label;
    std::unique_ptr<TextInput> m_description_input;
    std::unique_ptr<Button> m_generate_request_button;
    
    // Address list panel
    std::unique_ptr<Panel> m_address_list_panel;
    std::unique_ptr<Label> m_address_list_title_label;
    std::vector<std::unique_ptr<Label>> m_address_list_labels;
    std::unique_ptr<Button> m_show_used_addresses_button;
    
    std::string m_current_address;
    std::string m_requested_amount;
    std::string m_request_description;
    float m_elapsed_time{0.0f};
    
    // Content area methods
    void CreateContentPanel();
    void CreateQRPanel();
    void CreateAddressPanel();
    void CreateRequestPanel();
    void CreateAddressListPanel();
    void SetupButtonCallbacks();
    void GenerateNewAddress();
    void UpdateQRCode();
    void RefreshAddressList();
    void RepositionElements(int content_width, int content_height);
    
    // Action callbacks
    void OnCopyAddressClicked();
    void OnNewAddressClicked();
    void OnGenerateRequestClicked();
    void OnShowUsedAddressesClicked();
};

#endif // GOTHAM_SDL2_SCREENS_RECEIVE_SCREEN_H