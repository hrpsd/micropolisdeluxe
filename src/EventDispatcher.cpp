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

/**
 * EventDispatcher.cpp
 *
 * Top-level SDL event loop and window-event handling.
 *
 * Responsibilities:
 *   - Owns the EventHandling namespace state variables
 *   - Routes SDL events to the focused handler file
 *   - Handles SDL_WINDOWEVENT (resize, close)
 *   - Owns showWindowAndBringToFront (UI utility used by all sub-handlers)
 *
 * See also:
 *   MouseHandler.cpp   — mouse motion, button events, tool placement
 *   KeyboardHandler.cpp — key events, speed control, window visibility
 */

#include "main.h"

#include "AppWindows.h"
#include "MiniMapWindow.h"
#include "HomeMiniMapWindow.h"
#include "PopupState.h"
#include "Tool.h"

#include "w_util.h"

#include <algorithm>

// ── Forward declarations of handlers defined in sibling files ────────────────
void handleKeyEvent(SDL_Event& event);
void handleMouseEvent(SDL_Event& event);
void calculateMouseToWorld();
// ────────────────────────────────────────────────────────────────────────────


namespace EventHandling
{
    MPoint<int> MouseDownPosition{};
    MPoint<int> MouseClickPosition{};
    MPoint<int> MousePosition{};

    bool MouseLeftDown{false};
}


void showWindowAndBringToFront(WindowBase& window)
{
    window.show();
    if (&window == appWindows.home.get() || !appWindows.home->visible() || &window == appWindows.homeMiniMap.get() || &window == savedGamesWindow.get() || &window == appWindows.about.get())
    {
        guiWindowStack.bringToFront(&window);
    }
    if (window.visible()) { window.update(); }
    if (&window != appWindows.status.get() && &window != appWindows.messages.get())
        centerWindow(window);
}


// ── Window events ────────────────────────────────────────────────────────────

static void handleWindowEvent(SDL_Event& event)
{
    switch (event.window.event)
    {
        case SDL_WINDOWEVENT_RESIZED: {
            Vector<int> newResize = Vector<int>{event.window.data1, event.window.data2};
            windowResized(newResize);
            viewState.lastResize = newResize;
        } break;

    case SDL_WINDOWEVENT_CLOSE:
        if (event.window.windowID == mainWindowId)
        {
            simExit();
        }
        break;

    default:
        break;
    }
}


// ── Main event pump ──────────────────────────────────────────────────────────

// Touch-pan state (local to the dispatcher)
static MPoint<float> panStart;
static int numFingers{0};
static bool setPanStart{false};

void pumpEvents()
{
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        appWindows.gameMiniMap->injectEvent(event);

        switch (event.type)
        {
        case SDL_KEYDOWN:
            handleKeyEvent(event);
            break;

        case SDL_MOUSEMOTION:
        case SDL_MOUSEBUTTONDOWN:
        case SDL_MOUSEBUTTONUP:
        case SDL_MOUSEWHEEL:
            handleMouseEvent(event);
            break;

        case SDL_QUIT:
            simExit();
            break;

        case SDL_WINDOWEVENT:
            handleWindowEvent(event);
            break;

        case SDL_MULTIGESTURE:
        {
            SDL_MultiGestureEvent mge = event.mgesture;
            if (mge.numFingers == 2)
            {
                if (setPanStart)
                {
                    panStart = { mge.x, mge.y };
                }
                else
                {
                    viewState.mapOffset -= (MPoint { mge.x, mge.y } - panStart)
                        .skewBy(Vector { (float)FullMapViewportRect().w, (float)FullMapViewportRect().h });
                    clampViewOffset();
                    updateMapDrawParameters();
                    calculateMouseToWorld();
                    if (pendingTool() == Tool::Query)
                    {
                        queryTool(viewState.tilePointedAt.x, viewState.tilePointedAt.y, budget);
                        appWindows.query->setQueryResult(queryResult(), EventHandling::MousePosition);
                    }
                    panStart = { mge.x, mge.y };
                }
            }
            setPanStart = false;
            break;
        }

        case SDL_FINGERDOWN:
            numFingers++;
            if (numFingers == 2)
            {
                setPanStart = true;
            }
            break;

        case SDL_FINGERUP:
            numFingers--;
            setPanStart = false;
            break;

        default:
            break;
        }
    }
}
