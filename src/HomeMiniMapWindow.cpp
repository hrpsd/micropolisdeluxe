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
#include "HomeMiniMapWindow.h"

#include "main.h"

namespace {
    // Minimap size, defined against the home window's 1200x900 background-art
    // design canvas so it can be scaled by the background's own scale factor.
    SDL_Rect bgRect = { 0, 0, 480, 400 };

    constexpr float DesignWidth = 1500.0f;
}

HomeMiniMapWindow::HomeMiniMapWindow(SDL_Renderer* renderer) :
    mRenderer{ renderer }
{
    closeButtonActive(false);
    anchor();
}


void HomeMiniMapWindow::draw()
{
    // Anchor relative to the letterboxed background art, not the full home
    // window, so the preview stays glued to the background frame at any window
    // aspect ratio. The fractions place the top-left within that frame.
    const SDL_Rect homeBg = appWindows.home->backgroundRect();

    // Scale with the letterboxed background (its on-screen width / design
    // width), not the global window `scale`. Because backgroundRect() preserves
    // the 4:3 aspect, this factor keeps the minimap's own aspect intact too.
    const float bgScale = homeBg.w / DesignWidth;

    position({
        homeBg.x + (int)roundf(0.435f * homeBg.w),
        homeBg.y + (int)roundf(0.043f * homeBg.h)
    });
    size({ (int)roundf(bgRect.w * bgScale), (int)roundf(bgRect.h * bgScale) });

    const SDL_Rect dstRect{ area().x, area().y, area().width, area().height };
    SDL_RenderCopy(mRenderer, MainMapTexture.texture, nullptr, &dstRect);
}
