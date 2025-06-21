// Copyright (c) 2025 The Gotham Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or https://opensource.org/license/mit/.

#ifndef GOTHAM_SDL2_UI_SCREEN_H
#define GOTHAM_SDL2_UI_SCREEN_H

#include <SDL2/SDL.h>

class GothamCityGUI;
class Renderer;
struct Rect;

/**
 * Base class for all screens in Gotham City
 */
class Screen
{
public:
    explicit Screen(GothamCityGUI& gui);
    virtual ~Screen() = default;

    /**
     * Initialize the screen
     */
    virtual bool Initialize() = 0;

    /**
     * Handle SDL events
     */
    virtual void HandleEvent(const SDL_Event& event) = 0;

    /**
     * Update screen state
     */
    virtual void Update(float delta_time) = 0;

    /**
     * Render the screen
     */
    virtual void Render(Renderer& renderer) = 0;

    /**
     * Called when screen becomes active
     */
    virtual void OnActivate() {}

    /**
     * Called when screen becomes inactive
     */
    virtual void OnDeactivate() {}

    /**
     * Called when window is resized
     */
    virtual void OnResize(int new_width, int new_height) {}

    /**
     * Set the content area bounds for this screen (persistent layout)
     */
    virtual void SetContentAreaBounds(const struct Rect& bounds) {}

    /**
     * Called when navigating to this screen (persistent layout)
     */
    virtual void OnNavigatedTo(const class NavigationContext& context) {}

    /**
     * Called when navigating away from this screen (persistent layout)
     */
    virtual void OnNavigatedFrom() {}

    /**
     * Save navigation state for restoration
     */
    virtual void SaveNavigationState() {}

    /**
     * Restore navigation state
     */
    virtual void RestoreNavigationState() {}

protected:
    GothamCityGUI& m_gui;
    bool m_initialized{false};
};

#endif // GOTHAM_SDL2_UI_SCREEN_H