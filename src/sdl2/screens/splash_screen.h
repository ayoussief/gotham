// Copyright (c) 2025 The Gotham Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or https://opensource.org/license/mit/.

#ifndef GOTHAM_SDL2_SCREENS_SPLASH_SCREEN_H
#define GOTHAM_SDL2_SCREENS_SPLASH_SCREEN_H

#include "../ui/screen.h"
#include "../ui/label.h"
#include "../ui/progress_bar.h"
#include "../ui/ui_factory.h"
#include "../ui/layout_manager.h"
#include "../ui/ui_style_guide.h"
#include <memory>

/**
 * Splash screen shown during application startup
 */
class SplashScreen : public Screen
{
public:
    explicit SplashScreen(GothamCityGUI& gui);
    ~SplashScreen() override = default;

    bool Initialize() override;
    void HandleEvent(const SDL_Event& event) override;
    void Update(float delta_time) override;
    void Render(Renderer& renderer) override;
    void OnActivate() override;
    void OnResize(int new_width, int new_height) override;

private:
    // UI Systems
    std::unique_ptr<UIFactory> m_ui_factory;
    std::unique_ptr<LayoutManager> m_layout_manager;
    
    // UI Components
    std::unique_ptr<Label> m_title_label;
    std::unique_ptr<Label> m_subtitle_label;
    std::unique_ptr<Label> m_status_label;
    std::unique_ptr<ProgressBar> m_progress_bar;
    
    float m_elapsed_time{0.0f};
    float m_fade_alpha{0.0f};
    float m_logo_scale{0.8f};
    bool m_loading_complete{false};
    
    // Loading simulation
    float m_loading_progress{0.0f};
    std::string m_loading_status{"Initializing Gotham City..."};
    
    void CreateLayout();
    void CreateLabels();
    void UpdateLoadingProgress(float delta_time);
    void RenderBackground(Renderer& renderer);
    void RenderLogo(Renderer& renderer);
    void RenderLoadingBar(Renderer& renderer);
};

#endif // GOTHAM_SDL2_SCREENS_SPLASH_SCREEN_H