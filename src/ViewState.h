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

#include "Point.h"
#include "Vector.h"
#include "SDL_include.h"

/**
 * ViewState — groups all globals that describe the current map viewport.
 *
 * Kept separate from UILayout (panel geometry) and AppWindows (window objects).
 * renderTileSize and miniTileSize are NOT included here because w_util.cpp
 * references them by their old names and cannot be modified.
 */
struct ViewState
{
    MPoint<int> mapOffset{};           // pixel offset of the map viewport origin
    MPoint<int> tilePointedAt{};       // tile coordinate under the cursor
    SDL_Rect    tileHighlight{ 0, 0, 16, 16 }; // highlight rect in screen coords
    SDL_Rect    fullMapViewRect{};     // source rect used for SDL_RenderCopy of the map
    Vector<int> windowSize{};          // current SDL window size in pixels
    Vector<int> draggableToolVector{}; // current drag vector for line tools
    Vector<int> lastResize{};          // size from the most recent resize event
};

extern ViewState viewState;
