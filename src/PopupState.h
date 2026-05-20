// This file is part of Micropolis-SDL2PP
// Micropolis-SDL2PP is based on Micropolis
//
// Copyright © 2022 - 2024 Leeor Dicker
//
// Portions Copyright © 1989-2007 Electronic Arts Inc.
//
// Micropolis-SDL2PP is free software; you can redistribute it and/or modify
// it under the terms of the GNU GPLv3, with additional terms. See the README
// file, included in this distribution, for details.
#pragma once

#include "Point.h"
#include "SDL_include.h"

/**
 * Holds all transient popup/notification state.
 *
 * Member mapping from old globals:
 *   lastId               ← lastPopup
 *   texture              ← popupTexture
 *   title                ← popupTitle
 *   rect                 ← popupRect
 *   buttonRect           ← popupButtonRect
 *   notificationLocation ← notificationLocation
 *   gotoX                ← gotoX
 *   gotoY                ← gotoY
 */
struct PopupState
{
    int          lastId{0};
    SDL_Texture* texture{nullptr};
    char*        title{nullptr};
    SDL_Rect     rect{};
    SDL_Rect     buttonRect{};
    MPoint<int>  notificationLocation{};
    int          gotoX{-1};
    int          gotoY{-1};
};

// The notification popup state previously lived in a global PopupState named
// `popup`. It has moved into PopupWindow (appWindows.popup). PopupState is
// still kept here because HomeWindow uses it for its scenario-pitch struct.
