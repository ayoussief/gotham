// Copyright (c) 2025 The Gotham Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or https://opensource.org/license/mit/.

#include "send_screen.h"
#include "../gotham_city_gui.h"
#include "../utils/theme.h"
#include "../utils/font_manager.h"
#include "../wallet/wallet_manager.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <regex>

SendScreen::SendScreen(GothamCityGUI& gui) : Screen(gui)
{
    // Initialize UI systems
    if (auto* theme_manager = m_gui.GetThemeManager()) {
        m_ui_factory = std::make_unique<UIFactory>(*theme_manager);
    }
    m_layout_manager = std::make_unique<LayoutManager>();
}

bool SendScreen::Initialize()
{
    if (m_initialized) {
        return true;
    }

    // Only create content area elements
    CreateContentPanel();
    SetupButtonCallbacks();

    m_initialized = true;
    std::cout << "Send screen initialized (content area only)" << std::endl;
    return true;
}

void SendScreen::CreateContentPanel()
{
    if (!m_ui_factory) return;
    
    // Create main content panel using relative coordinates
    // Viewport translation handles positioning
    Rect relative_content_bounds(0, 0, m_content_area_bounds.w, m_content_area_bounds.h);
    m_content_panel = m_ui_factory->CreatePanel(
        relative_content_bounds,
        PanelStyle::CONTENT
    );
    
    CreateFormPanel();
    CreateActionsPanel();
    CreatePreviewPanel();
    CreateBalancePanel();
}

void SendScreen::CreateFormPanel()
{
    // Create send form panel - positioning will be handled by RepositionElements
    m_form_panel = m_ui_factory->CreatePanel(
        Rect(0, 0, 400, 300), // Placeholder bounds
        PanelStyle::CARD
    );
    
    m_form_title_label = m_ui_factory->CreateLabel(
        "Send Bitcoin", 
        Point(0, 0), // Placeholder position
        LabelStyle::HEADING
    );
    
    // Create form elements - positioning will be handled by RepositionElements
    m_recipient_label = m_ui_factory->CreateLabel(
        "Recipient Address:", 
        Point(0, 0), // Placeholder position
        LabelStyle::BODY
    );
    
    m_recipient_input = m_ui_factory->CreateTextInput(
        Rect(0, 0, 350, 35), // Placeholder bounds
        "Enter Bitcoin address..."
    );
    
    m_amount_label = m_ui_factory->CreateLabel(
        "Amount (BTC):", 
        Point(0, 0), // Placeholder position
        LabelStyle::BODY
    );
    
    m_amount_input = m_ui_factory->CreateTextInput(
        Rect(0, 0, 150, 35), // Placeholder bounds
        "0.00000000"
    );
    
    m_fee_label = m_ui_factory->CreateLabel(
        "Fee (BTC):", 
        Point(0, 0), // Placeholder position
        LabelStyle::BODY
    );
    
    m_fee_input = m_ui_factory->CreateTextInput(
        Rect(0, 0, 150, 35), // Placeholder bounds
        "0.00001000"
    );
    
    m_description_label = m_ui_factory->CreateLabel(
        "Description (optional):", 
        Point(0, 0), // Placeholder position
        LabelStyle::BODY
    );
    
    m_description_input = m_ui_factory->CreateTextInput(
        Rect(0, 0, 350, 35), // Placeholder bounds
        "Payment description..."
    );
    

}

void SendScreen::CreateActionsPanel()
{
    // Create actions panel exactly like main screen quick actions panel
    m_actions_panel = m_ui_factory->CreatePanel(
        Rect(0, 0, 100, 84), // Minimal placeholder size (like main screen)
        PanelStyle::CARD
    );
    
    // Create action buttons exactly like main screen - positions will be set by layout
    m_send_button = m_ui_factory->CreateButton(
        "📤 Send Bitcoin", 
        Rect(0, 0, 140, 44), // Same size as main screen buttons
        ButtonStyle::PRIMARY
    );
    
    m_clear_button = m_ui_factory->CreateButton(
        "🗑️ Clear", 
        Rect(0, 0, 140, 44), 
        ButtonStyle::SECONDARY
    );
    
    m_scan_qr_button = m_ui_factory->CreateButton(
        "📷 Scan QR", 
        Rect(0, 0, 140, 44), 
        ButtonStyle::GHOST
    );
}

void SendScreen::CreatePreviewPanel()
{
    // Create transaction preview panel
    m_preview_panel = m_ui_factory->CreatePanel(
        Rect(20, 340, 400, 150),
        PanelStyle::INFO
    );
    
    m_preview_title_label = m_ui_factory->CreateLabel(
        "Transaction Preview", 
        Point(30, 350), 
        LabelStyle::SUBHEADING
    );
    
    m_preview_recipient_label = m_ui_factory->CreateLabel(
        "To: (no recipient)", 
        Point(30, 380), 
        LabelStyle::CAPTION
    );
    
    m_preview_amount_label = m_ui_factory->CreateLabel(
        "Amount: 0.00000000 BTC", 
        Point(30, 400), 
        LabelStyle::BODY
    );
    
    m_preview_fee_label = m_ui_factory->CreateLabel(
        "Fee: 0.00001000 BTC", 
        Point(30, 420), 
        LabelStyle::CAPTION
    );
    
    m_preview_total_label = m_ui_factory->CreateLabel(
        "Total: 0.00001000 BTC", 
        Point(30, 450), 
        LabelStyle::SUBHEADING
    );
}

void SendScreen::CreateBalancePanel()
{
    // Create balance info panel
    m_balance_panel = m_ui_factory->CreatePanel(
        Rect(20, 510, 400, 80),
        PanelStyle::CARD
    );
    
    m_available_balance_label = m_ui_factory->CreateLabel(
        "Available: 0.00000000 BTC", 
        Point(30, 520), 
        LabelStyle::BODY
    );
    
    m_after_send_balance_label = m_ui_factory->CreateLabel(
        "After send: 0.00000000 BTC", 
        Point(30, 545), 
        LabelStyle::CAPTION
    );
}

void SendScreen::SetupButtonCallbacks()
{
    if (m_send_button) {
        m_send_button->SetOnClick([this]() {
            OnSendClicked();
        });
    }
    
    if (m_clear_button) {
        m_clear_button->SetOnClick([this]() {
            OnClearClicked();
        });
    }
    
    if (m_scan_qr_button) {
        m_scan_qr_button->SetOnClick([this]() {
            OnScanQRClicked();
        });
    }
}

void SendScreen::HandleEvent(const SDL_Event& event)
{
    // Handle keyboard shortcuts
    if (event.type == SDL_KEYDOWN) {
        switch (event.key.keysym.sym) {
            case SDLK_RETURN:
                if (event.key.keysym.mod & KMOD_CTRL) {
                    OnSendClicked();
                    return;
                }
                break;
            case SDLK_ESCAPE:
                OnClearClicked();
                return;
        }
    }

    // Handle content area events only
    if (m_content_panel) {
        m_content_panel->HandleEvent(event);
    }
    
    // Handle form inputs
    if (m_recipient_input) m_recipient_input->HandleEvent(event);
    if (m_amount_input) m_amount_input->HandleEvent(event);
    if (m_fee_input) m_fee_input->HandleEvent(event);
    if (m_description_input) m_description_input->HandleEvent(event);
    
    // Handle action buttons
    if (m_send_button) m_send_button->HandleEvent(event);
    if (m_clear_button) m_clear_button->HandleEvent(event);
    if (m_scan_qr_button) m_scan_qr_button->HandleEvent(event);
    
    // Update preview when inputs change
    UpdatePreview();
}

void SendScreen::Update(float delta_time)
{
    m_elapsed_time += delta_time;
    
    // Update form inputs
    if (m_recipient_input) m_recipient_input->Update(delta_time);
    if (m_amount_input) m_amount_input->Update(delta_time);
    if (m_fee_input) m_fee_input->Update(delta_time);
    if (m_description_input) m_description_input->Update(delta_time);
    
    // Update action buttons
    if (m_send_button) m_send_button->Update(delta_time);
    if (m_clear_button) m_clear_button->Update(delta_time);
    if (m_scan_qr_button) m_scan_qr_button->Update(delta_time);
    
    // Validate form periodically
    static float last_validation = 0.0f;
    if (m_elapsed_time - last_validation > 0.5f) {
        ValidateForm();
        UpdateBalanceInfo();
        last_validation = m_elapsed_time;
    }
}

void SendScreen::Render(Renderer& renderer)
{
    // Only render content area!
    if (m_content_panel) {
        m_content_panel->Render(renderer);
    }
    
    // Render panels
    if (m_form_panel) {
        m_form_panel->Render(renderer);
    }
    
    if (m_actions_panel) {
        m_actions_panel->Render(renderer);
    }
    
    if (m_preview_panel) {
        m_preview_panel->Render(renderer);
    }
    
    if (m_balance_panel) {
        m_balance_panel->Render(renderer);
    }
    
    // Get fonts
    TTF_Font* heading_font = m_gui.GetFontManager()->GetDefaultFont(UIStyleGuide::FontSize::HEADING);
    TTF_Font* subheading_font = m_gui.GetFontManager()->GetDefaultFont(UIStyleGuide::FontSize::SUBHEADING);
    TTF_Font* body_font = m_gui.GetFontManager()->GetDefaultFont(UIStyleGuide::FontSize::BODY);
    TTF_Font* caption_font = m_gui.GetFontManager()->GetDefaultFont(UIStyleGuide::FontSize::CAPTION);
    
    // Render form elements
    if (m_form_title_label && heading_font) {
        m_form_title_label->Render(renderer, heading_font);
    }
    
    if (m_recipient_label && body_font) {
        m_recipient_label->Render(renderer, body_font);
    }
    
    if (m_amount_label && body_font) {
        m_amount_label->Render(renderer, body_font);
    }
    
    if (m_fee_label && body_font) {
        m_fee_label->Render(renderer, body_font);
    }
    
    if (m_description_label && body_font) {
        m_description_label->Render(renderer, body_font);
    }
    
    // Render inputs
    if (m_recipient_input && body_font) {
        m_recipient_input->Render(renderer, body_font);
    }
    
    if (m_amount_input && body_font) {
        m_amount_input->Render(renderer, body_font);
    }
    
    if (m_fee_input && body_font) {
        m_fee_input->Render(renderer, body_font);
    }
    
    if (m_description_input && body_font) {
        m_description_input->Render(renderer, body_font);
    }
    
    // Render buttons
    if (m_send_button && body_font) {
        m_send_button->Render(renderer, body_font);
    }
    
    if (m_clear_button && body_font) {
        m_clear_button->Render(renderer, body_font);
    }
    
    if (m_scan_qr_button && caption_font) {
        m_scan_qr_button->Render(renderer, caption_font);
    }
    
    // Render preview
    if (m_preview_title_label && subheading_font) {
        m_preview_title_label->Render(renderer, subheading_font);
    }
    
    if (m_preview_recipient_label && caption_font) {
        m_preview_recipient_label->Render(renderer, caption_font);
    }
    
    if (m_preview_amount_label && body_font) {
        m_preview_amount_label->Render(renderer, body_font);
    }
    
    if (m_preview_fee_label && caption_font) {
        m_preview_fee_label->Render(renderer, caption_font);
    }
    
    if (m_preview_total_label && subheading_font) {
        m_preview_total_label->Render(renderer, subheading_font);
    }
    
    // Render balance info
    if (m_available_balance_label && body_font) {
        m_available_balance_label->Render(renderer, body_font);
    }
    
    if (m_after_send_balance_label && caption_font) {
        m_after_send_balance_label->Render(renderer, caption_font);
    }
}

void SendScreen::OnActivate()
{
    std::cout << "Send screen activated" << std::endl;
    UpdateBalanceInfo();
    ValidateForm();
}

void SendScreen::OnResize(int new_width, int new_height)
{
    // Content area bounds will be updated by SetContentAreaBounds() call from PersistentLayout
    // RepositionElements will be called from SetContentAreaBounds
}

void SendScreen::SetContentAreaBounds(const Rect& bounds)
{
    m_content_area_bounds = bounds;
    std::cout << "Send screen: Setting content area bounds: " << bounds.x << "," << bounds.y 
              << " " << bounds.w << "x" << bounds.h << std::endl;
    
    // Update content panel bounds if it exists, or create it if it doesn't
    if (m_content_panel) {
        // Use relative coordinates since viewport handles translation
        Rect relative_bounds(0, 0, m_content_area_bounds.w, m_content_area_bounds.h);
        m_content_panel->SetBounds(relative_bounds);
        std::cout << "Updated send screen content panel to relative bounds: 0,0 " 
                  << m_content_area_bounds.w << "x" << m_content_area_bounds.h << std::endl;
    } else {
        CreateContentPanel();
        std::cout << "Created new send screen content panel" << std::endl;
    }
    
    // Reposition all elements within the content area
    RepositionElements(m_content_area_bounds.w, m_content_area_bounds.h);
}

void SendScreen::OnNavigatedTo(const NavigationContext& context)
{
    std::cout << "Navigated to Send screen" << std::endl;
    
    // Restore form state if available
    RestoreFormState();
    
    // Handle navigation context
    if (context.HasParameter("recipient")) {
        std::string recipient = context.GetParameter("recipient");
        if (m_recipient_input) {
            m_recipient_input->SetText(recipient);
        }
    }
    
    if (context.HasParameter("amount")) {
        std::string amount = context.GetParameter("amount");
        if (m_amount_input) {
            m_amount_input->SetText(amount);
        }
    }
    
    // Focus on first empty input
    if (m_recipient_input && m_recipient_input->GetText().empty()) {
        m_recipient_input->SetFocus(true);
    } else if (m_amount_input && m_amount_input->GetText() == "0.00000000") {
        m_amount_input->SetFocus(true);
    }
    
    UpdatePreview();
    ValidateForm();
}

void SendScreen::OnNavigatedFrom()
{
    std::cout << "Navigated away from Send screen" << std::endl;
    
    // Save form state before leaving
    SaveFormState();
}

void SendScreen::ValidateForm()
{
    if (!m_recipient_input || !m_amount_input || !m_fee_input || !m_send_button) {
        return;
    }
    
    std::string recipient = m_recipient_input->GetText();
    std::string amount = m_amount_input->GetText();
    std::string fee = m_fee_input->GetText();
    
    bool is_valid = true;
    
    // Validate recipient address
    if (recipient.empty()) {
        is_valid = false;
    } else if (!IsValidBitcoinAddress(recipient)) {
        is_valid = false;
    }
    
    // Validate amount
    if (!IsValidAmount(amount)) {
        is_valid = false;
    }
    
    // Validate fee
    if (!IsValidFee(fee)) {
        is_valid = false;
    }
    
    // Check if we have sufficient balance
    if (is_valid) {
        double amount_val = GetAmountValue();
        double fee_val = GetFeeValue();
        double total_needed = amount_val + fee_val;
        
        if (auto* wallet = m_gui.GetWalletManager()) {
            auto balance = wallet->GetBalance();
            if (total_needed > balance.confirmed) {
                is_valid = false;
            }
        }
    }
    
    m_form_valid = is_valid;
    m_send_button->SetEnabled(is_valid);
}

void SendScreen::UpdatePreview()
{
    if (!m_recipient_input || !m_amount_input || !m_fee_input) {
        return;
    }
    
    std::string recipient = m_recipient_input->GetText();
    std::string amount = m_amount_input->GetText();
    std::string fee = m_fee_input->GetText();
    
    // Update preview labels
    if (m_preview_recipient_label) {
        if (recipient.empty()) {
            m_preview_recipient_label->SetText("To: (no recipient)");
        } else {
            std::string short_addr = recipient.length() > 20 ? 
                recipient.substr(0, 10) + "..." + recipient.substr(recipient.length() - 10) : 
                recipient;
            m_preview_recipient_label->SetText("To: " + short_addr);
        }
    }
    
    if (m_preview_amount_label) {
        m_preview_amount_label->SetText("Amount: " + amount + " BTC");
    }
    
    if (m_preview_fee_label) {
        m_preview_fee_label->SetText("Fee: " + fee + " BTC");
    }
    
    if (m_preview_total_label) {
        double amount_val = GetAmountValue();
        double fee_val = GetFeeValue();
        double total = amount_val + fee_val;
        
        std::ostringstream oss;
        oss << "Total: " << std::fixed << std::setprecision(8) << total << " BTC";
        m_preview_total_label->SetText(oss.str());
    }
}

void SendScreen::UpdateBalanceInfo()
{
    if (auto* wallet = m_gui.GetWalletManager()) {
        auto balance = wallet->GetBalance();
        
        std::ostringstream available_oss;
        available_oss << "Available: " << std::fixed << std::setprecision(8) << balance.confirmed << " BTC";
        
        if (m_available_balance_label) {
            m_available_balance_label->SetText(available_oss.str());
        }
        
        // Calculate balance after send
        double amount_val = GetAmountValue();
        double fee_val = GetFeeValue();
        double after_send = balance.confirmed - amount_val - fee_val;
        
        std::ostringstream after_oss;
        after_oss << "After send: " << std::fixed << std::setprecision(8) << after_send << " BTC";
        
        if (m_after_send_balance_label) {
            m_after_send_balance_label->SetText(after_oss.str());
        }
    }
}

void SendScreen::ClearForm()
{
    if (m_recipient_input) m_recipient_input->SetText("");
    if (m_amount_input) m_amount_input->SetText("0.00000000");
    if (m_fee_input) m_fee_input->SetText("0.00001000");
    if (m_description_input) m_description_input->SetText("");
    
    UpdatePreview();
    ValidateForm();
}

void SendScreen::SaveFormState()
{
    if (m_recipient_input) m_saved_form_state.recipient = m_recipient_input->GetText();
    if (m_amount_input) m_saved_form_state.amount = m_amount_input->GetText();
    if (m_fee_input) m_saved_form_state.fee = m_fee_input->GetText();
    if (m_description_input) m_saved_form_state.description = m_description_input->GetText();
}

void SendScreen::RestoreFormState()
{
    if (!m_saved_form_state.recipient.empty() && m_recipient_input) {
        m_recipient_input->SetText(m_saved_form_state.recipient);
    }
    if (!m_saved_form_state.amount.empty() && m_amount_input) {
        m_amount_input->SetText(m_saved_form_state.amount);
    }
    if (!m_saved_form_state.fee.empty() && m_fee_input) {
        m_fee_input->SetText(m_saved_form_state.fee);
    }
    if (!m_saved_form_state.description.empty() && m_description_input) {
        m_description_input->SetText(m_saved_form_state.description);
    }
}

void SendScreen::RepositionElements(int content_width, int content_height)
{
    // Position elements directly using relative coordinates (exactly like main screen)
    // Viewport translation is handled by PersistentLayout, so use (0,0) as starting point
    int padding = UIStyleGuide::Spacing::LG;
    int current_y = padding; // Start at relative position within content area
    int element_width = content_width - 2 * padding; // Use available width minus padding
    int element_x = padding; // Start at relative position within content area
    
    std::cout << "Send screen: Positioning elements within content area (relative): " << 0 << "," << 0 
              << " " << content_width << "x" << content_height << std::endl;
    
    // Update content panel bounds first
    if (m_content_panel) {
        Rect relative_bounds(0, 0, content_width, content_height);
        m_content_panel->SetBounds(relative_bounds);
    }
    
    // Position form title (exactly like main screen labels)
    if (m_form_title_label) {
        m_form_title_label->SetPosition(Point(element_x, current_y));
        current_y += UIStyleGuide::FontSize::HEADING + UIStyleGuide::Spacing::LG;
    }
    
    // Recipient field
    if (m_recipient_label) {
        m_recipient_label->SetPosition(Point(element_x, current_y));
        current_y += UIStyleGuide::FontSize::BODY + UIStyleGuide::Spacing::SM;
    }
    if (m_recipient_input) {
        m_recipient_input->SetBounds(Rect(element_x, current_y, element_width, 35));
        current_y += 35 + UIStyleGuide::Spacing::MD;
    }
    
    // Amount and Fee fields (side by side)
    int field_width = (element_width - UIStyleGuide::Spacing::MD) / 2;
    if (m_amount_label) {
        m_amount_label->SetPosition(Point(element_x, current_y));
    }
    if (m_fee_label) {
        m_fee_label->SetPosition(Point(element_x + field_width + UIStyleGuide::Spacing::MD, current_y));
    }
    current_y += UIStyleGuide::FontSize::BODY + UIStyleGuide::Spacing::SM;
    
    if (m_amount_input) {
        m_amount_input->SetBounds(Rect(element_x, current_y, field_width, 35));
    }
    if (m_fee_input) {
        m_fee_input->SetBounds(Rect(element_x + field_width + UIStyleGuide::Spacing::MD, current_y, field_width, 35));
    }
    current_y += 35 + UIStyleGuide::Spacing::MD;
    
    // Description field
    if (m_description_label) {
        m_description_label->SetPosition(Point(element_x, current_y));
        current_y += UIStyleGuide::FontSize::BODY + UIStyleGuide::Spacing::SM;
    }
    if (m_description_input) {
        m_description_input->SetBounds(Rect(element_x, current_y, element_width, 35));
        current_y += 35 + UIStyleGuide::Spacing::LG;
    }
    
    // Position form panel background (after calculating form content only)
    if (m_form_panel) {
        int form_panel_height = current_y - padding + UIStyleGuide::Spacing::MD;
        m_form_panel->SetBounds(Rect(element_x, padding, element_width, form_panel_height));
    }
    
    current_y += UIStyleGuide::Spacing::LG; // Spacing after form panel
    
    // Position actions panel exactly like main screen quick actions panel
    if (m_actions_panel) {
        int panel_height = UIStyleGuide::Accessibility::MIN_TOUCH_TARGET + (2 * UIStyleGuide::Spacing::MD);
        m_actions_panel->SetBounds(Rect(element_x, current_y, element_width, panel_height));
        
        // Position buttons within the panel - use flexible layout (exactly like main screen)
        int button_width = 140;
        int button_spacing = UIStyleGuide::Spacing::MD;
        int buttons_total_width = 3 * button_width + 2 * button_spacing;
        
        // Center buttons if there's enough space, otherwise use left alignment
        int button_start_x = element_x + ((element_width > buttons_total_width) ? 
            (element_width - buttons_total_width) / 2 : UIStyleGuide::Spacing::MD);
        int button_y = current_y + UIStyleGuide::Spacing::MD; // Relative position within content area
        
        if (m_send_button) {
            m_send_button->SetBounds(Rect(button_start_x, button_y, button_width, UIStyleGuide::Accessibility::MIN_TOUCH_TARGET));
        }
        if (m_clear_button) {
            m_clear_button->SetBounds(Rect(button_start_x + button_width + button_spacing, button_y, button_width, UIStyleGuide::Accessibility::MIN_TOUCH_TARGET));
        }
        if (m_scan_qr_button) {
            m_scan_qr_button->SetBounds(Rect(button_start_x + 2 * (button_width + button_spacing), button_y, button_width, UIStyleGuide::Accessibility::MIN_TOUCH_TARGET));
        }
        
        current_y += panel_height + UIStyleGuide::Spacing::LG; // Panel height + spacing (exactly like main screen)
    }
    
    
    // Position preview panel
    int preview_panel_height = 150;
    if (m_preview_panel) {
        m_preview_panel->SetBounds(Rect(element_x, current_y, element_width, preview_panel_height));
    }
    
    // Position preview elements directly within content area
    int preview_element_x = element_x + UIStyleGuide::Spacing::MD;
    int preview_current_y = current_y + UIStyleGuide::Spacing::MD;
    
    if (m_preview_title_label) {
        m_preview_title_label->SetPosition(Point(preview_element_x, preview_current_y));
        preview_current_y += UIStyleGuide::FontSize::SUBHEADING + UIStyleGuide::Spacing::SM;
    }
    if (m_preview_recipient_label) {
        m_preview_recipient_label->SetPosition(Point(preview_element_x, preview_current_y));  
        preview_current_y += UIStyleGuide::FontSize::CAPTION + UIStyleGuide::Spacing::XS;
    }
    if (m_preview_amount_label) {
        m_preview_amount_label->SetPosition(Point(preview_element_x, preview_current_y));
        preview_current_y += UIStyleGuide::FontSize::BODY + UIStyleGuide::Spacing::XS;
    }
    if (m_preview_fee_label) {
        m_preview_fee_label->SetPosition(Point(preview_element_x, preview_current_y));
        preview_current_y += UIStyleGuide::FontSize::CAPTION + UIStyleGuide::Spacing::XS;
    }
    if (m_preview_total_label) {
        m_preview_total_label->SetPosition(Point(preview_element_x, preview_current_y));
    }
    
    current_y += preview_panel_height + UIStyleGuide::Spacing::LG; // Move to next section
    
    // Position balance panel
    int balance_panel_height = 80;
    if (m_balance_panel) {
        m_balance_panel->SetBounds(Rect(element_x, current_y, element_width, balance_panel_height));
    }
    
    // Position balance elements directly within content area
    int balance_element_x = element_x + UIStyleGuide::Spacing::MD;
    int balance_current_y = current_y + UIStyleGuide::Spacing::MD;
    
    if (m_available_balance_label) {
        m_available_balance_label->SetPosition(Point(balance_element_x, balance_current_y));
        balance_current_y += UIStyleGuide::FontSize::BODY + UIStyleGuide::Spacing::XS;
    }
    if (m_after_send_balance_label) {
        m_after_send_balance_label->SetPosition(Point(balance_element_x, balance_current_y));
    }
}

void SendScreen::OnSendClicked()
{
    std::cout << "Send button clicked" << std::endl;
    
    if (!m_form_valid) {
        std::cout << "Form is not valid, cannot send" << std::endl;
        return;
    }
    
    // Get form values
    std::string recipient = m_recipient_input ? m_recipient_input->GetText() : "";
    double amount = GetAmountValue();
    double fee = GetFeeValue();
    std::string description = m_description_input ? m_description_input->GetText() : "";
    
    std::cout << "Sending " << amount << " BTC to " << recipient << " with fee " << fee << " BTC" << std::endl;
    
    // In real implementation, this would create and broadcast the transaction
    if (auto* wallet = m_gui.GetWalletManager()) {
        // wallet->SendTransaction(recipient, amount, fee, description);
        std::cout << "Transaction sent successfully!" << std::endl;
        
        // Clear form and navigate back
        ClearForm();
        m_gui.SwitchScreen(ScreenType::MAIN);
    }
}

void SendScreen::OnClearClicked()
{
    std::cout << "Clear button clicked" << std::endl;
    ClearForm();
}

void SendScreen::OnScanQRClicked()
{
    std::cout << "Scan QR button clicked" << std::endl;
    // In real implementation, this would open camera/QR scanner
    // For now, just show a placeholder message
    std::cout << "QR scanner would open here" << std::endl;
}

bool SendScreen::IsValidBitcoinAddress(const std::string& address)
{
    // Basic Bitcoin address validation
    if (address.empty() || address.length() < 26 || address.length() > 62) {
        return false;
    }
    
    // Check for valid Bitcoin address prefixes
    if (address[0] == '1' || address[0] == '3' || 
        (address.length() >= 3 && address.substr(0, 3) == "bc1")) {
        return true;
    }
    
    return false;
}

bool SendScreen::IsValidAmount(const std::string& amount)
{
    if (amount.empty()) return false;
    
    try {
        double val = std::stod(amount);
        return val > 0.0 && val <= 21000000.0; // Max 21M BTC
    } catch (...) {
        return false;
    }
}

bool SendScreen::IsValidFee(const std::string& fee)
{
    if (fee.empty()) return false;
    
    try {
        double val = std::stod(fee);
        return val >= 0.0 && val <= 1.0; // Max 1 BTC fee
    } catch (...) {
        return false;
    }
}

double SendScreen::GetAmountValue()
{
    if (!m_amount_input) return 0.0;
    
    try {
        return std::stod(m_amount_input->GetText());
    } catch (...) {
        return 0.0;
    }
}

double SendScreen::GetFeeValue()
{
    if (!m_fee_input) return 0.0;
    
    try {
        return std::stod(m_fee_input->GetText());
    } catch (...) {
        return 0.0;
    }
}