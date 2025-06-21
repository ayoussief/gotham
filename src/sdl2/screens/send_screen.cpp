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
    // Render background with consistent theme
    renderer.Clear(UIStyleGuide::Colors::BACKGROUND_DARK);
    
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
    if (!m_ui_factory || !m_layout_manager) return;
    
    int screen_width = m_gui.GetRenderer()->GetWidth();
    int screen_height = m_gui.GetRenderer()->GetHeight();
    
    // Calculate panel positions with proper spacing
    int header_height = UIStyleGuide::Dimensions::HEADER_HEIGHT;
    int current_y = header_height + UIStyleGuide::Spacing::LG;
    int panel_spacing = UIStyleGuide::Spacing::LG;
    
    CreateHeaderPanel();
    
    // Form panel
    int form_height = 350;
    CreateFormPanel(current_y, form_height);
    current_y += form_height + panel_spacing;
    
    // Preview panel - use remaining space
    int remaining_height = screen_height - current_y - UIStyleGuide::Spacing::LG;
    CreatePreviewPanel(current_y, std::max(200, remaining_height));
}

void SendScreen::CreateHeaderPanel()
{
    if (!m_ui_factory || !m_layout_manager) return;
    
    int width = m_gui.GetRenderer()->GetWidth();
    
    // Create header panel with consistent styling
    Rect header_bounds = Rect(0, 0, width, UIStyleGuide::Dimensions::HEADER_HEIGHT);
    m_header_panel = m_ui_factory->CreatePanel(header_bounds, PanelStyle::HEADER);
    
    // Create header components with factory
    m_title_label = m_ui_factory->CreateLabel("📤 Send Gotham Coin", Point(0, 0), LabelStyle::TITLE);
    m_back_button = m_ui_factory->CreateButton("← Back", Rect(0, 0, 100, 40), ButtonStyle::SECONDARY);
    
    // Setup layout for header components
    std::vector<LayoutItem> header_items(2);
    
    // Title item
    header_items[0].constraints.preferred_width = 300;
    header_items[0].margin = Margin(0, UIStyleGuide::Spacing::LG);
    header_items[0].on_bounds_changed = [this](const Rect& bounds) {
        if (m_title_label) {
            m_title_label->SetPosition(Point(bounds.x, bounds.y + bounds.h / 2 - 12));
        }
    };
    
    // Back button item
    header_items[1].constraints.preferred_width = 100;
    header_items[1].margin = Margin(UIStyleGuide::Spacing::SM);
    header_items[1].on_bounds_changed = [this](const Rect& bounds) {
        if (m_back_button) {
            m_back_button->SetBounds(bounds);
        }
    };
    
    // Apply horizontal layout for header
    m_layout_manager->CreateHorizontalLayout(header_bounds, header_items, 
                                           UIStyleGuide::Spacing::MD, Alignment::CENTER);
}

void SendScreen::CreateFormPanel(int y, int height)
{
    if (!m_ui_factory || !m_layout_manager) return;
    
    int width = m_gui.GetRenderer()->GetWidth();
    int form_width = std::min(600, width - 2 * UIStyleGuide::Spacing::LG);
    int form_x = (width - form_width) / 2;
    
    // Create form panel with consistent styling
    Rect form_bounds = Rect(form_x, y, form_width, height);
    m_form_panel = m_ui_factory->CreatePanel(form_bounds, PanelStyle::CARD);
    
    // Create form components with factory
    m_address_label = m_ui_factory->CreateLabel("Recipient Address:", Point(0, 0), LabelStyle::BODY);
    m_address_input = std::make_unique<TextInput>(Rect(0, 0, form_width - 2 * UIStyleGuide::Spacing::LG, UIStyleGuide::Dimensions::INPUT_HEIGHT));
    m_address_input->SetPlaceholder("Enter Gotham Coin address...");
    m_address_input->SetBackgroundColor(UIStyleGuide::Colors::SURFACE_DARK);
    m_address_input->SetBorderColor(UIStyleGuide::Colors::BORDER_DARK);
    m_address_input->SetTextColor(UIStyleGuide::Colors::TEXT_PRIMARY);
    
    m_amount_label = m_ui_factory->CreateLabel("Amount (GTC):", Point(0, 0), LabelStyle::BODY);
    m_amount_input = std::make_unique<TextInput>(Rect(0, 0, (form_width - 3 * UIStyleGuide::Spacing::LG) / 2, UIStyleGuide::Dimensions::INPUT_HEIGHT));
    m_amount_input->SetPlaceholder("0.00000000");
    m_amount_input->SetBackgroundColor(UIStyleGuide::Colors::SURFACE_DARK);
    m_amount_input->SetBorderColor(UIStyleGuide::Colors::BORDER_DARK);
    m_amount_input->SetTextColor(UIStyleGuide::Colors::TEXT_PRIMARY);
    
    m_fee_label = m_ui_factory->CreateLabel("Fee (GTC):", Point(0, 0), LabelStyle::BODY);
    m_fee_input = std::make_unique<TextInput>(Rect(0, 0, (form_width - 3 * UIStyleGuide::Spacing::LG) / 2, UIStyleGuide::Dimensions::INPUT_HEIGHT));
    m_fee_input->SetPlaceholder("0.00001000");
    m_fee_input->SetText("0.00001000"); // Default fee
    m_fee_input->SetBackgroundColor(UIStyleGuide::Colors::SURFACE_DARK);
    m_fee_input->SetBorderColor(UIStyleGuide::Colors::BORDER_DARK);
    m_fee_input->SetTextColor(UIStyleGuide::Colors::TEXT_PRIMARY);
    
    m_label_label = m_ui_factory->CreateLabel("Label (optional):", Point(0, 0), LabelStyle::BODY);
    m_label_input = std::make_unique<TextInput>(Rect(0, 0, form_width - 2 * UIStyleGuide::Spacing::LG, UIStyleGuide::Dimensions::INPUT_HEIGHT));
    m_label_input->SetPlaceholder("Transaction label...");
    m_label_input->SetBackgroundColor(UIStyleGuide::Colors::SURFACE_DARK);
    m_label_input->SetBorderColor(UIStyleGuide::Colors::BORDER_DARK);
    m_label_input->SetTextColor(UIStyleGuide::Colors::TEXT_PRIMARY);
    
    // Setup form layout using layout manager
    std::vector<LayoutItem> form_items;
    
    // Address field group
    form_items.push_back(LayoutItem{});
    form_items.back().constraints.preferred_height = UIStyleGuide::FontSize::BODY + UIStyleGuide::Spacing::SM;
    form_items.back().margin = Margin(UIStyleGuide::Spacing::MD);
    form_items.back().on_bounds_changed = [this](const Rect& bounds) {
        if (m_address_label) {
            m_address_label->SetPosition(Point(bounds.x, bounds.y));
        }
    };
    
    form_items.push_back(LayoutItem{});
    form_items.back().constraints.preferred_height = UIStyleGuide::Dimensions::INPUT_HEIGHT;
    form_items.back().margin = Margin(UIStyleGuide::Spacing::SM, UIStyleGuide::Spacing::MD);
    form_items.back().on_bounds_changed = [this](const Rect& bounds) {
        if (m_address_input) {
            m_address_input->SetBounds(bounds);
        }
    };
    
    // Amount and Fee row (horizontal layout)
    form_items.push_back(LayoutItem{});
    form_items.back().constraints.preferred_height = UIStyleGuide::FontSize::BODY + UIStyleGuide::Spacing::SM;
    form_items.back().margin = Margin(UIStyleGuide::Spacing::MD);
    form_items.back().on_bounds_changed = [this](const Rect& bounds) {
        if (m_amount_label) {
            m_amount_label->SetPosition(Point(bounds.x, bounds.y));
        }
        if (m_fee_label) {
            m_fee_label->SetPosition(Point(bounds.x + bounds.w / 2, bounds.y));
        }
    };
    
    form_items.push_back(LayoutItem{});
    form_items.back().constraints.preferred_height = UIStyleGuide::Dimensions::INPUT_HEIGHT;
    form_items.back().margin = Margin(UIStyleGuide::Spacing::SM, UIStyleGuide::Spacing::MD);
    form_items.back().on_bounds_changed = [this](const Rect& bounds) {
        if (m_amount_input) {
            Rect amount_bounds = bounds;
            amount_bounds.w = (bounds.w - UIStyleGuide::Spacing::SM) / 2;
            m_amount_input->SetBounds(amount_bounds);
        }
        if (m_fee_input) {
            Rect fee_bounds = bounds;
            fee_bounds.x += (bounds.w + UIStyleGuide::Spacing::SM) / 2;
            fee_bounds.w = (bounds.w - UIStyleGuide::Spacing::SM) / 2;
            m_fee_input->SetBounds(fee_bounds);
        }
    };
    
    // Label field group
    form_items.push_back(LayoutItem{});
    form_items.back().constraints.preferred_height = UIStyleGuide::FontSize::BODY + UIStyleGuide::Spacing::SM;
    form_items.back().margin = Margin(UIStyleGuide::Spacing::MD);
    form_items.back().on_bounds_changed = [this](const Rect& bounds) {
        if (m_label_label) {
            m_label_label->SetPosition(Point(bounds.x, bounds.y));
        }
    };
    
    form_items.push_back(LayoutItem{});
    form_items.back().constraints.preferred_height = UIStyleGuide::Dimensions::INPUT_HEIGHT;
    form_items.back().margin = Margin(UIStyleGuide::Spacing::SM, UIStyleGuide::Spacing::MD);
    form_items.back().on_bounds_changed = [this](const Rect& bounds) {
        if (m_label_input) {
            m_label_input->SetBounds(bounds);
        }
    };
    
    // Apply vertical layout for form
    m_layout_manager->CreateVerticalLayout(form_bounds, form_items, 
                                         UIStyleGuide::Spacing::SM, Alignment::START);
}

void SendScreen::CreatePreviewPanel(int y, int height)
{
    if (!m_ui_factory || !m_layout_manager) return;
    
    int width = m_gui.GetRenderer()->GetWidth();
    int panel_width = std::min(600, width - 2 * UIStyleGuide::Spacing::LG);
    int panel_x = (width - panel_width) / 2;
    
    // Create preview panel with consistent styling
    Rect preview_bounds = Rect(panel_x, y, panel_width, height);
    m_preview_panel = m_ui_factory->CreatePanel(preview_bounds, PanelStyle::CARD);
    
    // Create preview components with factory
    m_preview_title_label = m_ui_factory->CreateLabel("Transaction Preview", Point(0, 0), LabelStyle::HEADING);
    m_preview_title_label->SetColor(UIStyleGuide::Colors::GOTHAM_GOLD);
    
    m_preview_address_label = m_ui_factory->CreateLabel("To: -", Point(0, 0), LabelStyle::BODY);
    m_preview_address_label->SetColor(UIStyleGuide::Colors::TEXT_SECONDARY);
    
    m_preview_amount_label = m_ui_factory->CreateLabel("Amount: 0.00000000 GTC", Point(0, 0), LabelStyle::BODY);
    m_preview_amount_label->SetColor(UIStyleGuide::Colors::TEXT_SECONDARY);
    
    m_preview_fee_label = m_ui_factory->CreateLabel("Fee: 0.00001000 GTC", Point(0, 0), LabelStyle::BODY);
    m_preview_fee_label->SetColor(UIStyleGuide::Colors::TEXT_SECONDARY);
    
    m_preview_total_label = m_ui_factory->CreateLabel("Total: 0.00001000 GTC", Point(0, 0), LabelStyle::BODY);
    m_preview_total_label->SetColor(UIStyleGuide::Colors::GOTHAM_GOLD);
    
    // Create action buttons with consistent styling
    m_send_button = m_ui_factory->CreateButton("💸 Send Transaction", Rect(0, 0, 150, UIStyleGuide::Dimensions::BUTTON_HEIGHT), ButtonStyle::PRIMARY);
    m_send_button->SetEnabled(false); // Disabled until form is valid
    
    m_clear_button = m_ui_factory->CreateButton("🗑️ Clear Form", Rect(0, 0, 150, UIStyleGuide::Dimensions::BUTTON_HEIGHT), ButtonStyle::SECONDARY);
    
    m_status_label = m_ui_factory->CreateLabel("Enter recipient address and amount", Point(0, 0), LabelStyle::CAPTION);
    m_status_label->SetColor(UIStyleGuide::Colors::TEXT_DISABLED);
    
    // Setup preview layout using layout manager
    std::vector<LayoutItem> preview_items;
    
    // Title
    preview_items.push_back(LayoutItem{});
    preview_items.back().constraints.preferred_height = UIStyleGuide::FontSize::HEADING + UIStyleGuide::Spacing::SM;
    preview_items.back().margin = Margin(UIStyleGuide::Spacing::MD);
    preview_items.back().on_bounds_changed = [this](const Rect& bounds) {
        if (m_preview_title_label) {
            m_preview_title_label->SetPosition(Point(bounds.x, bounds.y));
        }
    };
    
    // Address
    preview_items.push_back(LayoutItem{});
    preview_items.back().constraints.preferred_height = UIStyleGuide::FontSize::BODY + UIStyleGuide::Spacing::XS;
    preview_items.back().margin = Margin(UIStyleGuide::Spacing::SM, UIStyleGuide::Spacing::LG);
    preview_items.back().on_bounds_changed = [this](const Rect& bounds) {
        if (m_preview_address_label) {
            m_preview_address_label->SetPosition(Point(bounds.x, bounds.y));
        }
    };
    
    // Amount
    preview_items.push_back(LayoutItem{});
    preview_items.back().constraints.preferred_height = UIStyleGuide::FontSize::BODY + UIStyleGuide::Spacing::XS;
    preview_items.back().margin = Margin(UIStyleGuide::Spacing::SM, UIStyleGuide::Spacing::LG);
    preview_items.back().on_bounds_changed = [this](const Rect& bounds) {
        if (m_preview_amount_label) {
            m_preview_amount_label->SetPosition(Point(bounds.x, bounds.y));
        }
    };
    
    // Fee
    preview_items.push_back(LayoutItem{});
    preview_items.back().constraints.preferred_height = UIStyleGuide::FontSize::BODY + UIStyleGuide::Spacing::XS;
    preview_items.back().margin = Margin(UIStyleGuide::Spacing::SM, UIStyleGuide::Spacing::LG);
    preview_items.back().on_bounds_changed = [this](const Rect& bounds) {
        if (m_preview_fee_label) {
            m_preview_fee_label->SetPosition(Point(bounds.x, bounds.y));
        }
    };
    
    // Total
    preview_items.push_back(LayoutItem{});
    preview_items.back().constraints.preferred_height = UIStyleGuide::FontSize::BODY + UIStyleGuide::Spacing::SM;
    preview_items.back().margin = Margin(UIStyleGuide::Spacing::MD, UIStyleGuide::Spacing::LG);
    preview_items.back().on_bounds_changed = [this](const Rect& bounds) {
        if (m_preview_total_label) {
            m_preview_total_label->SetPosition(Point(bounds.x, bounds.y));
        }
    };
    
    // Action buttons (horizontal layout)
    preview_items.push_back(LayoutItem{});
    preview_items.back().constraints.preferred_height = UIStyleGuide::Dimensions::BUTTON_HEIGHT;
    preview_items.back().margin = Margin(UIStyleGuide::Spacing::LG);
    preview_items.back().on_bounds_changed = [this](const Rect& bounds) {
        if (m_send_button && m_clear_button) {
            int button_width = (bounds.w - UIStyleGuide::Spacing::MD) / 2;
            
            Rect send_bounds = bounds;
            send_bounds.w = button_width;
            m_send_button->SetBounds(send_bounds);
            
            Rect clear_bounds = bounds;
            clear_bounds.x += button_width + UIStyleGuide::Spacing::MD;
            clear_bounds.w = button_width;
            m_clear_button->SetBounds(clear_bounds);
        }
    };
    
    // Status message
    preview_items.push_back(LayoutItem{});
    preview_items.back().constraints.preferred_height = UIStyleGuide::FontSize::CAPTION + UIStyleGuide::Spacing::SM;
    preview_items.back().margin = Margin(UIStyleGuide::Spacing::MD);
    preview_items.back().on_bounds_changed = [this](const Rect& bounds) {
        if (m_status_label) {
            m_status_label->SetPosition(Point(bounds.x, bounds.y));
        }
    };
    
    // Apply vertical layout for preview panel
    m_layout_manager->CreateVerticalLayout(preview_bounds, preview_items, 
                                         UIStyleGuide::Spacing::SM, Alignment::START);
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