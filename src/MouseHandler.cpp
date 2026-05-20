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
 * MouseHandler.cpp
 *
 * Mouse motion, button-press/release and tile coordinate mapping.
 *
 * Responsibilities:
 *   - Translate screen pixel position to tile coordinates (calculateMouseToWorld)
 *   - Route SDL_MOUSEMOTION / SDL_MOUSEBUTTONDOWN / SDL_MOUSEBUTTONUP events
 *   - Decide when a tool action should be triggered or ignored
 *   - Handle right-button drag (map pan) and left-button tool placement
 *
 * Does NOT contain: keyboard events, window visibility helpers, speed control.
 * See KeyboardHandler.cpp for those.
 */

#include "main.h"

#include "AppWindows.h"
#include "PopupState.h"
#include "MiniMapWindow.h"
#include "Tool.h"

#include "w_sound.h"
#include "w_util.h"

#include <algorithm>


// ── Tile-coordinate mapping ──────────────────────────────────────────────────

void calculateMouseToWorld()
{
    const auto screenCell = PositionToCell(EventHandling::MousePosition, viewState.mapOffset);

    viewState.tilePointedAt =
    {
       std::clamp(screenCell.x + (viewState.mapOffset.x / renderTileSize), 0, SimWidth - 1),
       std::clamp(screenCell.y + (viewState.mapOffset.y / renderTileSize), 0, SimHeight - 1)
    };

    viewState.tileHighlight =
    {
        (screenCell.x * renderTileSize) - viewState.mapOffset.x % renderTileSize,
        (screenCell.y * renderTileSize) - viewState.mapOffset.y % renderTileSize,
        renderTileSize, renderTileSize
    };

    appWindows.gameMiniMap->updateTilePointedAt(viewState.tilePointedAt);
}


// ── Mouse-up suppression ─────────────────────────────────────────────────────

/**
 * Returns true when a tool mouse-up should be ignored because the pointer is
 * inside a UI element or the drag started inside a window.
 */
static bool IgnoreToolMouseUp(MPoint<int>& mousePosition)
{
    for (auto rect : UiRects)
    {
        if (pointInRect(mousePosition, *rect))
        {
            return true;
        }
    }

    if (guiWindowStack.pointInWindow(EventHandling::MousePosition))
    {
        return true;
    }

    if (EventHandling::MouseDownPosition != EventHandling::MousePosition &&
        guiWindowStack.pointInWindow(EventHandling::MouseDownPosition))
    {
        return true;
    }

    return false;
}


// ── Mouse event handler ──────────────────────────────────────────────────────

void handleMouseEvent(SDL_Event& event)
{
    //dowId) { return; }

    Vector<int> mouseMotionDelta{};
    MPoint<int> mousePosition = {event.motion.x, event.motion.y};

    switch (event.type)
    {
    case SDL_MOUSEWHEEL:
        if (event.wheel.y < 0)
        {
            int newRenderTileSize = std::min(64, std::max(16, renderTileSize + 1));
            if (newRenderTileSize != renderTileSize)
            {
                renderTileSize = newRenderTileSize;
                setRenderTileSize();
            }
        }
        else {
            int newRenderTileSize = std::min(64, std::max(16, renderTileSize - 1));
            if (newRenderTileSize != renderTileSize)
            {
                renderTileSize = newRenderTileSize;
                setRenderTileSize();
            }
        }
        break;

    case SDL_MOUSEMOTION:
        EventHandling::MousePosition = { event.motion.x, event.motion.y };
        mouseMotionDelta = { event.motion.xrel, event.motion.yrel };

        viewState.draggableToolVector = {};
        if (pendingToolProperties().draggable && EventHandling::MouseLeftDown && toolStart() != viewState.tilePointedAt)
        {
            viewState.draggableToolVector = vectorFromPoints(toolStart(), viewState.tilePointedAt);
            validateDraggableToolVector(viewState.draggableToolVector, budget);
            executeDraggableTool(viewState.draggableToolVector, viewState.tilePointedAt, budget);
        }

        calculateMouseToWorld();

        guiWindowStack.injectMouseMotion(mousePosition);

        if ((SDL_GetMouseState(nullptr, nullptr) & SDL_BUTTON_RMASK) != 0)
        {
            viewState.mapOffset -= mouseMotionDelta;
            clampViewOffset();
            updateMapDrawParameters();
            RightButtonDrag = true;
        }

        if (pendingTool() == Tool::Query && pointInRect(mousePosition, FullMapViewportRect()))
        {
            queryTool(viewState.tilePointedAt.x, viewState.tilePointedAt.y, budget);
            guiWindowStack.bringToFront(appWindows.query.get());
            appWindows.query->setQueryResult(queryResult(), mousePosition);
            appWindows.query->show();
        }
        else
        {
            appWindows.query->hide();
        }

        break;

    case SDL_MOUSEBUTTONDOWN:
        if (event.button.button == SDL_BUTTON_LEFT)
        {
            EventHandling::MouseLeftDown = true;
            EventHandling::MouseDownPosition = { event.motion.x, event.motion.y };

            if (guiWindowStack.pointInWindow(mousePosition))
            {
                guiWindowStack.updateStack(mousePosition);
                guiWindowStack.front()->injectMouseDown(mousePosition);
                return;
            }

            toolStart(viewState.tilePointedAt);

            if (!pendingToolProperties().draggable && pointInRect(mousePosition, FullMapViewportRect()))
            {
                ToolDown(viewState.tilePointedAt, budget);
            }

            // Popup dismiss-button handling now lives in PopupWindow::onMouseDown.
            // Clicks on a visible popup are routed through guiWindowStack.
        }
        break;

    case SDL_MOUSEBUTTONUP:
        if (event.button.button == SDL_BUTTON_LEFT)
        {
            EventHandling::MouseLeftDown = false;
            EventHandling::MouseClickPosition = { event.button.x, event.button.y };

            guiWindowStack.injectMouseUp();

            if (IgnoreToolMouseUp(mousePosition))
            {
                return;
            }

            toolEnd(viewState.tilePointedAt);

            if (pendingToolProperties().draggable)
            {
                executeDraggableTool(viewState.draggableToolVector, viewState.tilePointedAt, budget);
            }
        }
        else if (event.button.button == SDL_BUTTON_RIGHT)
        {
            if (!RightButtonDrag)
            {
                pendingTool(Tool::None);
                appWindows.toolPalette->cancelTool();
            }
            RightButtonDrag = false;
        }
        break;

    default:
        break;
    }
}
