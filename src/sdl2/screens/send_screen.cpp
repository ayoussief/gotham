// Copyright (c) 2025 The Gotham Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or https://opensource.org/license/mit/.

#include "send_screen.h"
#include "../gotham_city_gui.h"
#include "../utils/theme.h"
#include "../utils/font_manager.h"
#include <iostream>
#include <regex>
#include <sstream>
#include <iomanip>

SendScreen::SendScreen(GothamCityGUI& gui) : Screen(gui)
{
}

bool SendScreen::Initialize()
{
    if (m_initialized) {
        return true;
    }

    CreateLayout();
    SetupButtonCallbacks();

    m_initialized = true;
    std::cout << "Send screen initialized" << std::endl;
    return true;
}

void SendScreen::HandleEvent(const SDL_Event& event)
{
    // Handle keyboard shortcuts
    if (event.type == SDL_KEYDOWN) {
        switch (event.key.keysym.sym) {
            case SDLK_ESCAPE:
                OnBackClicked();
                return;
            case SDLK_RETURN:
                if (event.key.keysym.mod & KMOD_CTRL) {
                    OnSendClicked();
                    return;
                }
                break;
        }
    }

    // Pass events to UI components
    if (m_back_button) m_back_button->HandleEvent(event);
    if (m_address_input) m_address_input->HandleEvent(event);
    if (m_amount_input) m_amount_input->HandleEvent(event);
    if (m_fee_input) m_fee_input->HandleEvent(event);
    if (m_label_input) m_label_input->HandleEvent(event);
    if (m_send_button) m_send_button->HandleEvent(event);
    if (m_clear_button) m_clear_button->HandleEvent(event);
}

void SendScreen::Update(float delta_time)
{
    m_elapsed_time += delta_time;
    
    // Update UI components
    if (m_back_button) m_back_button->Update(delta_time);
    if (m_address_input) m_address_input->Update(delta_time);
    if (m_amount_input) m_amount_input->Update(delta_time);
    if (m_fee_input) m_fee_input->Update(delta_time);
    if (m_label_input) m_label_input->Update(delta_time);
    if (m_send_button) m_send_button->Update(delta_time);
    if (m_clear_button) m_clear_button->Update(delta_time);
    
    // Update preview when inputs change
    UpdatePreview();
    ValidateForm();
}

void SendScreen::Render(Renderer& renderer)
{
    // Render background
    renderer.Clear(GetGothamDarkColor());
    
    // Render UI panels
    if (m_header_panel) m_header_panel->Render(renderer);
    if (m_form_panel) m_form_panel->Render(renderer);
    if (m_preview_panel) m_preview_panel->Render(renderer);
    
    // Render components with fonts
    FontManager* font_manager = m_gui.GetFontManager();
    if (font_manager) {
        TTF_Font* title_font = font_manager->GetDefaultFont(24);
        TTF_Font* label_font = font_manager->GetDefaultFont(16);
        TTF_Font* button_font = font_manager->GetDefaultFont(16);
        TTF_Font* body_font = font_manager->GetDefaultFont(14);
        
        // Header
        if (m_title_label && title_font) {
            m_title_label->Render(renderer, title_font);
        }
        if (m_back_button && button_font) {
            m_back_button->Render(renderer, button_font);
        }
        
        // Form labels
        if (m_address_label && label_font) {
            m_address_label->Render(renderer, label_font);
        }
        if (m_amount_label && label_font) {
            m_amount_label->Render(renderer, label_font);
        }
        if (m_fee_label && label_font) {
            m_fee_label->Render(renderer, label_font);
        }
        if (m_label_label && label_font) {
            m_label_label->Render(renderer, label_font);
        }
        
        // Form inputs
        if (m_address_input && body_font) {
            m_address_input->Render(renderer, body_font);
        }
        if (m_amount_input && body_font) {
            m_amount_input->Render(renderer, body_font);
        }
        if (m_fee_input && body_font) {
            m_fee_input->Render(renderer, body_font);
        }
        if (m_label_input && body_font) {
            m_label_input->Render(renderer, body_font);
        }
        
        // Preview section
        if (m_preview_title_label && label_font) {
            m_preview_title_label->Render(renderer, label_font);
        }
        if (m_preview_address_label && body_font) {
            m_preview_address_label->Render(renderer, body_font);
        }
        if (m_preview_amount_label && body_font) {
            m_preview_amount_label->Render(renderer, body_font);
        }
        if (m_preview_fee_label && body_font) {
            m_preview_fee_label->Render(renderer, body_font);
        }
        if (m_preview_total_label && label_font) {
            m_preview_total_label->Render(renderer, label_font);
        }
        
        // Action buttons
        if (m_send_button && button_font) {
            m_send_button->Render(renderer, button_font);
        }
        if (m_clear_button && button_font) {
            m_clear_button->Render(renderer, button_font);
        }
        
        // Status
        if (m_status_label && body_font) {
            m_status_label->Render(renderer, body_font);
        }
    }
}

void SendScreen::OnActivate()
{
    std::cout << "Send screen activated" << std::endl;
    OnClearClicked(); // Clear form when activated
}

void SendScreen::OnResize(int new_width, int new_height)
{
    std::cout << "Send screen resizing to: " << new_width << "x" << new_height << std::endl;
    CreateLayout();
    SetupButtonCallbacks();
}

void SendScreen::CreateLayout()
{
    CreateHeaderPanel();
    CreateFormPanel();
    CreatePreviewPanel();
}

void SendScreen::CreateHeaderPanel()
{
    int width = m_gui.GetRenderer()->GetWidth();
    
    m_header_panel = std::make_unique<Panel>(Rect(0, 0, width, 80));
    m_header_panel->SetBackgroundColor(GetGothamDarkColor());
    
    m_title_label = std::make_unique<Label>("📤 Send Bitcoin", Point(50, 30));
    m_title_label->SetColor(GetGothamGoldColor());
    
    m_back_button = std::make_unique<Button>("← Back", Rect(width - 120, 20, 100, 40));
    m_back_button->SetColors(Color(60, 60, 70, 255), Color(80, 80, 90, 255), Color(40, 40, 50, 255));
    m_back_button->SetTextColor(Color(255, 255, 255, 255));
}

void SendScreen::CreateFormPanel()
{
    int width = m_gui.GetRenderer()->GetWidth();
    int form_width = std::min(600, width - 40);
    int form_x = (width - form_width) / 2;
    
    m_form_panel = std::make_unique<Panel>(Rect(form_x, 100, form_width, 300));
    m_form_panel->SetBackgroundColor(Color(25, 25, 35, 255));
    m_form_panel->SetBorderColor(Color(60, 60, 70, 255));
    
    int input_width = form_width - 60;
    int y_pos = 120;
    
    // Address field
    m_address_label = std::make_unique<Label>("Recipient Address:", Point(form_x + 20, y_pos));
    m_address_label->SetColor(Color(255, 255, 255, 255));
    y_pos += 25;
    
    m_address_input = std::make_unique<TextInput>(Rect(form_x + 30, y_pos, input_width, 30));
    m_address_input->SetPlaceholder("Enter Bitcoin address...");
    y_pos += 45;
    
    // Amount field
    m_amount_label = std::make_unique<Label>("Amount (BTC):", Point(form_x + 20, y_pos));
    m_amount_label->SetColor(Color(255, 255, 255, 255));
    y_pos += 25;
    
    m_amount_input = std::make_unique<TextInput>(Rect(form_x + 30, y_pos, input_width / 2 - 10, 30));
    m_amount_input->SetPlaceholder("0.00000000");
    
    // Fee field
    m_fee_label = std::make_unique<Label>("Fee (BTC):", Point(form_x + input_width / 2 + 40, y_pos - 25));
    m_fee_label->SetColor(Color(255, 255, 255, 255));
    
    m_fee_input = std::make_unique<TextInput>(Rect(form_x + input_width / 2 + 50, y_pos, input_width / 2 - 10, 30));
    m_fee_input->SetPlaceholder("0.00001000");
    m_fee_input->SetText("0.00001000"); // Default fee
    y_pos += 45;
    
    // Label field
    m_label_label = std::make_unique<Label>("Label (optional):", Point(form_x + 20, y_pos));
    m_label_label->SetColor(Color(255, 255, 255, 255));
    y_pos += 25;
    
    m_label_input = std::make_unique<TextInput>(Rect(form_x + 30, y_pos, input_width, 30));
    m_label_input->SetPlaceholder("Transaction label...");
}

void SendScreen::CreatePreviewPanel()
{
    int width = m_gui.GetRenderer()->GetWidth();
    int height = m_gui.GetRenderer()->GetHeight();
    int panel_width = std::min(600, width - 40);
    int panel_x = (width - panel_width) / 2;
    
    m_preview_panel = std::make_unique<Panel>(Rect(panel_x, 420, panel_width, height - 500));
    m_preview_panel->SetBackgroundColor(Color(20, 20, 30, 255));
    m_preview_panel->SetBorderColor(GetGothamGoldColor());
    
    int y_pos = 440;
    
    m_preview_title_label = std::make_unique<Label>("Transaction Preview", Point(panel_x + 20, y_pos));
    m_preview_title_label->SetColor(GetGothamGoldColor());
    y_pos += 35;
    
    m_preview_address_label = std::make_unique<Label>("To: -", Point(panel_x + 30, y_pos));
    m_preview_address_label->SetColor(Color(200, 200, 200, 255));
    y_pos += 25;
    
    m_preview_amount_label = std::make_unique<Label>("Amount: 0.00000000 BTC", Point(panel_x + 30, y_pos));
    m_preview_amount_label->SetColor(Color(200, 200, 200, 255));
    y_pos += 25;
    
    m_preview_fee_label = std::make_unique<Label>("Fee: 0.00001000 BTC", Point(panel_x + 30, y_pos));
    m_preview_fee_label->SetColor(Color(200, 200, 200, 255));
    y_pos += 25;
    
    m_preview_total_label = std::make_unique<Label>("Total: 0.00001000 BTC", Point(panel_x + 30, y_pos));
    m_preview_total_label->SetColor(GetGothamGoldColor());
    y_pos += 40;
    
    // Action buttons
    int button_width = 120;
    int button_spacing = 20;
    int buttons_start_x = panel_x + (panel_width - (2 * button_width + button_spacing)) / 2;
    
    m_send_button = std::make_unique<Button>("Send", Rect(buttons_start_x, y_pos, button_width, 40));
    m_send_button->SetColors(Color(220, 53, 69, 255), Color(240, 73, 89, 255), Color(200, 33, 49, 255));
    m_send_button->SetTextColor(Color(255, 255, 255, 255));
    m_send_button->SetEnabled(false); // Disabled until form is valid
    
    m_clear_button = std::make_unique<Button>("Clear", Rect(buttons_start_x + button_width + button_spacing, y_pos, button_width, 40));
    m_clear_button->SetColors(Color(108, 117, 125, 255), Color(128, 137, 145, 255), Color(88, 97, 105, 255));
    m_clear_button->SetTextColor(Color(255, 255, 255, 255));
    
    y_pos += 60;
    
    m_status_label = std::make_unique<Label>("Enter recipient address and amount", Point(panel_x + 30, y_pos));
    m_status_label->SetColor(Color(150, 150, 150, 255));
}

void SendScreen::UpdatePreview()
{
    if (!m_address_input || !m_amount_input || !m_fee_input) {
        return;
    }
    
    std::string address = m_address_input->GetText();
    std::string amount_str = m_amount_input->GetText();
    std::string fee_str = m_fee_input->GetText();
    
    // Update preview labels
    if (m_preview_address_label) {
        std::string preview_addr = address.empty() ? "-" : (address.length() > 30 ? address.substr(0, 30) + "..." : address);
        m_preview_address_label->SetText("To: " + preview_addr);
    }
    
    if (m_preview_amount_label) {
        std::string amount_display = amount_str.empty() ? "0.00000000" : amount_str;
        m_preview_amount_label->SetText("Amount: " + amount_display + " BTC");
    }
    
    if (m_preview_fee_label) {
        std::string fee_display = fee_str.empty() ? "0.00001000" : fee_str;
        m_preview_fee_label->SetText("Fee: " + fee_display + " BTC");
    }
    
    if (m_preview_total_label) {
        double amount = amount_str.empty() ? 0.0 : std::stod(amount_str);
        double fee = fee_str.empty() ? 0.00001000 : std::stod(fee_str);
        double total = amount + fee;
        
        std::ostringstream oss;
        oss << std::fixed << std::setprecision(8) << total;
        m_preview_total_label->SetText("Total: " + oss.str() + " BTC");
    }
}

void SendScreen::ValidateForm()
{
    if (!m_address_input || !m_amount_input || !m_send_button || !m_status_label) {
        return;
    }
    
    std::string address = m_address_input->GetText();
    std::string amount_str = m_amount_input->GetText();
    
    bool is_valid = true;
    std::string status_message;
    
    if (address.empty()) {
        is_valid = false;
        status_message = "Enter recipient address";
    } else if (!IsValidAddress(address)) {
        is_valid = false;
        status_message = "Invalid Bitcoin address";
    } else if (amount_str.empty()) {
        is_valid = false;
        status_message = "Enter amount to send";
    } else if (!IsValidAmount(amount_str)) {
        is_valid = false;
        status_message = "Invalid amount";
    } else if (std::stod(amount_str) <= 0) {
        is_valid = false;
        status_message = "Amount must be greater than 0";
    } else {
        status_message = m_transaction_pending ? "Sending transaction..." : "Ready to send";
    }
    
    m_send_button->SetEnabled(is_valid && !m_transaction_pending);
    m_status_label->SetText(status_message);
    m_status_label->SetColor(is_valid ? Color(46, 160, 67, 255) : Color(220, 53, 69, 255));
}

void SendScreen::SetupButtonCallbacks()
{
    if (m_back_button) {
        m_back_button->SetOnClick([this]() { OnBackClicked(); });
    }
    if (m_send_button) {
        m_send_button->SetOnClick([this]() { OnSendClicked(); });
    }
    if (m_clear_button) {
        m_clear_button->SetOnClick([this]() { OnClearClicked(); });
    }
}

void SendScreen::OnBackClicked()
{
    m_gui.SwitchScreen(ScreenType::WALLET);
}

void SendScreen::OnSendClicked()
{
    if (m_transaction_pending) {
        return;
    }
    
    std::cout << "Sending transaction..." << std::endl;
    m_transaction_pending = true;
    
    if (m_status_label) {
        m_status_label->SetText("Sending transaction...");
        m_status_label->SetColor(Color(255, 193, 7, 255));
    }
    
    // TODO: Implement actual transaction sending
    // For now, simulate a delay and success
    // In a real implementation, this would interact with the wallet manager
}

void SendScreen::OnClearClicked()
{
    if (m_address_input) m_address_input->SetText("");
    if (m_amount_input) m_amount_input->SetText("");
    if (m_fee_input) m_fee_input->SetText("0.00001000");
    if (m_label_input) m_label_input->SetText("");
    
    m_transaction_pending = false;
    
    if (m_status_label) {
        m_status_label->SetText("Enter recipient address and amount");
        m_status_label->SetColor(Color(150, 150, 150, 255));
    }
}

bool SendScreen::IsValidAddress(const std::string& address) const
{
    // Simple Bitcoin address validation
    if (address.length() < 26 || address.length() > 35) {
        return false;
    }
    
    // Check if it starts with valid prefixes
    return (address[0] == '1' || address[0] == '3' || 
            (address.length() >= 3 && address.substr(0, 3) == "bc1"));
}

bool SendScreen::IsValidAmount(const std::string& amount) const
{
    if (amount.empty()) {
        return false;
    }
    
    try {
        double value = std::stod(amount);
        return value > 0 && value <= 21000000; // Max Bitcoin supply
    } catch (const std::exception&) {
        return false;
    }
}