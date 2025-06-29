// Copyright (c) 2025 The Gotham Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or https://opensource.org/license/mit/.

#include "persistent_layout.h"
#include "ui_style_guide.h"
#include "screen.h"
#include "../utils/font_manager.h"
#include "../gotham_city_gui.h"  // For ScreenType definition
#include <iostream>

PersistentLayout::PersistentLayout()
    : m_active_screen(ScreenType::MAIN)
{
}

bool PersistentLayout::Initialize(UIFactory& ui_factory, LayoutManager& layout_manager, 
                                NavigationManager& nav_manager)
{
    m_ui_factory = &ui_factory;
    m_layout_manager = &layout_manager;
    m_nav_manager = &nav_manager;
    
    CreatePersistentComponents();
    
    m_initialized = true;
    return true;
}

void PersistentLayout::SetViewportBounds(const Rect& bounds)
{
    m_viewport_bounds = bounds;
    CalculateLayoutBounds();
    UpdateLayout();
}

void PersistentLayout::HandleEvent(const SDL_Event& event)
{
    if (!m_initialized) return;
    
    // NOTE: NavigationBar event handling removed - we use custom header layout
    
    // Handle sidebar navigation
    for (auto& nav_item : m_navigation_items) {
        if (nav_item.button) {
            nav_item.button->HandleEvent(event);
        }
    }
    
    // Handle header actions
    if (m_settings_button) {
        m_settings_button->HandleEvent(event);
    }
    
    for (auto& action_button : m_header_action_buttons) {
        if (action_button) {
            action_button->HandleEvent(event);
        }
    }
    
    // Handle content screen events
    if (m_content_screen) {
        m_content_screen->HandleEvent(event);
    }
}

void PersistentLayout::Update(float delta_time)
{
    if (!m_initialized) return;
    
    // NOTE: NavigationBar update removed - we use custom header layout
    
    // Update navigation buttons
    for (auto& nav_item : m_navigation_items) {
        if (nav_item.button) {
            nav_item.button->Update(delta_time);
        }
    }
    
    // Update header components
    if (m_settings_button) {
        m_settings_button->Update(delta_time);
    }
    
    for (auto& action_button : m_header_action_buttons) {
        if (action_button) {
            action_button->Update(delta_time);
        }
    }
    
    // Update content screen
    if (m_content_screen) {
        m_content_screen->Update(delta_time);
    }
}

void PersistentLayout::RenderPersistentElements(Renderer& renderer, FontManager& font_manager)
{
    if (!m_initialized) return;
    
    // Render header panel
    if (m_header_panel) {
        m_header_panel->Render(renderer);
    }
    
    // NOTE: NavigationBar rendering removed - we use custom header layout
    
    // Render sidebar panel
    if (m_sidebar_panel && m_sidebar_visible) {
        m_sidebar_panel->Render(renderer);
        
        // Render navigation buttons
        TTF_Font* nav_font = font_manager.GetDefaultFont(UIStyleGuide::FontSize::BODY);
        for (auto& nav_item : m_navigation_items) {
            if (nav_item.button && nav_font) {
                nav_item.button->Render(renderer, nav_font);
            }
        }
    }
    
    // Render header components
    TTF_Font* title_font = font_manager.GetDefaultFont(UIStyleGuide::FontSize::HEADING);
    TTF_Font* body_font = font_manager.GetDefaultFont(UIStyleGuide::FontSize::BODY);
    
    if (m_app_title_label && title_font) {
        m_app_title_label->Render(renderer, title_font);
    }
    
    if (m_balance_label && body_font) {
        m_balance_label->Render(renderer, body_font);
    }
    
    if (m_settings_button && body_font) {
        m_settings_button->Render(renderer, body_font);
    }
    
    for (auto& action_button : m_header_action_buttons) {
        if (action_button && body_font) {
            action_button->Render(renderer, body_font);
        }
    }
    
    // Render status bar
    if (m_status_bar_panel && m_status_bar_visible) {
        m_status_bar_panel->Render(renderer);
        
        TTF_Font* status_font = font_manager.GetDefaultFont(UIStyleGuide::FontSize::CAPTION);
        
        if (m_connection_label && status_font) {
            m_connection_label->Render(renderer, status_font);
        }
        
        if (m_block_height_label && status_font) {
            m_block_height_label->Render(renderer, status_font);
        }
        
        if (m_sync_status_label && status_font) {
            m_sync_status_label->Render(renderer, status_font);
        }
    }
}

void PersistentLayout::RenderContentArea(Renderer& renderer)
{
    if (!m_initialized) {
        std::cout << "PersistentLayout not initialized" << std::endl;
        return;
    }
    
    if (!m_content_screen) {
        std::cout << "No content screen to render" << std::endl;
        return;
    }
    
    // Debug: Layout debugging can be enabled if needed
    std::cout << "RENDER DEBUG: Rendering content area: " << m_content_area_bounds.x << "," << m_content_area_bounds.y 
              << " " << m_content_area_bounds.w << "x" << m_content_area_bounds.h << std::endl;
    
    // Save current viewport
    SDL_Rect original_viewport;
    SDL_RenderGetViewport(renderer.GetSDLRenderer(), &original_viewport);
    
    // Set viewport to content area bounds - this translates coordinates
    SDL_Rect content_viewport = {
        m_content_area_bounds.x,
        m_content_area_bounds.y,
        m_content_area_bounds.w,
        m_content_area_bounds.h
    };
    SDL_RenderSetViewport(renderer.GetSDLRenderer(), &content_viewport);
    
    // Set up clipping AFTER viewport change - now using relative coordinates
    SDL_Rect clip_rect = {
        0,  // Relative to viewport
        0,  // Relative to viewport
        m_content_area_bounds.w,
        m_content_area_bounds.h
    };
    
    // Enable clipping to content area (now relative to the viewport)
    SDL_RenderSetClipRect(renderer.GetSDLRenderer(), &clip_rect);
    
    // Render the current screen's content (now at translated coordinates)
    m_content_screen->Render(renderer);
    
    // Restore original viewport
    SDL_RenderSetViewport(renderer.GetSDLRenderer(), &original_viewport);
    
    // Disable clipping
    SDL_RenderSetClipRect(renderer.GetSDLRenderer(), nullptr);
}

void PersistentLayout::SetContentArea(std::unique_ptr<Screen> content_screen)
{
    std::cout << "SetContentArea called with screen: " << (content_screen ? "valid" : "null") << std::endl;
    
    // Save state of previous screen if it exists
    if (m_content_screen) {
        m_content_screen->OnNavigatedFrom();
    }
    
    m_content_screen = std::move(content_screen);
    
    // Initialize new screen with content area bounds
    if (m_content_screen) {
        std::cout << "Initializing content screen with bounds: " << m_content_area_bounds.w << "x" << m_content_area_bounds.h << std::endl;
        m_content_screen->SetContentAreaBounds(m_content_area_bounds);
        m_content_screen->OnResize(m_content_area_bounds.w, m_content_area_bounds.h);
        m_content_screen->OnNavigatedTo(m_nav_manager->GetCurrentContext());
    }
}

void PersistentLayout::OnResize(int new_width, int new_height)
{
    m_viewport_bounds.w = new_width;
    m_viewport_bounds.h = new_height;
    
    CalculateLayoutBounds();
    UpdateLayout();
    
    // Notify content screen of resize
    if (m_content_screen) {
        m_content_screen->SetContentAreaBounds(m_content_area_bounds);
        m_content_screen->OnResize(m_content_area_bounds.w, m_content_area_bounds.h);
    }
}

void PersistentLayout::SetSidebarVisible(bool visible)
{
    m_sidebar_visible = visible;
    CalculateLayoutBounds();
    UpdateLayout();
}

void PersistentLayout::SetStatusBarVisible(bool visible)
{
    m_status_bar_visible = visible;
    CalculateLayoutBounds();
    UpdateLayout();
}

void PersistentLayout::AddNavigationItem(const std::string& text, const std::string& icon, 
                                       ScreenType target_screen, std::function<void()> callback)
{
    if (!m_ui_factory) return;
    
    NavigationItem nav_item;
    nav_item.target_screen = target_screen;
    nav_item.callback = callback;
    
    // Create navigation button
    std::string button_text = icon + " " + text;
    nav_item.button = m_ui_factory->CreateNavigationButton(button_text, Rect(0, 0, 180, 40), false);
    
    // Set click callback
    nav_item.button->SetOnClick([this, target_screen, callback]() {
        OnNavigationItemClicked(target_screen, callback);
    });
    
    m_navigation_items.push_back(std::move(nav_item));
    UpdateLayout();
}

void PersistentLayout::SetActiveNavigationItem(ScreenType screen_type)
{
    m_active_screen = screen_type;
    UpdateNavigationStates();
}

void PersistentLayout::UpdateWalletInfo(const std::string& balance, bool connected)
{
    if (m_balance_label) {
        std::string balance_text = connected ? ("[$] Balance: " + balance) : "[X] Wallet Disconnected";
        m_balance_label->SetText(balance_text);
        
        Color text_color = connected ? UIStyleGuide::Colors::TEXT_PRIMARY : UIStyleGuide::Colors::TEXT_ERROR;
        m_balance_label->SetColor(text_color);
    }
}

void PersistentLayout::UpdateNetworkInfo(int connections, int block_height, const std::string& status)
{
    if (m_connection_label) {
        std::string conn_icon = connections > 0 ? "[o]" : "[x]";
        m_connection_label->SetText(conn_icon + " Connections: " + std::to_string(connections));
    }
    
    if (m_block_height_label) {
        m_block_height_label->SetText("[#] Block: " + std::to_string(block_height));
    }
    
    if (m_sync_status_label) {
        std::string status_icon = "[~]";
        if (status == "Connected" || status == "Synced") status_icon = "[+]";
        else if (status == "Disconnected" || status == "Error") status_icon = "[!]";
        
        m_sync_status_label->SetText(status_icon + " Status: " + status);
    }
}

void PersistentLayout::AddHeaderAction(const std::string& text, std::function<void()> callback)
{
    if (!m_ui_factory) return;
    
    auto action_button = m_ui_factory->CreateButton(text, Rect(0, 0, 80, 30), ButtonStyle::SECONDARY);
    action_button->SetOnClick(callback);
    
    m_header_action_buttons.push_back(std::move(action_button));
    UpdateLayout();
}

void PersistentLayout::ClearHeaderActions()
{
    m_header_action_buttons.clear();
    UpdateLayout();
}

void PersistentLayout::CalculateLayoutBounds()
{
    // Header bounds (top of screen)
    m_header_bounds = Rect(0, 0, m_viewport_bounds.w, UIStyleGuide::Dimensions::HEADER_HEIGHT);
    
    // Status bar bounds (bottom of screen)
    int status_height = m_status_bar_visible ? UIStyleGuide::Dimensions::STATUS_BAR_HEIGHT : 0;
    m_status_bar_bounds = Rect(0, m_viewport_bounds.h - status_height, m_viewport_bounds.w, status_height);
    
    // Sidebar bounds (left side, between header and status bar)
    int sidebar_width = m_sidebar_visible ? UIStyleGuide::Dimensions::SIDEBAR_WIDTH : 0;
    int sidebar_y = m_header_bounds.h;
    int sidebar_height = m_viewport_bounds.h - m_header_bounds.h - status_height;
    m_sidebar_bounds = Rect(0, sidebar_y, sidebar_width, sidebar_height);
    
    // Content area bounds (remaining space)
    int content_x = sidebar_width;
    int content_y = m_header_bounds.h;
    int content_width = m_viewport_bounds.w - sidebar_width;
    int content_height = m_viewport_bounds.h - m_header_bounds.h - status_height;
    m_content_area_bounds = Rect(content_x, content_y, content_width, content_height);
}

void PersistentLayout::CreatePersistentComponents()
{
    if (!m_ui_factory) return;
    
    CreateHeaderComponents();
    CreateSidebarComponents();
    CreateStatusBarComponents();
}

void PersistentLayout::CreateHeaderComponents()
{
    // Create header panel with enhanced styling
    m_header_panel = m_ui_factory->CreatePanel(m_header_bounds, PanelStyle::HEADER);
    
    // NOTE: NavigationBar removed - we handle navigation in our custom header layout
    
    // Create app title with proper styling and ASCII icon
    m_app_title_label = m_ui_factory->CreateLabel("*** GOTHAM CITY ***", Point(0, 0), LabelStyle::TITLE);
    
    // Create balance label with ASCII icon
    m_balance_label = m_ui_factory->CreateLabel("[$] Balance: Loading...", Point(0, 0), LabelStyle::BODY);
    
    // Create settings button with ASCII icon
    m_settings_button = m_ui_factory->CreateButton("[*] Settings", Rect(0, 0, 120, 35), ButtonStyle::SECONDARY);
    m_settings_button->SetOnClick([this]() { OnSettingsClicked(); });
}

void PersistentLayout::CreateSidebarComponents()
{
    // Create sidebar panel
    m_sidebar_panel = m_ui_factory->CreatePanel(m_sidebar_bounds, PanelStyle::SIDEBAR);
    
    // Navigation items will be added via AddNavigationItem()
}

void PersistentLayout::CreateStatusBarComponents()
{
    // Create status bar panel with enhanced styling
    m_status_bar_panel = m_ui_factory->CreatePanel(m_status_bar_bounds, PanelStyle::STATUS);
    
    // Create status labels with ASCII icons
    m_connection_label = m_ui_factory->CreateLabel("[o] Connections: 0", Point(0, 0), LabelStyle::CAPTION);
    m_block_height_label = m_ui_factory->CreateLabel("[#] Block: 0", Point(0, 0), LabelStyle::CAPTION);
    m_sync_status_label = m_ui_factory->CreateLabel("[~] Status: Connecting...", Point(0, 0), LabelStyle::CAPTION);
}

void PersistentLayout::UpdateLayout()
{
    if (!m_layout_manager) return;
    
    // Update panel bounds
    if (m_header_panel) {
        m_header_panel->SetBounds(m_header_bounds);
    }
    
    if (m_sidebar_panel) {
        m_sidebar_panel->SetBounds(m_sidebar_bounds);
    }
    
    if (m_status_bar_panel) {
        m_status_bar_panel->SetBounds(m_status_bar_bounds);
    }
    
    // NOTE: NavigationBar bounds update removed - we use custom header layout
    
    // === PROFESSIONAL HEADER LAYOUT ===
    LayoutHeaderComponents();
    
    // === PROFESSIONAL STATUS BAR LAYOUT ===
    LayoutStatusBarComponents();
    
    // Layout sidebar navigation items
    if (!m_navigation_items.empty() && m_sidebar_visible) {
        std::vector<LayoutItem> nav_items;
        nav_items.resize(m_navigation_items.size());
        
        for (size_t i = 0; i < m_navigation_items.size(); ++i) {
            nav_items[i].constraints.preferred_height = 45;
            nav_items[i].margin = Margin(UIStyleGuide::Spacing::SM);
            nav_items[i].on_bounds_changed = [this, i](const Rect& bounds) {
                if (i < m_navigation_items.size() && m_navigation_items[i].button) {
                    m_navigation_items[i].button->SetBounds(bounds);
                }
            };
        }
        
        // Apply vertical layout to sidebar
        Rect sidebar_content = m_sidebar_bounds;
        sidebar_content.x += UIStyleGuide::Spacing::SM;
        sidebar_content.y += UIStyleGuide::Spacing::MD;
        sidebar_content.w -= 2 * UIStyleGuide::Spacing::SM;
        
        m_layout_manager->CreateVerticalLayout(sidebar_content, nav_items, 
                                             UIStyleGuide::Spacing::SM, Alignment::START);
    }
}

void PersistentLayout::LayoutHeaderComponents()
{
    if (!m_layout_manager) return;
    
    std::cout << "LAYOUT DEBUG: Header bounds: " << m_header_bounds.x << "," << m_header_bounds.y 
              << " " << m_header_bounds.w << "x" << m_header_bounds.h << std::endl;
    
    // === MANUAL PROFESSIONAL LAYOUT ===
    // Let's do a simple but professional 3-column layout manually
    
    int header_padding = UIStyleGuide::Spacing::LG;
    int content_height = m_header_bounds.h - (2 * UIStyleGuide::Spacing::SM);
    int center_y = m_header_bounds.y + (m_header_bounds.h - content_height) / 2;
    
    // === LEFT SECTION: APP TITLE ===
    if (m_app_title_label) {
        int title_width = 200;
        Point title_pos(m_header_bounds.x + header_padding, 
                       center_y + (content_height - UIStyleGuide::FontSize::HEADING) / 2);
        m_app_title_label->SetPosition(title_pos);
        std::cout << "LAYOUT DEBUG: Title positioned at " << title_pos.x << "," << title_pos.y << std::endl;
    }
    
    // === CENTER SECTION: BALANCE ===
    if (m_balance_label) {
        int balance_width = 300;
        int balance_x = m_header_bounds.x + (m_header_bounds.w - balance_width) / 2;
        Point balance_pos(balance_x, 
                         center_y + (content_height - UIStyleGuide::FontSize::BODY) / 2);
        m_balance_label->SetPosition(balance_pos);
        std::cout << "LAYOUT DEBUG: Balance positioned at " << balance_pos.x << "," << balance_pos.y << std::endl;
    }
    
    // === RIGHT SECTION: SETTINGS BUTTON ===
    if (m_settings_button) {
        int button_width = 120;
        int button_height = 35;
        Rect settings_rect(
            m_header_bounds.x + m_header_bounds.w - header_padding - button_width,
            center_y + (content_height - button_height) / 2,
            button_width,
            button_height
        );
        m_settings_button->SetBounds(settings_rect);
        std::cout << "LAYOUT DEBUG: Settings button positioned at " << settings_rect.x << "," << settings_rect.y 
                  << " " << settings_rect.w << "x" << settings_rect.h << std::endl;
    }
    
    // === ACTION BUTTONS (if any) ===
    int next_button_x = m_header_bounds.x + m_header_bounds.w - header_padding - 120 - UIStyleGuide::Spacing::SM;
    for (int i = m_header_action_buttons.size() - 1; i >= 0; i--) {
        if (m_header_action_buttons[i]) {
            int action_width = 90;
            int action_height = 35;
            next_button_x -= action_width;
            
            Rect action_rect(
                next_button_x,
                center_y + (content_height - action_height) / 2,
                action_width,
                action_height
            );
            m_header_action_buttons[i]->SetBounds(action_rect);
            
            next_button_x -= UIStyleGuide::Spacing::SM; // Space between buttons
        }
    }
}

void PersistentLayout::LayoutStatusBarComponents()
{
    if (!m_layout_manager) return;
    
    std::cout << "LAYOUT DEBUG: Status bar bounds: " << m_status_bar_bounds.x << "," << m_status_bar_bounds.y 
              << " " << m_status_bar_bounds.w << "x" << m_status_bar_bounds.h << std::endl;
    
    // === MANUAL PROFESSIONAL STATUS BAR LAYOUT ===
    // Professional 3-section layout: Left, Center, Right distribution
    
    int status_padding = UIStyleGuide::Spacing::MD;
    int label_height = UIStyleGuide::FontSize::CAPTION;
    int center_y = m_status_bar_bounds.y + (m_status_bar_bounds.h - label_height) / 2;
    
    // Calculate available width
    int available_width = m_status_bar_bounds.w - (2 * status_padding);
    int section_width = available_width / 3; // Divide into 3 equal sections
    
    // === LEFT SECTION: CONNECTION STATUS ===
    if (m_connection_label) {
        Point conn_pos(m_status_bar_bounds.x + status_padding, center_y);
        m_connection_label->SetPosition(conn_pos);
        std::cout << "LAYOUT DEBUG: Connection positioned at " << conn_pos.x << "," << conn_pos.y << std::endl;
    }
    
    // === CENTER SECTION: BLOCK HEIGHT ===
    if (m_block_height_label) {
        int center_section_start = m_status_bar_bounds.x + status_padding + section_width;
        int block_text_width = 100; // Approximate width of "Block: XXXXX"
        Point block_pos(center_section_start + (section_width - block_text_width) / 2, center_y);
        m_block_height_label->SetPosition(block_pos);
        std::cout << "LAYOUT DEBUG: Block height positioned at " << block_pos.x << "," << block_pos.y << std::endl;
    }
    
    // === RIGHT SECTION: SYNC STATUS ===
    if (m_sync_status_label) {
        int sync_text_width = 180; // Approximate width of "Status: XXXXX"
        Point sync_pos(
            m_status_bar_bounds.x + m_status_bar_bounds.w - status_padding - sync_text_width, 
            center_y
        );
        m_sync_status_label->SetPosition(sync_pos);
        std::cout << "LAYOUT DEBUG: Sync status positioned at " << sync_pos.x << "," << sync_pos.y << std::endl;
    }
}

void PersistentLayout::UpdateNavigationStates()
{
    for (auto& nav_item : m_navigation_items) {
        bool is_active = (nav_item.target_screen == m_active_screen);
        nav_item.is_active = is_active;
        
        if (nav_item.button) {
            // Update button appearance based on active state
            if (is_active) {
                nav_item.button->SetColors(UIStyleGuide::Colors::PRIMARY, 
                                         UIStyleGuide::Colors::PRIMARY_HOVER, 
                                         UIStyleGuide::Colors::PRIMARY_PRESSED);
            } else {
                nav_item.button->SetColors(UIStyleGuide::Colors::SURFACE, 
                                         UIStyleGuide::Colors::SURFACE_HOVER, 
                                         UIStyleGuide::Colors::SURFACE_PRESSED);
            }
        }
    }
}

void PersistentLayout::OnNavigationItemClicked(ScreenType screen_type, std::function<void()> callback)
{
    // Update active navigation state
    SetActiveNavigationItem(screen_type);
    
    // Execute custom callback if provided
    if (callback) {
        callback();
    }
    
    // Navigate using navigation manager
    if (m_nav_manager) {
        m_nav_manager->NavigateTo(screen_type);
    }
}

void PersistentLayout::OnSettingsClicked()
{
    if (m_nav_manager) {
        m_nav_manager->NavigateToSettingsFromMain();
    }
}

// ResponsivePersistentLayout implementation
ResponsivePersistentLayout::ResponsivePersistentLayout(const LayoutConfiguration& config)
    : m_config(config)
{
}

void ResponsivePersistentLayout::UpdateResponsiveLayout(int screen_width, int screen_height)
{
    ApplyResponsiveRules(screen_width, screen_height);
    OnResize(screen_width, screen_height);
}

void ResponsivePersistentLayout::SetConfiguration(const LayoutConfiguration& config)
{
    m_config = config;
    UpdateLayoutProtected();
}

void ResponsivePersistentLayout::ApplyResponsiveRules(int screen_width, int screen_height)
{
    // Apply responsive rules based on screen size
    if (screen_width < 768) {
        // Mobile: Collapse sidebar
        m_config = LayoutConfiguration::GetMobile();
        SetSidebarVisible(false);
    } else if (screen_width < 1024) {
        // Tablet: Narrow sidebar
        m_config = LayoutConfiguration::GetTablet();
        SetSidebarVisible(true);
    } else {
        // Desktop: Full sidebar
        m_config = LayoutConfiguration::GetDefault();
        SetSidebarVisible(true);
    }
}