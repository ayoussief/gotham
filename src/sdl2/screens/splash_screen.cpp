// Copyright (c) 2025 The Gotham Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or https://opensource.org/license/mit/.

#include "splash_screen.h"
#include "../gotham_city_gui.h"
#include "../utils/theme.h"
#include "../utils/font_manager.h"
#include <cmath>
#include <iostream>

SplashScreen::SplashScreen(GothamCityGUI& gui)
    : Screen(gui)
{
    // Initialize UI systems
    if (auto* theme_manager = m_gui.GetThemeManager()) {
        m_ui_factory = std::make_unique<UIFactory>(*theme_manager);
    }
    m_layout_manager = std::make_unique<LayoutManager>();
}

bool SplashScreen::Initialize()
{
    if (m_initialized) {
        return true;
    }

    CreateLayout();

    m_initialized = true;
    std::cout << "Splash screen initialized" << std::endl;
    return true;
}

void SplashScreen::CreateLayout()
{
    CreateLabels();
}

void SplashScreen::CreateLabels()
{
    if (!m_ui_factory || !m_layout_manager) return;
    
    // Get current screen dimensions
    int screen_width = m_gui.GetRenderer()->GetWidth();
    int screen_height = m_gui.GetRenderer()->GetHeight();
    int center_x = screen_width / 2;
    int center_y = screen_height / 2;

    // Create a centered content container using layout manager
    int content_width = std::min(600, screen_width - UIStyleGuide::Spacing::XL * 2);
    int content_height = 300;
    Rect content_container = Rect(
        center_x - content_width / 2,
        center_y - content_height / 2,
        content_width,
        content_height
    );

    // Create labels with proper centering
    m_title_label = m_ui_factory->CreateLabel("🦇 GOTHAM CITY", Point(center_x, center_y - 80), LabelStyle::TITLE);
    m_title_label->SetAlignment(TextAlignment::CENTER);
    m_title_label->SetColor(UIStyleGuide::Colors::GOTHAM_GOLD);

    m_subtitle_label = m_ui_factory->CreateLabel("Cryptocurrency Wallet & Node", Point(center_x, center_y - 30), LabelStyle::HEADING);
    m_subtitle_label->SetAlignment(TextAlignment::CENTER);
    m_subtitle_label->SetColor(UIStyleGuide::Colors::TEXT_SECONDARY);

    m_status_label = m_ui_factory->CreateLabel(m_loading_status, Point(center_x, center_y + 60), LabelStyle::BODY);
    m_status_label->SetAlignment(TextAlignment::CENTER);
    m_status_label->SetColor(UIStyleGuide::Colors::TEXT_SECONDARY);
    
    // Create progress bar with responsive width and proper centering
    int progress_width = std::min(400, screen_width - UIStyleGuide::Spacing::XL * 2);
    int progress_height = 8;
    Rect progress_bounds = Rect(
        center_x - progress_width / 2, 
        center_y + 100, 
        progress_width, 
        progress_height
    );
    
    m_progress_bar = std::make_unique<ProgressBar>(progress_bounds);
    m_progress_bar->SetBackgroundColor(UIStyleGuide::Colors::SURFACE_DARK);
    m_progress_bar->SetForegroundColor(UIStyleGuide::Colors::GOTHAM_GOLD);
    m_progress_bar->SetBorderColor(UIStyleGuide::Colors::BORDER_DARK);
    
    // Use layout manager to ensure proper vertical spacing
    std::vector<LayoutItem> content_items(4);
    
    // Title
    content_items[0].constraints.preferred_height = 40;
    content_items[0].margin = Margin(UIStyleGuide::Spacing::MD);
    content_items[0].on_bounds_changed = [this, center_x](const Rect& bounds) {
        if (m_title_label) {
            m_title_label->SetPosition(Point(center_x, bounds.y + bounds.h / 2));
        }
    };
    
    // Subtitle
    content_items[1].constraints.preferred_height = 30;
    content_items[1].margin = Margin(UIStyleGuide::Spacing::SM);
    content_items[1].on_bounds_changed = [this, center_x](const Rect& bounds) {
        if (m_subtitle_label) {
            m_subtitle_label->SetPosition(Point(center_x, bounds.y + bounds.h / 2));
        }
    };
    
    // Status
    content_items[2].constraints.preferred_height = 25;
    content_items[2].margin = Margin(UIStyleGuide::Spacing::LG);
    content_items[2].on_bounds_changed = [this, center_x](const Rect& bounds) {
        if (m_status_label) {
            m_status_label->SetPosition(Point(center_x, bounds.y + bounds.h / 2));
        }
    };
    
    // Progress bar
    content_items[3].constraints.preferred_height = progress_height + UIStyleGuide::Spacing::SM;
    content_items[3].margin = Margin(UIStyleGuide::Spacing::MD);
    content_items[3].on_bounds_changed = [this, progress_width](const Rect& bounds) {
        if (m_progress_bar) {
            int center_x = bounds.x + bounds.w / 2;
            Rect new_bounds = Rect(center_x - progress_width / 2, bounds.y, progress_width, 8);
            m_progress_bar->SetBounds(new_bounds);
        }
    };
    
    // Apply vertical layout to center content properly
    m_layout_manager->CreateVerticalLayout(content_container, content_items, 
                                         UIStyleGuide::Spacing::MD, Alignment::CENTER);
}

void SplashScreen::HandleEvent(const SDL_Event& event)
{
    // Splash screen doesn't handle events, just displays loading
}

void SplashScreen::Update(float delta_time)
{
    m_elapsed_time += delta_time;
    
    // Fade in effect
    if (m_fade_alpha < 1.0f) {
        m_fade_alpha = std::min(1.0f, m_fade_alpha + delta_time * 2.0f);
    }
    
    // Logo pulsing effect
    m_logo_scale = 0.8f + 0.1f * std::sin(m_elapsed_time * 2.0f);
    
    // Update loading progress
    UpdateLoadingProgress(delta_time);
    
    // Switch to main screen after loading is complete
    if (m_loading_complete && m_elapsed_time > 3.0f) {
        m_gui.SwitchScreen(ScreenType::MAIN);
    }
}

void SplashScreen::Render(Renderer& renderer)
{
    // Render background with consistent theme
    renderer.Clear(UIStyleGuide::Colors::BACKGROUND_DARK);
    
    // Render background effects
    RenderBackground(renderer);
    
    // Render logo area
    RenderLogo(renderer);
    
    // Render text with fade effect
    FontManager* font_manager = m_gui.GetFontManager();
    if (font_manager) {
        // Title
        TTF_Font* title_font = font_manager->GetDefaultFont(48);
        if (title_font && m_title_label) {
            Color title_color = UIStyleGuide::Colors::GOTHAM_GOLD;
            title_color.a = static_cast<Uint8>(255 * m_fade_alpha);
            m_title_label->SetColor(title_color);
            m_title_label->Render(renderer, title_font);
        }
        
        // Subtitle
        TTF_Font* subtitle_font = font_manager->GetDefaultFont(20);
        if (subtitle_font && m_subtitle_label) {
            Color subtitle_color = UIStyleGuide::Colors::TEXT_SECONDARY;
            subtitle_color.a = static_cast<Uint8>(255 * m_fade_alpha);
            m_subtitle_label->SetColor(subtitle_color);
            m_subtitle_label->Render(renderer, subtitle_font);
        }
        
        // Status
        TTF_Font* status_font = font_manager->GetDefaultFont(16);
        if (status_font && m_status_label) {
            Color status_color = UIStyleGuide::Colors::TEXT_SECONDARY;
            status_color.a = static_cast<Uint8>(255 * m_fade_alpha);
            m_status_label->SetColor(status_color);
            m_status_label->SetText(m_loading_status);
            m_status_label->Render(renderer, status_font);
        }
    }
    
    // Render progress bar with consistent styling
    if (m_progress_bar) {
        m_progress_bar->SetProgress(m_loading_progress);
        m_progress_bar->Render(renderer);
    }
}

void SplashScreen::OnActivate()
{
    std::cout << "Splash screen activated" << std::endl;
    m_elapsed_time = 0.0f;
    m_fade_alpha = 0.0f;
    m_loading_progress = 0.0f;
    m_loading_complete = false;
}

void SplashScreen::OnResize(int new_width, int new_height)
{
    // Recreate labels with new positions based on new screen size
    std::cout << "Splash screen resizing to: " << new_width << "x" << new_height << std::endl;
    CreateLabels();
}

void SplashScreen::UpdateLoadingProgress(float delta_time)
{
    if (m_loading_complete) {
        return;
    }
    
    // Simulate loading progress
    m_loading_progress += delta_time * 0.3f; // 30% per second
    
    if (m_loading_progress >= 1.0f) {
        m_loading_progress = 1.0f;
        m_loading_complete = true;
        m_loading_status = "Ready to launch!";
    } else if (m_loading_progress > 0.8f) {
        m_loading_status = "Finalizing setup...";
    } else if (m_loading_progress > 0.6f) {
        m_loading_status = "Loading wallet data...";
    } else if (m_loading_progress > 0.4f) {
        m_loading_status = "Connecting to network...";
    } else if (m_loading_progress > 0.2f) {
        m_loading_status = "Initializing blockchain...";
    }
    
    // Update status label text
    if (m_status_label) {
        m_status_label->SetText(m_loading_status);
    }
}

void SplashScreen::RenderBackground(Renderer& renderer)
{
    // Background already cleared with consistent theme color
    
    // Add some atmospheric effects
    int width = renderer.GetWidth();
    int height = renderer.GetHeight();
    
    // Subtle gradient overlay
    for (int y = 0; y < height; y += 4) {
        float gradient = static_cast<float>(y) / height;
        Color line_color = UIStyleGuide::Colors::GOTHAM_DARK_BLUE;
        line_color.r = std::min(255, static_cast<int>(line_color.r + gradient * 20));
        line_color.g = std::min(255, static_cast<int>(line_color.g + gradient * 20));
        line_color.b = std::min(255, static_cast<int>(line_color.b + gradient * 30));
        line_color.a = static_cast<Uint8>(50 * m_fade_alpha);
        
        renderer.SetDrawColor(line_color);
        renderer.DrawLine(Point(0, y), Point(width, y));
    }
}

void SplashScreen::RenderLogo(Renderer& renderer)
{
    int center_x = renderer.GetWidth() / 2;
    int center_y = renderer.GetHeight() / 2;
    
    // Draw a stylized bat symbol placeholder
    // In a real implementation, this would load an actual logo image
    
    int logo_size = static_cast<int>(100 * m_logo_scale);
    Color logo_color = UIStyleGuide::Colors::GOTHAM_GOLD;
    logo_color.a = static_cast<Uint8>(255 * m_fade_alpha);
    
    renderer.SetDrawColor(logo_color);
    
    // Simple geometric bat shape
    Rect body = {center_x - logo_size/6, center_y - 150 - logo_size/4, logo_size/3, logo_size/2};
    renderer.FillRect(body);
    
    // Wings
    Rect left_wing = {center_x - logo_size/2, center_y - 150 - logo_size/6, logo_size/3, logo_size/4};
    Rect right_wing = {center_x + logo_size/6, center_y - 150 - logo_size/6, logo_size/3, logo_size/4};
    renderer.FillRect(left_wing);
    renderer.FillRect(right_wing);
    
    // Glow effect
    Color glow_color = logo_color;
    glow_color.a = static_cast<Uint8>(50 * m_fade_alpha);
    renderer.SetDrawColor(glow_color);
    
    for (int i = 1; i <= 5; ++i) {
        Rect glow_rect = {
            center_x - logo_size/2 - i*2,
            center_y - 150 - logo_size/2 - i*2,
            logo_size + i*4,
            logo_size + i*4
        };
        renderer.DrawRect(glow_rect);
    }
}

void SplashScreen::RenderLoadingBar(Renderer& renderer)
{
    int center_x = renderer.GetWidth() / 2;
    int center_y = renderer.GetHeight() / 2;
    
    int bar_width = 300;
    int bar_height = 6;
    int bar_x = center_x - bar_width / 2;
    int bar_y = center_y + 120;
    
    // Background bar
    Color bg_color(50, 50, 60, static_cast<Uint8>(200 * m_fade_alpha));
    renderer.SetDrawColor(bg_color);
    Rect bg_rect = {bar_x, bar_y, bar_width, bar_height};
    renderer.FillRect(bg_rect);
    
    // Progress bar
    Color progress_color = UIStyleGuide::Colors::GOTHAM_GOLD;
    progress_color.a = static_cast<Uint8>(255 * m_fade_alpha);
    renderer.SetDrawColor(progress_color);
    
    int progress_width = static_cast<int>(bar_width * m_loading_progress);
    Rect progress_rect = {bar_x, bar_y, progress_width, bar_height};
    renderer.FillRect(progress_rect);
    
    // Progress bar glow
    Color glow_color = progress_color;
    glow_color.a = static_cast<Uint8>(100 * m_fade_alpha);
    renderer.SetDrawColor(glow_color);
    
    Rect glow_rect = {bar_x - 1, bar_y - 1, progress_width + 2, bar_height + 2};
    renderer.DrawRect(glow_rect);
}