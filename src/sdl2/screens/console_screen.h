// Copyright (c) 2025 The Gotham Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or https://opensource.org/license/mit/.

#ifndef GOTHAM_SDL2_SCREENS_CONSOLE_SCREEN_H
#define GOTHAM_SDL2_SCREENS_CONSOLE_SCREEN_H

#include "../ui/screen.h"

class ConsoleScreen : public Screen
{
public:
    explicit ConsoleScreen(GothamCityGUI& gui);
    bool Initialize() override;
    void HandleEvent(const SDL_Event& event) override;
    void Update(float delta_time) override;
    void Render(Renderer& renderer) override;
};

#endif // GOTHAM_SDL2_SCREENS_CONSOLE_SCREEN_H