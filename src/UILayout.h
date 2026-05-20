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

#include "SDL_include.h"

/**
 * UILayout — groups all globals that describe fixed UI panel geometry.
 *
 * These rects are computed once in initViewParameters() / windowSize() and
 * used by UISetup.cpp, MouseHandler.cpp, and the draw functions.
 */
struct UILayout
{
    SDL_Rect headerRect{};                    // top-left info header panel
    SDL_Rect statusRect{};                    // city status panel below the header
    SDL_Rect rciDestination{};                // RCI demand indicator position
    SDL_Rect residentialValve{ 0, 0, 4, 0 }; // R demand bar
    SDL_Rect commercialValve{ 0, 0, 4, 0 };  // C demand bar
    SDL_Rect industrialValve{ 0, 0, 4, 0 };  // I demand bar
};

extern UILayout uiLayout;
