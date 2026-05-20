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
#include "Rectangle.h"
#include "Vector.h"
#include "Texture.h"

#include "SDL_include.h"

class WindowBase
{
public:
    WindowBase();
    WindowBase(const WindowBase&) = delete;
    const WindowBase& operator=(const WindowBase&) = delete;

    virtual ~WindowBase() = default;

    bool visible() const;

    void toggleVisible();

    void show();
    void hide();

    void move(const Vector<int>& delta);

    void position(const MPoint<int>& point);
    MPoint<int> position();

    void size(const Vector<int>& size);

    constexpr Vector<int> size() const
    {
        return mArea.size();
    }

    const MRectangle<int>& area() const;

    void anchor();

    void unanchor();

    bool anchored() const;

    void alwaysVisible(bool always);
    bool alwaysVisible() const;

    void injectMouseDown(const MPoint<int>& position);
    void injectMouseUp();
    void injectMouseMotion(const MPoint<int>& delta);

    void injectKeyDown(int32_t key);

    virtual void draw();
    virtual void update() = 0;

protected:
    const MRectangle<int>& clientArea() const;

    void closeButtonActive(bool show);
    
    MPoint<int> scalePoint(MPoint<int> srcPoint);
    SDL_Rect scaleRect(SDL_Rect srcRect, float scale);
    SDL_Rect globalScaleRect(SDL_Rect srcRect);

    MRectangle<int> mTitleBarArea;
    std::string mTitle;

private:
    virtual void onMouseDown(const MPoint<int>&) {};
    virtual void onMouseUp() {};
    virtual void onMouseMotion(const MPoint<int>&) {};

    /** Takes a keycode equivalent to SDL_Keycode */
    virtual void onKeyDown(int32_t key) {};

    virtual void onMoved(const Vector<int>&) {};
    virtual void onPositionChanged(const MPoint<int>&) {};

    virtual void onShow() {};
    virtual void onHide() {};

    MRectangle<int> mArea;
    MRectangle<int> mClientArea;
    MRectangle<int> mCloseButtonArea;

    bool mVisible{ false };
    bool mAlwaysVisible{ false };
    bool mAnchored{ false };
    bool mDragging{ false };
    bool mCloseButtonActive{ true };
    
    MPoint<int> lastMousePosition;
    
    static bool UILoaded;
    static Texture mTitleBarTexture;
    static Texture mCloseButton;
    SDL_Rect mCloseRect;
};
