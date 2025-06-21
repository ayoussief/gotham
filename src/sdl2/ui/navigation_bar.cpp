// Copyright (c) 2025 The Gotham Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or https://opensource.org/license/mit/.

#include "navigation_bar.h"
#include "ui_factory.h"
#include "layout_manager.h"
#include "ui_style_guide.h"
#include "../utils/font_manager.h"
#include <iostream>

NavigationBar::NavigationBar(NavigationManager& nav_manager)
    : m_nav_manager(nav_manager)
{
}

bool NavigationBar::Initialize(UIFactory& ui_factory, LayoutManager& layout_manager)
{
    m_ui_factory = &ui_factory;
    m_layout_manager = &layout_manager;
    
    CreateComponents();
    
    m_initialized = true;
    return true;
}

void NavigationBar::HandleEvent(const SDL_Event& event)
{
    if (!m_initialized) return;
    
    // Handle back button
    if (m_back_button && m_show_back_button) {
        m_back_button->HandleEvent(event);
    }
    
    // Handle contextual action buttons
    for (auto& button : m_contextual_buttons) {
        if (button) {
            button->HandleEvent(event);
        }
    }
}

void NavigationBar::Update(float delta_time)
{
    if (!m_initialized) return;
    
    // Update components
    if (m_back_button) {
        m_back_button->Update(delta_time);
    }
    
    for (auto& button : m_contextual_buttons) {
        if (button) {
            button->Update(delta_time);
        }
    }
}

void NavigationBar::Render(Renderer& renderer, FontManager& font_manager)
{
    if (!m_initialized) return;
    
    // Render background
    if (m_background_panel) {
        m_background_panel->Render(renderer);
    }
    
    // Get fonts
    TTF_Font* title_font = font_manager.GetDefaultFont(UIStyleGuide::FontSize::HEADING);
    TTF_Font* breadcrumb_font = font_manager.GetDefaultFont(UIStyleGuide::FontSize::CAPTION);
    TTF_Font* button_font = font_manager.GetDefaultFont(UIStyleGuide::FontSize::BODY);
    
    // Render back button
    if (m_back_button && m_show_back_button && button_font) {
        m_back_button->Render(renderer, button_font);
    }
    
    // Render title
    if (m_title_label && title_font) {
        m_title_label->Render(renderer, title_font);
    }
    
    // Render breadcrumbs
    if (m_breadcrumb_label && m_show_breadcrumbs && breadcrumb_font) {
        m_breadcrumb_label->Render(renderer, breadcrumb_font);
    }
    
    // Render contextual buttons
    for (auto& button : m_contextual_buttons) {
        if (button && button_font) {
            button->Render(renderer, button_font);
        }
    }
}

void NavigationBar::SetBounds(const Rect& bounds)
{
    m_bounds = bounds;
    UpdateLayout();
}

void NavigationBar::UpdateNavigationState()
{
    if (!m_initialized) return;
    
    UpdateBackButton();
    UpdateBreadcrumbs();
    UpdateTitle();
    UpdateLayout();
}

void NavigationBar::AddContextualAction(const std::string& text, std::function<void()> callback)
{
    if (!m_ui_factory) return;
    
    auto button = m_ui_factory->CreateButton(text, Rect(0, 0, 100, UIStyleGuide::Dimensions::BUTTON_HEIGHT), 
                                           ButtonStyle::SECONDARY);
    
    button->SetOnClick([callback]() {
        if (callback) callback();
    });
    
    m_contextual_buttons.push_back(std::move(button));
    UpdateLayout();
}

void NavigationBar::ClearContextualActions()
{
    m_contextual_buttons.clear();
    UpdateLayout();
}

void NavigationBar::SetNavigationCallback(std::function<void(ScreenType)> callback)
{
    m_navigation_callback = callback;
}

void NavigationBar::CreateComponents()
{
    if (!m_ui_factory) return;
    
    // Create background panel
    m_background_panel = m_ui_factory->CreatePanel(m_bounds, PanelStyle::HEADER);
    
    // Create back button
    m_back_button = m_ui_factory->CreateButton("← Back", Rect(0, 0, 80, UIStyleGuide::Dimensions::BUTTON_HEIGHT), 
                                             ButtonStyle::GHOST);
    m_back_button->SetOnClick([this]() { OnBackButtonClicked(); });
    
    // Create title label
    m_title_label = m_ui_factory->CreateLabel("", Point(0, 0), LabelStyle::TITLE);
    
    // Create breadcrumb label
    m_breadcrumb_label = m_ui_factory->CreateLabel("", Point(0, 0), LabelStyle::CAPTION);
    m_breadcrumb_label->SetColor(UIStyleGuide::Colors::TEXT_SECONDARY);
}

void NavigationBar::UpdateLayout()
{
    if (!m_layout_manager || m_bounds.w <= 0 || m_bounds.h <= 0) return;
    
    std::vector<LayoutItem> items;
    int item_index = 0;
    
    // Back button (if shown and can go back)
    if (m_show_back_button && m_nav_manager.CanGoBack()) {
        LayoutItem back_item;
        back_item.constraints.preferred_width = 80;
        back_item.margin = Margin(UIStyleGuide::Spacing::SM);
        back_item.on_bounds_changed = [this](const Rect& bounds) {
            if (m_back_button) {
                m_back_button->SetBounds(bounds);
            }
        };
        items.push_back(back_item);
        item_index++;
    }
    
    // Title and breadcrumbs (flexible space)
    LayoutItem title_item;
    title_item.constraints.weight = 1.0f;
    title_item.margin = Margin(UIStyleGuide::Spacing::SM, UIStyleGuide::Spacing::MD);
    title_item.on_bounds_changed = [this](const Rect& bounds) {
        if (m_title_label) {
            // Position title at top of bounds
            m_title_label->SetPosition(Point(bounds.x, bounds.y + UIStyleGuide::Spacing::SM));
        }
        if (m_breadcrumb_label && m_show_breadcrumbs) {
            // Position breadcrumbs below title
            m_breadcrumb_label->SetPosition(Point(bounds.x, bounds.y + UIStyleGuide::Spacing::SM + 24));
        }
    };
    items.push_back(title_item);
    
    // Contextual action buttons
    for (size_t i = 0; i < m_contextual_buttons.size(); ++i) {
        LayoutItem action_item;
        action_item.constraints.preferred_width = 100;
        action_item.margin = Margin(UIStyleGuide::Spacing::SM);
        action_item.on_bounds_changed = [this, i](const Rect& bounds) {
            if (i < m_contextual_buttons.size() && m_contextual_buttons[i]) {
                m_contextual_buttons[i]->SetBounds(bounds);
            }
        };
        items.push_back(action_item);
    }
    
    // Apply horizontal layout
    if (!items.empty()) {
        m_layout_manager->CreateHorizontalLayout(m_bounds, items, UIStyleGuide::Spacing::SM, Alignment::CENTER);
    }
    
    // Update background panel bounds
    if (m_background_panel) {
        m_background_panel->SetBounds(m_bounds);
    }
}

void NavigationBar::UpdateBreadcrumbs()
{
    if (!m_breadcrumb_label) return;
    
    std::string breadcrumb_text = BreadcrumbBuilder::BuildBreadcrumbText(m_nav_manager, " › ");
    m_breadcrumb_label->SetText(breadcrumb_text);
}

void NavigationBar::UpdateBackButton()
{
    if (!m_back_button) return;
    
    bool can_go_back = m_nav_manager.CanGoBack();
    m_back_button->SetEnabled(can_go_back);
    
    if (can_go_back) {
        ScreenType prev_screen = m_nav_manager.GetPreviousScreen();
        std::string back_text = "← " + m_nav_manager.GetScreenTitle(prev_screen);
        m_back_button->SetText(back_text);
    } else {
        m_back_button->SetText("← Back");
    }
}

void NavigationBar::UpdateTitle()
{
    if (!m_title_label) return;
    
    ScreenType current_screen = m_nav_manager.GetCurrentScreen();
    std::string title = m_nav_manager.GetScreenTitle(current_screen);
    m_title_label->SetText(title);
}

void NavigationBar::OnBackButtonClicked()
{
    if (m_nav_manager.CanGoBack()) {
        m_nav_manager.NavigateBack();
    }
}

void NavigationBar::OnBreadcrumbClicked(ScreenType screen_type)
{
    if (m_navigation_callback) {
        m_navigation_callback(screen_type);
    }
}

// NavigationContextMenu implementation
NavigationContextMenu::NavigationContextMenu(NavigationManager& nav_manager)
    : m_nav_manager(nav_manager)
{
}

void NavigationContextMenu::Show(const Point& position)
{
    m_position = position;
    m_visible = true;
    CreateMenuItems();
    UpdateMenuLayout();
}

void NavigationContextMenu::Hide()
{
    m_visible = false;
    m_menu_items.clear();
}

void NavigationContextMenu::HandleEvent(const SDL_Event& event)
{
    if (!m_visible) return;
    
    // Handle menu item clicks
    for (auto& item : m_menu_items) {
        if (item) {
            item->HandleEvent(event);
        }
    }
    
    // Hide on click outside (simplified)
    if (event.type == SDL_MOUSEBUTTONDOWN) {
        // TODO: Implement proper bounds checking
        Hide();
    }
}

void NavigationContextMenu::Render(Renderer& renderer, FontManager& font_manager)
{
    if (!m_visible) return;
    
    // Render menu panel
    if (m_menu_panel) {
        m_menu_panel->Render(renderer);
    }
    
    // Render menu items
    TTF_Font* menu_font = font_manager.GetDefaultFont(UIStyleGuide::FontSize::BODY);
    for (auto& item : m_menu_items) {
        if (item && menu_font) {
            item->Render(renderer, menu_font);
        }
    }
}

void NavigationContextMenu::CreateMenuItems()
{
    m_menu_items.clear();
    
    // Add navigation history items
    auto breadcrumbs = m_nav_manager.GetBreadcrumbs();
    for (const auto& crumb : breadcrumbs) {
        if (!crumb.can_go_back) continue;
        
        // TODO: Create menu item button
        // This would require a proper menu item component
    }
}

void NavigationContextMenu::UpdateMenuLayout()
{
    // TODO: Implement menu layout
    // This would position menu items vertically
}