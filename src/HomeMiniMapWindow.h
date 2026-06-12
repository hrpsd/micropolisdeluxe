// This file is part of Micropolis-SDL2PP
// Micropolis-SDL2PP is based on Micropolis
//
// Copyright © 2022 - 2024 Leeor Dicker
// Copyright © 2025 - 2026 Sylvain Nowé
//
// Portions Copyright © 1989-2007 Electronic Arts Inc.
//
// Micropolis-SDL2PP is free software; you can redistribute it and/or modify
// it under the terms of the GNU GPLv3, with additional terms. See the README
// file, included in this distribution, for details.
#pragma once

#include "WindowBase.h"

#include "SDL_include.h"

// A lightweight, anchored preview shown on the home screen that renders the
// scaled-down MainMapTexture. Sized to 1/5 of the home window by the caller,
// so it scales with the global `scale`.
class HomeMiniMapWindow : public WindowBase
{
public:
    HomeMiniMapWindow() = delete;
    HomeMiniMapWindow(const HomeMiniMapWindow&) = delete;
    const HomeMiniMapWindow& operator=(const HomeMiniMapWindow&) = delete;

    explicit HomeMiniMapWindow(SDL_Renderer* renderer);
    ~HomeMiniMapWindow() override = default;

    void draw() override;
    void update() override {}

private:
    SDL_Renderer* mRenderer{ nullptr };
};
