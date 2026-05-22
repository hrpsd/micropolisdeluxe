// This file is part of Micropolis-SDL2PP
// Micropolis-SDL2PP is based on Micropolis
//
// Copyright © 2025 - 2026 Sylvain Nowé
//
// Micropolis-SDL2PP is free software; you can redistribute it and/or modify
// it under the terms of the GNU GPLv3, with additional terms. See the README
// file, included in this distribution, for details.

#include "main.h"
#include "AppWindows.h"
#include "GameOptions.h"
#include "MiniMapWindow.h"
#include "Texture.h"
#include "s_msg.h"
#include "w_sound.h"

#include "SDL_include.h"

#include <cmath>
#include <cstdlib>
#include <cstring>


namespace
{
    constexpr SDL_Color Black { 0,   0,   0,   255 };
    constexpr SDL_Color Gray  { 127, 127, 127, 255 };
    constexpr SDL_Color Red   { 255, 0,   0,   255 };

    // Matches WindowBase::TitleBarHeight (kept private over there). Used both
    // for the inherited title-bar strip at the top of the window and for the
    // Dismiss-button strip at the bottom so the popup looks balanced.
    constexpr int TitleBarHeight = 24;
}


PopupWindow::PopupWindow(SDL_Renderer* renderer) :
    mRenderer{ renderer }
{
    closeButtonActive(true);
    alwaysVisible(true);   // ESC / guiWindowStack.hide() must not auto-dismiss; close button + Dismiss handle it.
    // No anchor() call — popup is now draggable via its title bar.
    mTitle = "Notice";
}


PopupWindow::~PopupWindow()
{
    if (mTexture)   { SDL_DestroyTexture(mTexture); mTexture = nullptr; }
    if (mTitleText) { std::free(mTitleText); mTitleText = nullptr; }
}


void PopupWindow::showMessage(int messageId, bool force)
{
    // Per-game-option gating: suppress low-priority notification ids unless
    // notifications are enabled in the game options.
    if (messageId < -8 && !gameOptions.mNotices)
    {
        return;
    }

    // Only build a new popup if nothing is currently displayed, or the caller
    // explicitly forces a rebuild (used on window resize).
    if (mMessageId != 0 && !force)
    {
        return;
    }

    // Find the message text in the popMsgs table.
    msgpr msg{};
    int i = -1;
    do {
        msg = popMsgs[++i];
    } while (msg.messageId != 0 && msg.messageId != messageId);

    if (msg.messageId == 0)
    {
        printf("Popup not found %d\n", messageId);
        return;
    }

    // Title up to the "- " separator becomes the WindowBase title;
    // the rest is rendered as the popup body.
    if (mTitleText) { std::free(mTitleText); mTitleText = nullptr; }
	#if defined(_WIN64)
    mTitleText = _strdup(popMsgs[i].text);
	#else
	mTitleText = strdup(popMsgs[i].text);
	#endif
    if (char* dash = strstr(mTitleText, "- ")) { *dash = 0; }
    mTitle = mTitleText;  // inherited WindowBase title text

	#if defined(_WIN64)
    char* msgTextOwned = _strdup(popMsgs[i].text);
	#else
	char* msgTextOwned = strdup(popMsgs[i].text);
	#endif
    char* bodyText = msgTextOwned;
    if (char* dash = strstr(bodyText, "- ")) { bodyText = dash + 2; }

    Font* font = (messageId >= -8) ? fonts.large.get() : fonts.body.get();

    int textWidth;
    if (messageId < -8)
    {
        textWidth = (MessageLocation() != MPoint<int>{ -1, -1 })
            ? (int)std::roundf(MiniMapSize().x / 2.0f / 1.15f)
            : (int)std::roundf(MiniMapSize().x / 1.15f);
    }
    else
    {
        textWidth = (int)std::roundf(viewState.windowSize.x / 3.0f);
    }

    SDL_Surface* surface = TTF_RenderText_Blended_Wrapped(
        font->mFontInfo.font, bodyText, Black, textWidth);
    std::free(msgTextOwned);

    if (!surface)
    {
        printf("Failed to render popup text: %s\n", TTF_GetError());
        return;
    }

    if (mTexture) { SDL_DestroyTexture(mTexture); }
    mTextRect.w = surface->w;
    mTextRect.h = surface->h;
    mTexture    = SDL_CreateTextureFromSurface(mRenderer, surface);
    SDL_FreeSurface(surface);

    mNotificationLocation = MessageLocation();
    mMessageId            = messageId;

    recomputeLayout();
    show();
    guiWindowStack.bringToFront(this);
}


void PopupWindow::dismiss()
{
    // hide() routes through WindowBase::hide() → onHide(), which clears state
    // (same path used by the inherited close-button click handler).
    MakeSoundOn(nullptr, "edit", "Layzone");
    hide();
}


void PopupWindow::onHide()
{
    // Single cleanup path — covers both the Dismiss button (via dismiss())
    // and the inherited close button (handled in WindowBase::injectMouseDown).
    mMessageId = 0;
    if (mTexture)   { SDL_DestroyTexture(mTexture); mTexture = nullptr; }
    if (mTitleText) { std::free(mTitleText); mTitleText = nullptr; }
    mNotificationLocation = { -1, -1 };
    mTitle = "Notice";
}


void PopupWindow::recomputeLayout()
{
    if (mMessageId == 0) { return; }

    // The window is laid out as:
    //   ┌──────────────────────────┐
    //   │ Title bar (WindowBase)   │ ← titleBarH (inherited chrome)
    //   ├──────────────────────────┤
    //   │ Body text                │ ← bodyH
    //   ├──────────────────────────┤
    //   │ Dismiss button           │ ← buttonH
    //   └──────────────────────────┘
    const int titleBarH = (int)std::roundf(TitleBarHeight * scale);
    const int buttonH   = titleBarH;

    int bodyW, bodyH;
    if (mMessageId >= -8)
    {
        bodyW = (int)std::roundf(mTextRect.w * 1.15f);
        bodyH = (int)std::roundf(mTextRect.h * 1.15f);
    }
    else
    {
        bodyW = MiniMapSize().x;
        bodyH = (int)std::roundf(mTextRect.h * 1.15f);
    }

    const int totalW = bodyW;
    const int totalH = titleBarH + bodyH + buttonH;

    int x, y;
    if (mMessageId >= -8)
    {
        // Centered scenario popup.
        x = (viewState.windowSize.x - totalW) / 2;
        y = (viewState.windowSize.y - totalH) / 2;
    }
    else
    {
        // Bottom-bar notification.
        x = 0;
        y = viewState.windowSize.y - totalH;
    }

    position({ x, y });
    size({ totalW, totalH });
}


void PopupWindow::draw()
{
    if (mMessageId == 0) { return; }

    // Inherited WindowBase chrome: white fill, title bar, close button, border,
    // title-bar text. WindowBase::draw() reads mArea / mTitleBarArea so it
    // tracks dragging automatically.
    WindowBase::draw();

    // Body and Dismiss button live inside area(), derived from the current
    // (possibly dragged) window position each frame.
    const int titleBarH = mTitleBarArea.height;
    const int buttonH   = titleBarH;

    const SDL_Rect bodyRect{
        area().x,
        area().y + titleBarH,
        area().width,
        area().height - titleBarH - buttonH
    };

    // Body inner frame (subtle separator under the title bar).
    SDL_SetRenderDrawColor(mRenderer, Gray.r, Gray.g, Gray.b, Gray.a);
    SDL_RenderDrawRect(mRenderer, &bodyRect);

    // Body text — vertically centred. Scenario popup keeps the text
    // horizontally centred; notification mode preserves the original
    // off-centre placement (so the minimap thumbnail has room when present).
    if (mTexture)
    {
        SDL_Rect textDst{ 0, 0, mTextRect.w, mTextRect.h };
        if (mMessageId >= -8)
        {
            textDst.x = bodyRect.x + (bodyRect.w - mTextRect.w) / 2;
        }
        else
        {
            const float widthMul = (mNotificationLocation != MPoint<int>{ -1, -1 }) ? 0.5f : 1.075f;
            textDst.x = bodyRect.x + bodyRect.w / 2 - (int)std::roundf(mTextRect.w * widthMul);
        }
        textDst.y = bodyRect.y + (bodyRect.h - mTextRect.h) / 2;
        SDL_RenderCopy(mRenderer, mTexture, nullptr, &textDst);
    }

    // Dismiss button at the bottom of the window.
    mButtonRect = SDL_Rect{
        area().x,
        area().y + area().height - buttonH,
        area().width,
        buttonH
    };

    SDL_SetRenderDrawColor(mRenderer, Red.r, Red.g, Red.b, Red.a);
    SDL_RenderFillRect(mRenderer, &mButtonRect);
    SDL_SetRenderDrawColor(mRenderer, Gray.r, Gray.g, Gray.b, Gray.a);
    SDL_RenderDrawRect(mRenderer, &mButtonRect);

    Font* buttonFont = (mMessageId >= -8) ? fonts.large.get() : fonts.body.get();
    SDL_SetTextureColorMod(buttonFont->texture(), 0, 0, 0);
    const auto dismissSize = buttonFont->size("Dismiss");
    fonts.renderer->drawString(
        *buttonFont, "Dismiss",
        { mButtonRect.x + (mButtonRect.w - dismissSize.x) / 2,
          mButtonRect.y + (mButtonRect.h - dismissSize.y) / 2 });

    // Optional minimap thumbnail of the goto target, inside the body rect.
    if (mNotificationLocation != MPoint<int>{ -1, -1 })
    {
        SDL_Rect dstRect{ bodyRect.x + 2, bodyRect.y + 2, bodyRect.w / 2 - 4, bodyRect.h - 4 };
        SDL_Rect srcRect = dstRect;
        srcRect.x = mGotoX * renderTileSize - srcRect.w / 2;
        srcRect.y = mGotoY * renderTileSize - srcRect.h / 2;
        SDL_RenderCopy(mRenderer, MainMapTexture.texture, &srcRect, &dstRect);
    }
}


void PopupWindow::onMouseDown(const MPoint<int>& point)
{
    const SDL_Point pt{ point.x, point.y };
    if (SDL_PointInRect(&pt, &mButtonRect))
    {
        dismiss();
    }
    // Title-bar clicks are absorbed by WindowBase::injectMouseDown() before
    // they reach onMouseDown(), so dragging works automatically.
}
