// This file is part of Micropolis-SDL2PP
// Micropolis-SDL2PP is based on Micropolis
//
// Copyright © 2025 - 2026 Sylvain Nowé
//
// Micropolis-SDL2PP is free software; you can redistribute it and/or modify
// it under the terms of the GNU GPLv3, with additional terms. See the README
// file, included in this distribution, for details.
#include "SaveAsWindow.h"

#include "main.h"
#include "AppWindows.h"
#include "CityProperties.h"
#include "FontSet.h"
#include "s_fileio.h"

#include "SDL_include.h"

#include <algorithm>
#include <cctype>
#include <cstring>


namespace
{
    // ── Layout (design-space rects inside a 400×180 canvas) ──────────────────
    constexpr int DesignW = 400;
    constexpr int DesignH = 180;

    // Title bar lives in the first ~24 px (handled by WindowBase). Everything
    // below is the client area we lay out here.
    constexpr SDL_Rect LabelAnchor   {  20,  44, 0,   0  };
    constexpr SDL_Rect TextFieldRect {  20,  68, 360, 32 };
    constexpr SDL_Rect SaveButton    {  90, 120, 100, 36 };
    constexpr SDL_Rect CancelButton  { 210, 120, 100, 36 };

    constexpr SDL_Color Black { 0,   0,   0,   255 };

    char keyToPrintable(int32_t key, bool shift)
    {
        if (key >= SDLK_a && key <= SDLK_z)
        {
            return static_cast<char>((shift ? 'A' : 'a') + (key - SDLK_a));
        }
        if (key >= SDLK_0 && key <= SDLK_9)
        {
            static const char shifted[] = ")!@#$%^&*(";
            return shift ? shifted[key - SDLK_0] : static_cast<char>('0' + (key - SDLK_0));
        }
        if (key >= SDLK_KP_1 && key <= SDLK_KP_9)
        {
            return static_cast<char>('1' + (key - SDLK_KP_1));
        }
        switch (key)
        {
        case SDLK_SPACE:        return ' ';
        case SDLK_MINUS:        return shift ? '_' : '-';
        case SDLK_EQUALS:       return shift ? '+' : '=';
        case SDLK_PERIOD:       return shift ? '>' : '.';
        case SDLK_COMMA:        return shift ? '<' : ',';
        case SDLK_KP_0:         return '0';
        case SDLK_KP_PERIOD:    return '.';
        case SDLK_KP_MINUS:     return '-';
        case SDLK_KP_PLUS:      return '+';
        case SDLK_KP_EQUALS:    return '=';
        default:                return 0;
        }
    }

    // Filename hygiene — strips path separators and other characters that would
    // produce an invalid file on Windows. Whitespace at the ends is trimmed.
    std::string sanitizeForFilename(const std::string& s)
    {
        std::string out;
        out.reserve(s.size());
        for (char c : s)
        {
            const unsigned char uc = static_cast<unsigned char>(c);
            if (uc < 0x20)              { continue; }
            if (std::strchr("\\/:*?\"<>|", c)) { continue; }
            out.push_back(c);
        }
        const auto notSpace = [](unsigned char c) { return !std::isspace(c); };
        out.erase(out.begin(), std::find_if(out.begin(), out.end(), notSpace));
        out.erase(std::find_if(out.rbegin(), out.rend(), notSpace).base(), out.end());
        return out;
    }
}
// ─────────────────────────────────────────────────────────────────────────────


SaveAsWindow::SaveAsWindow(SDL_Renderer* renderer) :
    mRenderer{ renderer }
{
    size({ DesignW, DesignH });
    mTitle = "Save City As";
    closeButtonActive(true);
}


void SaveAsWindow::setCityName(const std::string& name)
{
    mTextFieldContent = name;
    mTextFieldFocused = true;
    mCaretBlinkStart  = SDL_GetTicks();
}


void SaveAsWindow::onShow()
{
    // The text field always starts focused so users can type immediately.
    mTextFieldFocused = true;
    mCaretBlinkStart  = SDL_GetTicks();
}


SDL_Rect SaveAsWindow::scaledRect(const SDL_Rect& designRect) const
{
    const float sx = static_cast<float>(area().width)  / static_cast<float>(DesignW);
    const float sy = static_cast<float>(area().height) / static_cast<float>(DesignH);
    return {
        area().x + static_cast<int>(designRect.x * sx),
        area().y + static_cast<int>(designRect.y * sy),
        static_cast<int>(designRect.w * sx),
        static_cast<int>(designRect.h * sy)
    };
}


void SaveAsWindow::draw()
{
    WindowBase::draw();

    Font* labelFont = fonts.title.get();
    Font* bodyFont  = fonts.body.get();
    SDL_SetTextureColorMod(labelFont->texture(), Black.r, Black.g, Black.b);
    SDL_SetTextureColorMod(bodyFont->texture(),  Black.r, Black.g, Black.b);

    // ── "City Name:" label ──────────────────────────────────────────────────
    const SDL_Rect labelAnchor = scaledRect(LabelAnchor);
    fonts.renderer->drawString(*labelFont, "City Name:", { labelAnchor.x, labelAnchor.y });

    // ── Text field ──────────────────────────────────────────────────────────
    const SDL_Rect fieldRect = scaledRect(TextFieldRect);

    SDL_SetRenderDrawColor(mRenderer, 255, 255, 255, 255);
    SDL_RenderFillRect(mRenderer, &fieldRect);

    if (mTextFieldFocused) { SDL_SetRenderDrawColor(mRenderer,  60,  90, 200, 255); }
    else                   { SDL_SetRenderDrawColor(mRenderer, 127, 127, 127, 255); }
    SDL_RenderDrawRect(mRenderer, &fieldRect);
    const SDL_Rect innerBorder{ fieldRect.x + 1, fieldRect.y + 1, fieldRect.w - 2, fieldRect.h - 2 };
    SDL_RenderDrawRect(mRenderer, &innerBorder);

    const int padX  = static_cast<int>(6.0f * (static_cast<float>(fieldRect.w) / TextFieldRect.w));
    const int textY = fieldRect.y + fieldRect.h / 2 - bodyFont->height() / 2;
    const int textX = fieldRect.x + padX;

    if (!mTextFieldContent.empty())
    {
        fonts.renderer->drawString(*bodyFont, mTextFieldContent, { textX, textY });
    }

    if (mTextFieldFocused)
    {
        const Uint32 elapsed = SDL_GetTicks() - mCaretBlinkStart;
        const bool caretVisible = ((elapsed / 500u) % 2u) == 0u;
        if (caretVisible)
        {
            const int caretX = textX + (mTextFieldContent.empty() ? 0 : bodyFont->width(mTextFieldContent)) + 1;
            SDL_SetRenderDrawColor(mRenderer, 0, 0, 0, 255);
            SDL_RenderDrawLine(mRenderer, caretX, fieldRect.y + 4, caretX, fieldRect.y + fieldRect.h - 5);
        }
    }

    // ── Save / Cancel buttons ───────────────────────────────────────────────
    auto drawButton = [&](const SDL_Rect& designBtn, const char* label, bool hovered, bool disabled) {
        const SDL_Rect btn = scaledRect(designBtn);

        if (disabled)      { SDL_SetRenderDrawColor(mRenderer, 220, 220, 220, 255); }
        else if (hovered)  { SDL_SetRenderDrawColor(mRenderer, 200, 215, 245, 255); }
        else               { SDL_SetRenderDrawColor(mRenderer, 230, 230, 230, 255); }
        SDL_RenderFillRect(mRenderer, &btn);

        SDL_SetRenderDrawColor(mRenderer, disabled ? 170 : 100, disabled ? 170 : 100, disabled ? 170 : 100, 255);
        SDL_RenderDrawRect(mRenderer, &btn);

        const int tx = btn.x + btn.w / 2 - labelFont->width(label) / 2;
        const int ty = btn.y + btn.h / 2 - labelFont->height() / 2;
        const Uint8 g = disabled ? 130 : 0;
        SDL_SetTextureColorMod(labelFont->texture(), g, g, g);
        fonts.renderer->drawString(*labelFont, label, { tx, ty });
    };

    const bool saveDisabled = sanitizeForFilename(mTextFieldContent).empty();
    drawButton(SaveButton,   "Save",   mSaveHovered && !saveDisabled, saveDisabled);
    drawButton(CancelButton, "Cancel", mCancelHovered,                false);
}


void SaveAsWindow::onMouseDown(const MPoint<int>& point)
{
    const SDL_Point clickPoint{ point.x, point.y };

    const SDL_Rect fieldRect  = scaledRect(TextFieldRect);
    const SDL_Rect saveBtn    = scaledRect(SaveButton);
    const SDL_Rect cancelBtn  = scaledRect(CancelButton);

    // Focus toggles based on whether the click lands inside the text field.
    const bool wasFocused = mTextFieldFocused;
    mTextFieldFocused = SDL_PointInRect(&clickPoint, &fieldRect);
    if (mTextFieldFocused && !wasFocused) { mCaretBlinkStart = SDL_GetTicks(); }

    if (SDL_PointInRect(&clickPoint, &saveBtn))
    {
        commitSave();
        return;
    }

    if (SDL_PointInRect(&clickPoint, &cancelBtn))
    {
        hide();
        return;
    }
}


void SaveAsWindow::onMouseMotion(const MPoint<int>& point)
{
    const SDL_Point sdlPoint{ point.x, point.y };
    const SDL_Rect saveBtn   = scaledRect(SaveButton);
    const SDL_Rect cancelBtn = scaledRect(CancelButton);
    mSaveHovered   = SDL_PointInRect(&sdlPoint, &saveBtn);
    mCancelHovered = SDL_PointInRect(&sdlPoint, &cancelBtn);
}


void SaveAsWindow::onKeyDown(int32_t key)
{
    if (key == SDLK_ESCAPE)
    {
        hide();
        return;
    }
    if (key == SDLK_RETURN || key == SDLK_KP_ENTER)
    {
        commitSave();
        return;
    }
    if (!mTextFieldFocused) { return; }

    if (key == SDLK_BACKSPACE)
    {
        if (!mTextFieldContent.empty()) { mTextFieldContent.pop_back(); }
        mCaretBlinkStart = SDL_GetTicks();
        return;
    }

    const bool shift = (SDL_GetModState() & KMOD_SHIFT) != 0;
    const char ch = keyToPrintable(key, shift);
    if (ch != 0)
    {
        mTextFieldContent.push_back(ch);
        mCaretBlinkStart = SDL_GetTicks();
    }
}


void SaveAsWindow::commitSave()
{
    const std::string cleaned = sanitizeForFilename(mTextFieldContent);
    if (cleaned.empty()) { return; }  // refuse to save with an empty / invalid name

    cityProperties.CityName(cleaned);
    const std::string path = "cities\\" + cleaned + ".cty";
    SaveCity(path, cityProperties, budget, gameOptions);

    hide();
}
