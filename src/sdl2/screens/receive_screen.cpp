// Copyright (c) 2025 The Gotham Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or https://opensource.org/license/mit/.

#include "receive_screen.h"
#include "../gotham_city_gui.h"
#include "../utils/theme.h"
#include "../utils/font_manager.h"
#include "../wallet/wallet_manager.h"
#include <iostream>

ReceiveScreen::ReceiveScreen(GothamCityGUI& gui) : Screen(gui)
{
    if (auto* theme_manager = m_gui.GetThemeManager()) {
        m_ui_factory = std::make_unique<UIFactory>(*theme_manager);
    }
    m_layout_manager = std::make_unique<LayoutManager>();
}

bool ReceiveScreen::Initialize()
{
    if (m_initialized) return true;
    
    CreateContentPanel();
    SetupButtonCallbacks();
    
    m_initialized = true;
    std::cout << "Receive screen initialized (content area only)" << std::endl;
    return true;
}

void ReceiveScreen::CreateContentPanel()
{
    if (!m_ui_factory) return;
    
    m_content_panel = m_ui_factory->CreatePanel(Rect(0, 0, 800, 600), PanelStyle::CONTENT);
    CreateQRPanel();
    CreateAddressPanel();
    CreateRequestPanel();
    CreateAddressListPanel();
}

void ReceiveScreen::CreateQRPanel()
{
    m_qr_panel = m_ui_factory->CreatePanel(Rect(20, 20, 200, 200), PanelStyle::CARD);
    m_qr_title_label = m_ui_factory->CreateLabel("QR Code", Point(30, 30), LabelStyle::HEADING);
}

void ReceiveScreen::CreateAddressPanel()
{
    m_address_panel = m_ui_factory->CreatePanel(Rect(240, 20, 300, 200), PanelStyle::CARD);
    m_address_title_label = m_ui_factory->CreateLabel("Your Address", Point(250, 30), LabelStyle::HEADING);
    m_address_label = m_ui_factory->CreateLabel("bc1qexampleaddress...", Point(250, 60), LabelStyle::BODY);
    m_copy_address_button = m_ui_factory->CreateButton("📋 Copy", Rect(250, 90, 80, 30), ButtonStyle::PRIMARY);
    m_new_address_button = m_ui_factory->CreateButton("🔄 New", Rect(340, 90, 80, 30), ButtonStyle::SECONDARY);
}

void ReceiveScreen::CreateRequestPanel()
{
    m_request_panel = m_ui_factory->CreatePanel(Rect(20, 240, 300, 150), PanelStyle::CARD);
    m_request_title_label = m_ui_factory->CreateLabel("Request Amount", Point(30, 250), LabelStyle::SUBHEADING);
    m_amount_label = m_ui_factory->CreateLabel("Amount (BTC):", Point(30, 280), LabelStyle::BODY);
    m_amount_input = m_ui_factory->CreateTextInput(Rect(30, 300, 150, 30), "0.00000000");
    m_description_label = m_ui_factory->CreateLabel("Description:", Point(30, 340), LabelStyle::BODY);
    m_description_input = m_ui_factory->CreateTextInput(Rect(30, 360, 250, 30), "Payment for...");
}

void ReceiveScreen::CreateAddressListPanel()
{
    m_address_list_panel = m_ui_factory->CreatePanel(Rect(340, 240, 200, 150), PanelStyle::INFO);
    m_address_list_title_label = m_ui_factory->CreateLabel("Recent Addresses", Point(350, 250), LabelStyle::SUBHEADING);
}

void ReceiveScreen::SetupButtonCallbacks()
{
    if (m_copy_address_button) {
        m_copy_address_button->SetOnClick([this]() { OnCopyAddressClicked(); });
    }
    if (m_new_address_button) {
        m_new_address_button->SetOnClick([this]() { OnNewAddressClicked(); });
    }
}

void ReceiveScreen::HandleEvent(const SDL_Event& event)
{
    if (m_content_panel) m_content_panel->HandleEvent(event);
    if (m_copy_address_button) m_copy_address_button->HandleEvent(event);
    if (m_new_address_button) m_new_address_button->HandleEvent(event);
    if (m_amount_input) m_amount_input->HandleEvent(event);
    if (m_description_input) m_description_input->HandleEvent(event);
}

void ReceiveScreen::Update(float delta_time)
{
    m_elapsed_time += delta_time;
    if (m_copy_address_button) m_copy_address_button->Update(delta_time);
    if (m_new_address_button) m_new_address_button->Update(delta_time);
    if (m_amount_input) m_amount_input->Update(delta_time);
    if (m_description_input) m_description_input->Update(delta_time);
}

void ReceiveScreen::Render(Renderer& renderer)
{
    if (m_content_panel) m_content_panel->Render(renderer);
    if (m_qr_panel) m_qr_panel->Render(renderer);
    if (m_address_panel) m_address_panel->Render(renderer);
    if (m_request_panel) m_request_panel->Render(renderer);
    if (m_address_list_panel) m_address_list_panel->Render(renderer);
    
    TTF_Font* heading_font = m_gui.GetFontManager()->GetDefaultFont(UIStyleGuide::FontSize::HEADING);
    TTF_Font* body_font = m_gui.GetFontManager()->GetDefaultFont(UIStyleGuide::FontSize::BODY);
    
    if (m_qr_title_label && heading_font) m_qr_title_label->Render(renderer, heading_font);
    if (m_address_title_label && heading_font) m_address_title_label->Render(renderer, heading_font);
    if (m_address_label && body_font) m_address_label->Render(renderer, body_font);
    if (m_copy_address_button && body_font) m_copy_address_button->Render(renderer, body_font);
    if (m_new_address_button && body_font) m_new_address_button->Render(renderer, body_font);
    if (m_request_title_label && body_font) m_request_title_label->Render(renderer, body_font);
    if (m_amount_label && body_font) m_amount_label->Render(renderer, body_font);
    if (m_amount_input && body_font) m_amount_input->Render(renderer, body_font);
    if (m_description_label && body_font) m_description_label->Render(renderer, body_font);
    if (m_description_input && body_font) m_description_input->Render(renderer, body_font);
    if (m_address_list_title_label && body_font) m_address_list_title_label->Render(renderer, body_font);
}

void ReceiveScreen::OnActivate()
{
    std::cout << "Receive screen activated" << std::endl;
    GenerateNewAddress();
}

void ReceiveScreen::OnResize(int new_width, int new_height)
{
    // Content area bounds will be updated by SetContentAreaBounds() call from PersistentLayout
    // RepositionElements will be called from SetContentAreaBounds
}

void ReceiveScreen::SetContentAreaBounds(const Rect& bounds)
{
    // TODO: Implement receive screen positioning fix (like send screen)
    // For now, just store the bounds to fix compilation
    // This will be implemented in the next step
    std::cout << "Receive screen: SetContentAreaBounds called: " << bounds.x << "," << bounds.y 
              << " " << bounds.w << "x" << bounds.h << std::endl;
    RepositionElements(bounds.w, bounds.h);
}

void ReceiveScreen::OnNavigatedTo(const NavigationContext& context)
{
    std::cout << "Navigated to Receive screen" << std::endl;
    GenerateNewAddress();
    UpdateQRCode();
}

void ReceiveScreen::OnNavigatedFrom()
{
    std::cout << "Navigated away from Receive screen" << std::endl;
}

void ReceiveScreen::GenerateNewAddress()
{
    if (auto* wallet = m_gui.GetWalletManager()) {
        m_current_address = wallet->GetNewAddress();
        if (m_address_label) {
            m_address_label->SetText(m_current_address);
        }
    }
}

void ReceiveScreen::UpdateQRCode()
{
    // QR code generation would be implemented here
    std::cout << "QR code updated for address: " << m_current_address << std::endl;
}

void ReceiveScreen::RefreshAddressList()
{
    // Address list refresh would be implemented here
}

void ReceiveScreen::RepositionElements(int content_width, int content_height)
{
    // Responsive repositioning would be implemented here
}

void ReceiveScreen::OnCopyAddressClicked()
{
    std::cout << "Address copied to clipboard: " << m_current_address << std::endl;
}

void ReceiveScreen::OnNewAddressClicked()
{
    std::cout << "Generating new address..." << std::endl;
    GenerateNewAddress();
    UpdateQRCode();
}

void ReceiveScreen::OnGenerateRequestClicked()
{
    std::cout << "Payment request generated" << std::endl;
}

void ReceiveScreen::OnShowUsedAddressesClicked()
{
    std::cout << "Showing used addresses" << std::endl;
}