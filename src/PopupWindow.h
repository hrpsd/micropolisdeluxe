// This file is part of Micropolis-SDL2PP
// Micropolis-SDL2PP is based on Micropolis
//
// Copyright © 2025 - 2026 Sylvain Nowé
//
// Micropolis-SDL2PP is free software; you can redistribute it and/or modify
// it under the terms of the GNU GPLv3, with additional terms. See the README
// file, included in this distribution, for details.
#pragma once

#include "WindowBase.h"

#include "Point.h"
#include "SDL_include.h"


/**
 * Notification / scenario-pitch popup, expressed as a regular WindowBase-derived
 * window so it participates in the standard window stack (mouse routing,
 * stacking order, hit-testing) and uses the inherited window chrome.
 *
 * Two visual modes, selected by message id:
 *   id >= -8  : centered "scenario pitch" popup that pauses the simulation
 *   id <  -8  : bottom-bar notification (with optional minimap thumbnail)
 *
 * Chrome:
 *   - Title bar with the popup title text and a close button (handled by
 *     WindowBase). Drag the title bar to move the popup.
 *   - A red "Dismiss" button strip rendered at the bottom of the client area;
 *     clicking it plays the dismissal sound and hides the popup.
 *   - The inherited close button silently hides the popup (no sound). Both
 *     paths funnel through onHide() so popup state is always cleared.
 */
class PopupWindow : public WindowBase
{
public:
    PopupWindow() = delete;
    PopupWindow(const PopupWindow&) = delete;
    const PopupWindow& operator=(const PopupWindow&) = delete;

    explicit PopupWindow(SDL_Renderer* renderer);
    ~PopupWindow() override;

    void draw() override;
    void update() override {}

    /** Compose a popup for the supplied message id. `force` rebuilds even when
     *  another popup is already showing — used by the resize handler. */
    void showMessage(int messageId, bool force = false);

    /** Dismiss the popup (hide + clear state). */
    void dismiss();

    /** Active message id; 0 means no popup is currently displayed. */
    int messageId() const { return mMessageId; }

    /** Scenario popups suspend the simulation; bottom-bar notifications don't. */
    bool suspendsSimulation() const { return mMessageId != 0 && mMessageId >= -8; }

    /** Tile coordinate that the "auto-goto" thumbnail focuses on. */
    void gotoCoords(int x, int y) { mGotoX = x; mGotoY = y; }
    int  gotoX() const { return mGotoX; }
    int  gotoY() const { return mGotoY; }

private:
    void onMouseDown(const MPoint<int>&) override;
    void onHide() override;

    /** Recomputes window position + size from the current text texture and
     *  viewport. Called from showMessage() (new popup) and from the resize
     *  handler — NOT every frame, so user-dragged positions are preserved. */
    void recomputeLayout();

    SDL_Renderer* mRenderer{ nullptr };

    int          mMessageId{ 0 };
    SDL_Texture* mTexture{ nullptr };
    char*        mTitleText{ nullptr };         ///< raw title string (also mirrored into inherited mTitle)
    SDL_Rect     mTextRect{};                   ///< width/height of the rendered text texture
    SDL_Rect     mButtonRect{};                 ///< Dismiss-button rect, refreshed each frame (used for hit-test)
    MPoint<int>  mNotificationLocation{ -1, -1 };
    int          mGotoX{ -1 };
    int          mGotoY{ -1 };
};
