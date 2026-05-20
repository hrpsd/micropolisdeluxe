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

#include "SDL_include.h"

#include <string>


/**
 * Small modal-style centered window that lets the user edit a city name and
 * save the current city to "cities/<name>.cty".
 *
 * Layout is expressed in a fixed 400×180 design space; rectangles are scaled
 * each frame against the actual window size so the dialog tracks renderer
 * resizes the same way other UI windows do.
 */
class SaveAsWindow : public WindowBase
{
public:
    SaveAsWindow() = delete;
    SaveAsWindow(const SaveAsWindow&) = delete;
    const SaveAsWindow& operator=(const SaveAsWindow&) = delete;

    explicit SaveAsWindow(SDL_Renderer* renderer);

    void draw() override;
    void update() override {}

    /** Called from the menu handler before showing the window. Pre-fills the
     *  text field with the supplied default (usually the current city name). */
    void setCityName(const std::string& name);

private:
    void onMouseDown(const MPoint<int>&) override;
    void onMouseMotion(const MPoint<int>&) override;
    void onKeyDown(int32_t key) override;
    void onShow() override;

    SDL_Rect scaledRect(const SDL_Rect& designRect) const;

    void commitSave();

    SDL_Renderer* mRenderer{ nullptr };

    std::string mTextFieldContent;
    bool        mTextFieldFocused{ true };
    Uint32      mCaretBlinkStart{ 0 };

    bool mSaveHovered{ false };
    bool mCancelHovered{ false };
};
