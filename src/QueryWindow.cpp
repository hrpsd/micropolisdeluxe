// This file is part of Micropolis-SDL2PP
// Micropolis-SDL2PP is based on Micropolis
//
// Copyright © 2022 - 2024 Leeor Dicker
// Copyright © 2025 - 2026 Sylvain Nowé
//
// Portions Copyright � 1989-2007 Electronic Arts Inc.
//
// Micropolis-SDL2PP is free software; you can redistribute it and/or modify
// it under the terms of the GNU GPLv3, with additional terms. See the README
// file, included in this distribution, for details.

#include "QueryWindow.h"
#include "FontSet.h"

#include "main.h"

#include "w_resrc.h"

#include <array>

namespace
{
    constexpr SDL_Rect BgRect{ 0, 0, 192, 160 };
    constexpr SDL_Rect CloseButtonRect{ 61, 221, 142, 20 };
};


QueryWindow::QueryWindow(SDL_Renderer* renderer) :
    mRenderer{ renderer },
    mStringRenderer{ renderer }
{
    size({ (int)roundf(BgRect.w * scale), (int)roundf(BgRect.h * scale) });

    closeButtonActive(false);
}

void QueryWindow::setQueryResult(ZoneStats& stats, MPoint<int> mousePosition)
{
    this->stats = &stats;
    this->mousePosition = mousePosition;
}

void QueryWindow::draw()
{
    size({ (int)roundf(BgRect.w * scale), (int)roundf(BgRect.h * scale) });
    int dx = (FullMapViewportRect().x + FullMapViewportRect().w - mousePosition.x > (BgRect.w + 32) * scale) ? (int)roundf(32 * scale) : (int)roundf(-BgRect.w * scale - 32 * scale);
    int dy = (FullMapViewportRect().y + FullMapViewportRect().h - mousePosition.y > (BgRect.h + 32) * scale) ? (int)roundf(32 * scale) : (int)roundf(-BgRect.h * scale - 32 * scale);
    MPoint<int> mousePos = mousePosition + Vector<int>{ dx, dy };
    
    SDL_SetRenderDrawColor(mainWindowRenderer, 255, 255, 255, 255);
    SDL_Rect bgRect = globalScaleRect(BgRect);
    bgRect.x = mousePosition.x + dx; bgRect.y = mousePosition.y + dy;
    SDL_RenderFillRect(mainWindowRenderer, &bgRect);
    SDL_SetRenderDrawColor(mainWindowRenderer, 0, 0, 0, 255);
    SDL_RenderDrawRect(mainWindowRenderer, &bgRect);
    
    const int titleX = mousePos.x + size().x / 2 - fonts.largeTitle->width(stats->title) / 2;

    SDL_SetTextureColorMod(fonts.body->texture(), 0, 0, 0);
    SDL_SetTextureColorMod(fonts.title->texture(), 0, 0, 0);
    SDL_SetTextureColorMod(fonts.largeTitle->texture(), 0, 0, 0);
    mStringRenderer.drawString(*fonts.largeTitle, stats->title, { titleX, mousePos.y + (int)roundf(10 * scale) });

    const std::array<std::tuple<std::string, std::string>, 5> queryStats =
    {
        std::tuple{ "Density", stats->density },
        std::tuple{ "Land Value", stats->landValue },
        std::tuple{ "Pollution Level", stats->pollution },
        std::tuple{ "Crime", stats->crime },
        std::tuple{ "Growth", stats->populationGrowth },
    };

    MPoint<int> startPoint = { mousePos.x + (int)roundf(10 * scale), mousePos.y + (int)roundf(35 * scale) };

    int row = 0;
    for (const auto& [title, value] : queryStats)
    {
        const MPoint<int> titleDrawOrigin = startPoint + Vector<int>{ 0, fonts.body->height()* row + (int)roundf(10 * scale) };
        mStringRenderer.drawString(*fonts.title, title, titleDrawOrigin);

        const MPoint<int> valueDrawOrigin = { mousePos.x + size().x - (int)roundf(10 * scale) - fonts.body->width(value), titleDrawOrigin.y };
        mStringRenderer.drawString(*fonts.body, value, valueDrawOrigin);

        ++row;
    }
}


void QueryWindow::onMouseDown(const MPoint<int>& pt)
{
    const MRectangle<int> closeButtonRect =
    {
        area().x + CloseButtonRect.x,
        area().y + CloseButtonRect.y,
        CloseButtonRect.w,
        CloseButtonRect.h
    };
    
    if (closeButtonRect.contains(pt))
    {
        hide();
    }
}
