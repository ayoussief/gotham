// Copyright (c) 2025 The Gotham Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or https://opensource.org/license/mit/.

#include "receive_screen.h"
#include "../gotham_city_gui.h"
#include "../renderer.h"
#include "../utils/theme.h"
#include "../utils/font_manager.h"
#include <iostream>
#include <random>
#include <algorithm>

ReceiveScreen::ReceiveScreen(GothamCityGUI& gui) : Screen(gui)
{
    // Initialize UI systems
    if (auto* theme_manager = m_gui.GetThemeManager()) {
        m_ui_factory = std::make_unique<UIFactory>(*theme_manager);
    }
    m_layout_manager = std::make_unique<LayoutManager>();
}

bool ReceiveScreen::Initialize()
{
    if (m_initialized) {
        return true;
    }

    GenerateNewAddress();
    CreateLayout();
    SetupButtonCallbacks();
    m_initialized = true;
    std::cout << "Receive screen initialized" << std::endl;
    return true;
}

void ReceiveScreen::HandleEvent(const SDL_Event& event)
{
    // Handle keyboard shortcuts
    if (event.type == SDL_KEYDOWN) {
        switch (event.key.keysym.sym) {
            case SDLK_ESCAPE:
                m_gui.SwitchScreen(ScreenType::WALLET);
                return;
            case SDLK_c:
                if (event.key.keysym.mod & KMOD_CTRL) {
                    SDL_SetClipboardText(m_current_address.c_str());
                    std::cout << "Address copied to clipboard: " << m_current_address << std::endl;
                    return;
                }
                break;
            case SDLK_n:
                if (event.key.keysym.mod & KMOD_CTRL) {
                    GenerateNewAddress();
                    if (m_address_label) {
                        m_address_label->SetText(m_current_address);
                    }
                    std::cout << "New address generated: " << m_current_address << std::endl;
                    return;
                }
                break;
        }
    }
    
    // Pass events to UI components
    if (m_back_button) m_back_button->HandleEvent(event);
    if (m_copy_button) m_copy_button->HandleEvent(event);
    if (m_new_address_button) m_new_address_button->HandleEvent(event);
    if (m_amount_input) m_amount_input->HandleEvent(event);
    if (m_amount_label_input) m_amount_label_input->HandleEvent(event);
}

void ReceiveScreen::Update(float delta_time)
{
    m_elapsed_time += delta_time;
}

void ReceiveScreen::Render(Renderer& renderer)
{
    // Render background with consistent theme
    renderer.Clear(UIStyleGuide::Colors::BACKGROUND_DARK);
    
    // Render UI panels
    if (m_header_panel) m_header_panel->Render(renderer);
    if (m_address_panel) m_address_panel->Render(renderer);
    if (m_qr_panel) m_qr_panel->Render(renderer);
    
    // Get fonts from font manager
    FontManager* font_manager = m_gui.GetFontManager();
    if (font_manager) {
        TTF_Font* title_font = font_manager->GetDefaultFont(24);
        TTF_Font* body_font = font_manager->GetDefaultFont(16);
        TTF_Font* small_font = font_manager->GetDefaultFont(14);
        
        // Render components with fonts
        if (m_title_label) m_title_label->Render(renderer, title_font);
        if (m_back_button) m_back_button->Render(renderer, body_font);
        
        if (m_address_title_label) m_address_title_label->Render(renderer, body_font);
        if (m_address_label) m_address_label->Render(renderer, small_font);
        if (m_copy_button) m_copy_button->Render(renderer, body_font);
        if (m_new_address_button) m_new_address_button->Render(renderer, body_font);
        
        if (m_amount_title_label) m_amount_title_label->Render(renderer, body_font);
        if (m_amount_input) m_amount_input->Render(renderer, body_font);
        if (m_amount_label_input_label) m_amount_label_input_label->Render(renderer, body_font);
        if (m_amount_label_input) m_amount_label_input->Render(renderer, body_font);
        
        if (m_qr_title_label) m_qr_title_label->Render(renderer, body_font);
        if (m_qr_placeholder_label) m_qr_placeholder_label->Render(renderer, body_font);
    }
}

void ReceiveScreen::OnActivate()
{
    std::cout << "Receive screen activated" << std::endl;
}

void ReceiveScreen::OnResize(int new_width, int new_height)
{
    std::cout << "Receive screen resizing to: " << new_width << "x" << new_height << std::endl;
}

void ReceiveScreen::GenerateNewAddress()
{
    // Generate a mock Bitcoin address
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 15);
    
    m_current_address = "bc1q";
    for (int i = 0; i < 39; ++i) {
        char c = dis(gen) < 10 ? '0' + dis(gen) % 10 : 'a' + dis(gen) % 6;
        m_current_address += c;
    }
}

void ReceiveScreen::CreateLayout()
{
    if (!m_ui_factory || !m_layout_manager) return;
    
    int screen_width = m_gui.GetRenderer()->GetWidth();
    int screen_height = m_gui.GetRenderer()->GetHeight();
    
    // Calculate panel positions with proper spacing
    int header_height = UIStyleGuide::Dimensions::HEADER_HEIGHT;
    int current_y = header_height + UIStyleGuide::Spacing::LG;
    int panel_spacing = UIStyleGuide::Spacing::LG;
    
    CreateHeaderPanel();
    
    // Address panel
    int address_height = 200;
    CreateAddressPanel(current_y, address_height);
    current_y += address_height + panel_spacing;
    
    // QR panel - use remaining space
    int remaining_height = screen_height - current_y - UIStyleGuide::Spacing::LG;
    CreateQRPanel(current_y, std::max(250, remaining_height));
}

void ReceiveScreen::CreateHeaderPanel()
{
    if (!m_ui_factory || !m_layout_manager) return;
    
    int width = m_gui.GetRenderer()->GetWidth();
    
    // Create header panel with consistent styling
    Rect header_bounds = Rect(0, 0, width, UIStyleGuide::Dimensions::HEADER_HEIGHT);
    m_header_panel = m_ui_factory->CreatePanel(header_bounds, PanelStyle::HEADER);
    
    // Create header components with factory
    m_title_label = m_ui_factory->CreateLabel("📥 Receive Gotham Coin", Point(0, 0), LabelStyle::TITLE);
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

void ReceiveScreen::CreateAddressPanel(int y, int height)
{
    if (!m_ui_factory || !m_layout_manager) return;
    
    int width = m_gui.GetRenderer()->GetWidth();
    int panel_width = std::min(600, width - 2 * UIStyleGuide::Spacing::LG);
    int panel_x = (width - panel_width) / 2;
    
    // Create address panel with consistent styling
    Rect address_bounds = Rect(panel_x, y, panel_width, height);
    m_address_panel = m_ui_factory->CreatePanel(address_bounds, PanelStyle::CARD);
    
    // Create address components with factory
    m_address_title_label = m_ui_factory->CreateLabel("Your Gotham Coin Address", Point(0, 0), LabelStyle::HEADING);
    m_address_label = m_ui_factory->CreateLabel(m_current_address, Point(0, 0), LabelStyle::BODY);
    m_address_label->SetColor(UIStyleGuide::Colors::GOTHAM_GOLD);
    
    // Create action buttons
    m_copy_button = m_ui_factory->CreateButton("📋 Copy Address", Rect(0, 0, 150, UIStyleGuide::Dimensions::BUTTON_HEIGHT), ButtonStyle::PRIMARY);
    m_new_address_button = m_ui_factory->CreateButton("🔄 New Address", Rect(0, 0, 150, UIStyleGuide::Dimensions::BUTTON_HEIGHT), ButtonStyle::SECONDARY);
    
    // Create amount request inputs
    m_amount_title_label = m_ui_factory->CreateLabel("Request Amount (optional)", Point(0, 0), LabelStyle::BODY);
    m_amount_input = std::make_unique<TextInput>(Rect(0, 0, (panel_width - 3 * UIStyleGuide::Spacing::MD) / 2, UIStyleGuide::Dimensions::INPUT_HEIGHT));
    m_amount_input->SetPlaceholder("0.00000000");
    m_amount_input->SetBackgroundColor(UIStyleGuide::Colors::SURFACE_DARK);
    m_amount_input->SetBorderColor(UIStyleGuide::Colors::BORDER_DARK);
    m_amount_input->SetTextColor(UIStyleGuide::Colors::TEXT_PRIMARY);
    
    m_amount_label_input_label = m_ui_factory->CreateLabel("Label", Point(0, 0), LabelStyle::BODY);
    m_amount_label_input = std::make_unique<TextInput>(Rect(0, 0, (panel_width - 3 * UIStyleGuide::Spacing::MD) / 2, UIStyleGuide::Dimensions::INPUT_HEIGHT));
    m_amount_label_input->SetPlaceholder("Payment for...");
    m_amount_label_input->SetBackgroundColor(UIStyleGuide::Colors::SURFACE_DARK);
    m_amount_label_input->SetBorderColor(UIStyleGuide::Colors::BORDER_DARK);
    m_amount_label_input->SetTextColor(UIStyleGuide::Colors::TEXT_PRIMARY);
    
    // Setup layout for address panel
    std::vector<LayoutItem> address_items;
    
    // Address title
    address_items.push_back(LayoutItem{});
    address_items.back().constraints.preferred_height = UIStyleGuide::FontSize::HEADING + UIStyleGuide::Spacing::SM;
    address_items.back().margin = Margin(UIStyleGuide::Spacing::MD);
    address_items.back().on_bounds_changed = [this](const Rect& bounds) {
        if (m_address_title_label) {
            m_address_title_label->SetPosition(Point(bounds.x, bounds.y));
        }
    };
    
    // Address display
    address_items.push_back(LayoutItem{});
    address_items.back().constraints.preferred_height = UIStyleGuide::FontSize::BODY + UIStyleGuide::Spacing::SM;
    address_items.back().margin = Margin(UIStyleGuide::Spacing::SM, UIStyleGuide::Spacing::MD);
    address_items.back().on_bounds_changed = [this](const Rect& bounds) {
        if (m_address_label) {
            m_address_label->SetPosition(Point(bounds.x, bounds.y));
        }
    };
    
    // Action buttons (horizontal layout)
    address_items.push_back(LayoutItem{});
    address_items.back().constraints.preferred_height = UIStyleGuide::Dimensions::BUTTON_HEIGHT;
    address_items.back().margin = Margin(UIStyleGuide::Spacing::MD);
    address_items.back().on_bounds_changed = [this](const Rect& bounds) {
        if (m_copy_button && m_new_address_button) {
            int button_width = (bounds.w - UIStyleGuide::Spacing::MD) / 2;
            
            Rect copy_bounds = bounds;
            copy_bounds.w = button_width;
            m_copy_button->SetBounds(copy_bounds);
            
            Rect new_bounds = bounds;
            new_bounds.x += button_width + UIStyleGuide::Spacing::MD;
            new_bounds.w = button_width;
            m_new_address_button->SetBounds(new_bounds);
        }
    };
    
    // Apply vertical layout for address panel
    m_layout_manager->CreateVerticalLayout(address_bounds, address_items, 
                                         UIStyleGuide::Spacing::SM, Alignment::START);
}

void ReceiveScreen::CreateQRPanel(int y, int height)
{
    if (!m_ui_factory || !m_layout_manager) return;
    
    int width = m_gui.GetRenderer()->GetWidth();
    int panel_width = std::min(400, width - 2 * UIStyleGuide::Spacing::LG);
    int panel_x = (width - panel_width) / 2;
    
    // Create QR panel with consistent styling
    Rect qr_bounds = Rect(panel_x, y, panel_width, height);
    m_qr_panel = m_ui_factory->CreatePanel(qr_bounds, PanelStyle::CARD);
    
    // Create QR components with factory
    m_qr_title_label = m_ui_factory->CreateLabel("QR Code", Point(0, 0), LabelStyle::HEADING);
    m_qr_placeholder_label = m_ui_factory->CreateLabel("📱 QR Code will be displayed here\n\nScan with your mobile wallet\nto send Gotham Coin to this address", Point(0, 0), LabelStyle::BODY);
    m_qr_placeholder_label->SetColor(UIStyleGuide::Colors::TEXT_SECONDARY);
    
    // Setup layout for QR panel
    std::vector<LayoutItem> qr_items;
    
    // QR title
    qr_items.push_back(LayoutItem{});
    qr_items.back().constraints.preferred_height = UIStyleGuide::FontSize::HEADING + UIStyleGuide::Spacing::SM;
    qr_items.back().margin = Margin(UIStyleGuide::Spacing::MD);
    qr_items.back().on_bounds_changed = [this](const Rect& bounds) {
        if (m_qr_title_label) {
            m_qr_title_label->SetPosition(Point(bounds.x + bounds.w / 2 - 50, bounds.y));
        }
    };
    
    // QR placeholder (centered)
    qr_items.push_back(LayoutItem{});
    qr_items.back().constraints.weight = 1.0f;
    qr_items.back().margin = Margin(UIStyleGuide::Spacing::LG);
    qr_items.back().on_bounds_changed = [this](const Rect& bounds) {
        if (m_qr_placeholder_label) {
            m_qr_placeholder_label->SetPosition(Point(bounds.x + bounds.w / 2 - 100, bounds.y + bounds.h / 2 - 40));
        }
    };
    
    // Apply vertical layout for QR panel
    m_layout_manager->CreateVerticalLayout(qr_bounds, qr_items, 
                                         UIStyleGuide::Spacing::MD, Alignment::CENTER);
}

void ReceiveScreen::SetupButtonCallbacks()
{
    if (m_back_button) {
        m_back_button->SetOnClick([this]() { 
            m_gui.SwitchScreen(ScreenType::WALLET); 
        });
    }
    
    if (m_copy_button) {
        m_copy_button->SetOnClick([this]() {
            // Copy address to clipboard
            SDL_SetClipboardText(m_current_address.c_str());
            std::cout << "Address copied to clipboard: " << m_current_address << std::endl;
        });
    }
    
    if (m_new_address_button) {
        m_new_address_button->SetOnClick([this]() {
            GenerateNewAddress();
            if (m_address_label) {
                m_address_label->SetText(m_current_address);
            }
            std::cout << "Generated new address: " << m_current_address << std::endl;
        });
    }
}

void ReceiveScreen::OnBackClicked()
{
    m_gui.SwitchScreen(ScreenType::WALLET);
}

void ReceiveScreen::OnCopyClicked()
{
    std::cout << "Address copied to clipboard: " << m_current_address << std::endl;
}

void ReceiveScreen::OnNewAddressClicked()
{
    GenerateNewAddress();
    std::cout << "New address generated: " << m_current_address << std::endl;
}



